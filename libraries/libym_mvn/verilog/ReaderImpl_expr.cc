
/// @file libym_mvn/verilog/ReaderImpl.cc
/// @brief ReaderImpl の実装クラス
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010 Yusuke Matsunaga
/// All rights reserved.


#include "ReaderImpl.h"
#include "DeclMap.h"
#include "Driver.h"
#include "Env.h"
#include "ym_mvn/MvMgr.h"
#include "ym_mvn/MvModule.h"
#include "ym_mvn/MvPort.h"
#include "ym_mvn/MvNode.h"
#include "ym_verilog/BitVector.h"
#include "ym_verilog/vl/VlDecl.h"
#include "ym_verilog/vl/VlExpr.h"
#include "ym_verilog/vl/VlRange.h"


BEGIN_NAMESPACE_YM_MVN_VERILOG

using namespace nsYm::nsVerilog;

// @brief 式に対応したノードの木を作る．
// @param[in] parent_module 親のモジュール
// @param[in] expr 式
// @param[in] env 環境
MvNode*
ReaderImpl::gen_expr(MvModule* parent_module,
		     const VlExpr* expr,
		     const Env& env)
{
  if ( expr->is_const() ) {
    assert_cond( is_bitvector_type(expr->value_type()), __FILE__, __LINE__);
    BitVector bv;
    //expr->eval_bitvector(bv);
    ymuint bit_size = bv.size();
    ymuint n = (bit_size + 31) / 32;
    vector<ymuint32> tmp(n);
    for (ymuint i = 0; i < bit_size; ++ i) {
      ymuint blk = i / 32;
      ymuint pos = i % 32;
      switch ( bv.value(i) ) {
      case kVpiScalar1: tmp[blk] |= (1 << pos); break;
      case kVpiScalar0: break;
      case kVpiScalarX:
      case kVpiScalarZ:
	mMsgMgr.put_msg(__FILE__, __LINE__,
			expr->file_region(),
			kMsgError,
			"MVN_VLXXX",
			"'X' or 'Z' in constant expression");
	break;
      }
    }
    return mMvMgr->new_const(parent_module, bit_size, tmp);
  }
  if ( expr->is_operation() ) {
    ymuint n = expr->operand_num();
    vector<MvNode*> inputs(n);
    for (ymuint i = 0; i < n; ++ i) {
      inputs[i] = gen_expr(parent_module, expr->operand(i), env);
    }

    if ( expr->op_type() == kVpiMultiConcatOp ) {
      ymint r = expr->rep_num();
      ymuint n1 = n - 1;
      vector<ymuint> bw_array(n * r);
      for (ymint j = 0; j < r; ++ j) {
	ymuint base = j * r;
	for (ymuint i = 0; i < n1; ++ i) {
	  bw_array[base + i] = inputs[i + 1]->output(0)->bit_width();
	}
      }
      MvNode* node = mMvMgr->new_concat(parent_module, bw_array);
      for (ymint j = 0; j < r; ++ j) {
	ymuint base = j * r;
	for (ymuint i = 0; i < n1; ++ i) {
	  mMvMgr->connect(inputs[i + 1], 0, node, base + i);
	}
      }
      return node;
    }
    return gen_opr(parent_module, expr->op_type(), inputs, expr->bit_size());
  }

  MvNode* node = gen_primary(expr, env);
  if ( expr->is_primary() ) {
    return node;
  }
  if ( expr->is_bitselect() ) {
    if ( expr->is_constant_select() ) {
      const VlDeclBase* decl = expr->decl_base();
      ymuint bitpos = decl->bit_offset(expr->index_val());
      const MvOutputPin* pin = node->output(0);
      MvNode* node1 = mMvMgr->new_constbitselect(parent_module,
						 bitpos,
						 pin->bit_width());
      assert_cond( node, __FILE__, __LINE__);
      mMvMgr->connect(node, 0, node1, 0);
      return node1;
    }
    else {
#warning "TODO: [msb:lsb] のオフセット変換をしていない"
      MvNode* node1 = gen_expr(parent_module, expr->index(), env);
      const MvOutputPin* pin0 = node->output(0);
      const MvOutputPin* pin1 = node1->output(0);
      MvNode* node2 = mMvMgr->new_bitselect(parent_module,
					    pin0->bit_width(),
					    pin1->bit_width());
      mMvMgr->connect(node, 0, node2, 0);
      mMvMgr->connect(node1, 0, node2, 1);
      return node2;
    }
  }
  if ( expr->is_partselect() ) {
    if ( expr->is_constant_select() ) {
      const VlDeclBase* decl = expr->decl_base();
      ymuint msb = decl->bit_offset(expr->left_range_val());
      ymuint lsb = decl->bit_offset(expr->right_range_val());
      const MvOutputPin* pin = node->output(0);
      MvNode* node1 = mMvMgr->new_constpartselect(parent_module,
						  msb, lsb,
						  pin->bit_width());
      mMvMgr->connect(node, 0, node1, 0);
      return node1;
    }
    else {
#warning "TODO: [msb:lsb] のオフセット変換をしていない"
      // まだできてない．
      // というか可変 part_select は VPI がおかしいと思う．
      assert_not_reached(__FILE__, __LINE__);
      return NULL;
    }
  }
  assert_not_reached(__FILE__, __LINE__);
  return NULL;
}

// @brief 演算に対応したノードの木を作る．
// @param[in] parent_module 親のモジュール
// @param[in] op_type 演算の種類
// @param[in] operand_array オペランドに対応するノードの配列
// @param[in] out_bw 出力のビット幅
MvNode*
ReaderImpl::gen_opr(MvModule* parent_module,
		    tVpiOpType op_type,
		    const vector<MvNode*>& operand_array,
		    ymuint out_bw)
{
  ymuint n = operand_array.size();

  switch ( op_type ) {
  case kVpiNullOp:
    return operand_array[0];

  case kVpiMinusOp:
    {
      MvNode* node = mMvMgr->new_cmpl(parent_module, out_bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      return node;
    }

  case kVpiNotOp:
    {
      MvNode* node = mMvMgr->new_not(parent_module, 1);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      return node;
    }

  case kVpiBitNegOp:
    {
      MvNode* node = mMvMgr->new_not(parent_module, out_bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      return node;
    }

  case kVpiPlusOp:
    return operand_array[0];

  case kVpiUnaryAndOp:
    {
      ymuint bw = operand_array[0]->output(0)->bit_width();
      MvNode* node = mMvMgr->new_rand(parent_module, bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      return node;
    }

  case kVpiUnaryNandOp:
    {
      ymuint bw = operand_array[0]->output(0)->bit_width();
      MvNode* node = mMvMgr->new_rand(parent_module, bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      MvNode* node1 = mMvMgr->new_not(parent_module, 1);
      mMvMgr->connect(node, 0, node1, 0);
      return node1;
    }

  case kVpiUnaryOrOp:
    {
      ymuint bw = operand_array[0]->output(0)->bit_width();
      MvNode* node = mMvMgr->new_ror(parent_module, bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      return node;
    }

  case kVpiUnaryNorOp:
    {
      ymuint bw = operand_array[0]->output(0)->bit_width();
      MvNode* node = mMvMgr->new_ror(parent_module, bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      MvNode* node1 = mMvMgr->new_not(parent_module, 1);
      mMvMgr->connect(node, 0, node1, 0);
      return node1;
    }

  case kVpiUnaryXorOp:
    {
      ymuint bw = operand_array[0]->output(0)->bit_width();
      MvNode* node = mMvMgr->new_rxor(parent_module, bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      return node;
    }

  case kVpiUnaryXNorOp:
    {
      ymuint bw = operand_array[0]->output(0)->bit_width();
      MvNode* node = mMvMgr->new_rxor(parent_module, bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      MvNode* node1 = mMvMgr->new_not(parent_module, 1);
      mMvMgr->connect(node, 0, node1, 0);
      return node1;
    }

  case kVpiPosedgeOp:
  case kVpiNegedgeOp:
    assert_not_reached(__FILE__, __LINE__);
    break;

  case kVpiAddOp:
    {
      ymuint bw1 = operand_array[0]->output(0)->bit_width();
      ymuint bw2 = operand_array[1]->output(0)->bit_width();
      ymuint bw3 = out_bw;
      MvNode* node = mMvMgr->new_add(parent_module, bw1, bw2, bw3);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiSubOp:
    {
      ymuint bw1 = operand_array[0]->output(0)->bit_width();
      ymuint bw2 = operand_array[1]->output(0)->bit_width();
      ymuint bw3 = out_bw;
      MvNode* node = mMvMgr->new_sub(parent_module, bw1, bw2, bw3);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiMultOp:
    {
      ymuint bw1 = operand_array[0]->output(0)->bit_width();
      ymuint bw2 = operand_array[1]->output(0)->bit_width();
      ymuint bw3 = out_bw;
      MvNode* node = mMvMgr->new_mult(parent_module, bw1, bw2, bw3);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiDivOp:
    {
      ymuint bw1 = operand_array[0]->output(0)->bit_width();
      ymuint bw2 = operand_array[1]->output(0)->bit_width();
      ymuint bw3 = out_bw;
      MvNode* node = mMvMgr->new_div(parent_module, bw1, bw2, bw3);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiModOp:
    {
      ymuint bw1 = operand_array[0]->output(0)->bit_width();
      ymuint bw2 = operand_array[1]->output(0)->bit_width();
      ymuint bw3 = out_bw;
      MvNode* node = mMvMgr->new_mod(parent_module, bw1, bw2, bw3);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiPowerOp:
    {
      ymuint bw1 = operand_array[0]->output(0)->bit_width();
      ymuint bw2 = operand_array[1]->output(0)->bit_width();
      ymuint bw3 = out_bw;
      MvNode* node = mMvMgr->new_pow(parent_module, bw1, bw2, bw3);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiLShiftOp:
    {
      ymuint bw1 = operand_array[0]->output(0)->bit_width();
      ymuint bw2 = operand_array[1]->output(0)->bit_width();
      ymuint bw3 = out_bw;
      MvNode* node = mMvMgr->new_sll(parent_module, bw1, bw2, bw3);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiRShiftOp:
    {
      ymuint bw1 = operand_array[0]->output(0)->bit_width();
      ymuint bw2 = operand_array[1]->output(0)->bit_width();
      ymuint bw3 = out_bw;
      MvNode* node = mMvMgr->new_srl(parent_module, bw1, bw2, bw3);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiArithLShiftOp:
    {
      ymuint bw1 = operand_array[0]->output(0)->bit_width();
      ymuint bw2 = operand_array[1]->output(0)->bit_width();
      ymuint bw3 = out_bw;
      MvNode* node = mMvMgr->new_sla(parent_module, bw1, bw2, bw3);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiArithRShiftOp:
    {
      ymuint bw1 = operand_array[0]->output(0)->bit_width();
      ymuint bw2 = operand_array[1]->output(0)->bit_width();
      ymuint bw3 = out_bw;
      MvNode* node = mMvMgr->new_sra(parent_module, bw1, bw2, bw3);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiBitAndOp:
    {
      ymuint bw = out_bw;
      MvNode* node = mMvMgr->new_and(parent_module, 2, bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiBitOrOp:
    {
      ymuint bw = out_bw;
      MvNode* node = mMvMgr->new_or(parent_module, 2, bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiBitXNorOp:
    {
      ymuint bw = out_bw;
      MvNode* node = mMvMgr->new_xor(parent_module, 2, bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      MvNode* node1 = mMvMgr->new_not(parent_module, bw);
      mMvMgr->connect(node, 0, node1, 0);
      return node1;
    }

  case kVpiBitXorOp:
    {
      ymuint bw = out_bw;
      MvNode* node = mMvMgr->new_xor(parent_module, 2, bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiLogAndOp:
    {
      MvNode* node = mMvMgr->new_and(parent_module, 2, 1);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiLogOrOp:
    {
      MvNode* node = mMvMgr->new_or(parent_module, 2, 1);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiCaseEqOp:
  case kVpiCaseNeqOp:
    assert_not_reached(__FILE__, __LINE__);
    break;

  case kVpiEqOp:
    {
      ymuint bw = operand_array[0]->output(0)->bit_width();
      MvNode* node = mMvMgr->new_equal(parent_module, bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiNeqOp:
    {
      ymuint bw = operand_array[0]->output(0)->bit_width();
      MvNode* node = mMvMgr->new_equal(parent_module, bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      MvNode* node1 = mMvMgr->new_not(parent_module, 1);
      mMvMgr->connect(node, 0, node1, 0);
      return node1;
    }

  case kVpiLtOp:
    {
      ymuint bw = operand_array[0]->output(0)->bit_width();
      MvNode* node = mMvMgr->new_lt(parent_module, bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      return node;
    }

  case kVpiGeOp:
    {
      ymuint bw = operand_array[0]->output(0)->bit_width();
      MvNode* node = mMvMgr->new_lt(parent_module, bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      MvNode* node1 = mMvMgr->new_not(parent_module, 1);
      mMvMgr->connect(node, 0, node1, 0);
      return node1;
    }

  case kVpiGtOp:
    {
      ymuint bw = operand_array[0]->output(0)->bit_width();
      MvNode* node = mMvMgr->new_lt(parent_module, bw);
      mMvMgr->connect(operand_array[1], 0, node, 0);
      mMvMgr->connect(operand_array[0], 0, node, 1);
      return node;
    }

  case kVpiLeOp:
    {
      ymuint bw = operand_array[0]->output(0)->bit_width();
      MvNode* node = mMvMgr->new_lt(parent_module, bw);
      mMvMgr->connect(operand_array[1], 0, node, 0);
      mMvMgr->connect(operand_array[0], 0, node, 1);
      MvNode* node1 = mMvMgr->new_not(parent_module, 1);
      mMvMgr->connect(node, 0, node1, 0);
      return node1;
    }

  case kVpiConditionOp:
    {
      ymuint bw = out_bw;
      MvNode* node = mMvMgr->new_ite(parent_module, bw);
      mMvMgr->connect(operand_array[0], 0, node, 0);
      mMvMgr->connect(operand_array[1], 0, node, 1);
      mMvMgr->connect(operand_array[2], 0, node, 2);
      return node;
    }

  case kVpiMinTypMaxOp:
    assert_not_reached(__FILE__, __LINE__);
    break;

  case kVpiConcatOp:
    {
      vector<ymuint> bw_array(n);
      for (ymuint i = 0; i < n; ++ i) {
	bw_array[i] = operand_array[i]->output(0)->bit_width();
      }
      MvNode* node = mMvMgr->new_concat(parent_module, bw_array);
      for (ymuint i = 0; i < n; ++ i) {
	mMvMgr->connect(operand_array[i], 0, node, i);
      }
      return node;
    }

  case kVpiMultiConcatOp:
    assert_not_reached(__FILE__, __LINE__);

  default:
    break;
  }
  assert_not_reached(__FILE__, __LINE__);
  return NULL;
}

// @brief 宣言要素への参照に対応するノードを作る．
// @param[in] expr 式
// @param[in] env 環境
MvNode*
ReaderImpl::gen_primary(const VlExpr* expr,
			const Env& env)
{
  const VlDecl* decl = expr->decl_obj();
  const VlDeclArray* declarray = expr->declarray_obj();
  if ( decl ) {
    assert_cond(expr->declarray_dimension() == 0, __FILE__, __LINE__);
    MvNode* node = env.get(decl);
    if ( node == NULL ) {
      cerr << decl->name() << " is not found in mGlobalEnv" << endl;
    }
    assert_cond( node != NULL, __FILE__, __LINE__);
    return node;
  }
  else if ( declarray ) {
    if ( expr->is_constant_select() ) {
      // インデックス固定の配列要素
      ymuint offset = expr->declarray_offset();
      MvNode* node = env.get(declarray, offset);
      if ( node == NULL ) {
	cerr << decl->name() << " is not found in mGlobalEnv" << endl;
      }
      assert_cond( node != NULL, __FILE__, __LINE__);
      return node;
    }
    else {
      // インデックス可変の配列要素
#if 0
      ymuint dim = expr->declarray_dimension();
      assert_cond( declarray->dimension() == dim, __FILE__, __LINE__);
      ymuint offset = 0;
      ymuint mlt = 1;
      for (ymuint i = 0; i < dim; ++ i) {
	const VlExpr* index = expr->declarray_index(i);
	int index_val;
	bool stat = index->eval_int(index_val);
	assert_cond( stat, __FILE__, __LINE__);
	offset += index_val * mlt;
	mlt *= declarray->range(i)->size();
      }
      MvNode* node = env.get(declarray, offset);
      if ( node == NULL ) {
	cerr << decl->name() << " is not found in mGlobalEnv" << endl;
      }
      assert_cond( node != NULL, __FILE__, __LINE__);
      return node;
#else
      assert_not_reached(__FILE__, __LINE__);
      return NULL;
#endif
    }
  }
  cout << "Error in " << expr->decompile() << endl;
  assert_not_reached(__FILE__, __LINE__);
  return NULL;
}

END_NAMESPACE_YM_MVN_VERILOG
