
/// @file libym_verilog/elaborator/ItemGen_taskfunc.cc
/// @brief ElbMgr の実装ファイル(タスク/関数の実体化)
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: ItemGen_taskfunc.cc 2507 2009-10-17 16:24:02Z matsunaga $
///
/// Copyright (C) 2005-2010 Yusuke Matsunaga
/// All rights reserved.


#include "ItemGen.h"
#include "ElbEnv.h"

#include "ym_verilog/BitVector.h"

#include "ym_verilog/pt/PtDecl.h"
#include "ym_verilog/pt/PtItem.h"
#include "ym_verilog/pt/PtExpr.h"
#include "ym_verilog/pt/PtMisc.h"

#include "ElbTaskFunc.h"
#include "ElbDecl.h"
#include "ElbExpr.h"

#include "ElbStub.h"


BEGIN_NAMESPACE_YM_VERILOG

BEGIN_NONAMESPACE

const int debug = 0;
#define dout cout

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// task/function 関係の instantiate 関数
//////////////////////////////////////////////////////////////////////

// @brief task/function の生成を行う．
// @param[in] parent 親のヘッダ
// @param[in] pt_item タスク/関数定義
void
ItemGen::phase1_tf(const VlNamedObj* parent,
		   const PtItem* pt_item)
{
  if ( debug ) {
    dout << endl
	 << "phase1_tf( "
	 << parent->full_name()
	 << " ["
	 << hex << reinterpret_cast<ympuint>(parent) << dec
	 << "], "
	 << pt_item->name() << " )" << endl
	 << endl;
  }
  
  ElbTask* task = NULL;
  ElbFunction* func = NULL;
  VlNamedObj* namedobj = NULL;

  if ( pt_item->type() == kPtItem_Task ) {
    task = factory().new_Task(parent, pt_item);
    reg_task(task);
    namedobj = task;
  }
  else {
    assert_cond( pt_item->type() == kPtItem_Func, __FILE__, __LINE__);

    const PtExpr* pt_left = pt_item->left_range();
    const PtExpr* pt_right = pt_item->right_range();

    ElbExpr* left = NULL;
    ElbExpr* right = NULL;
    int left_val = 0;
    int right_val = 0;

    if ( !instantiate_range(parent,
			    pt_left, pt_right,
			    left, right,
			    left_val, right_val) ) {
      return;
    }
    func = factory().new_Function(parent,
				  pt_item,
				  left, right,
				  left_val, right_val);
    reg_function(func);
    namedobj = func;
  }

  // parameter の生成
  instantiate_param(namedobj, pt_item->paramhead_array(), false);

  // localparam の生成
  instantiate_param(namedobj, pt_item->localparamhead_array(), true);
  
  // attribute instance の生成
  //instantiate_attribute(pt_item->attr_top(), false, namedobj);

  ostringstream buf;
  buf << "instantiating task/func : " << namedobj->full_name() << ".";
  put_msg(__FILE__, __LINE__,
	  pt_item->file_region(),
	  kMsgInfo,
	  "ELAB",
	  buf.str());

  // 本体のステートメント内部のスコープの生成
  const PtStmt* pt_body = pt_item->body();
  phase1_stmt(namedobj, pt_body);

  // 残りの仕事は phase2, phase3 で行う．
  add_phase2stub(make_stub(this, &ItemGen::phase2_tf,
			   task, func, pt_item));
  add_phase3stub(make_stub(this, &ItemGen::phase3_tf,
			   task, func, pt_item));

  if ( debug ) {
    dout << "phase1_tf end" << endl
	 << endl;
  }
}

// @param[in] task/function 内の宣言要素の生成を行う．
// @param[in] task タスク本体
// @param[in] func 関数本体
// @param[in] pt_item パース木のタスク/関数定義
void
ItemGen::phase2_tf(ElbTask* task,
		   ElbFunction* func,
		   const PtItem* pt_item)
{
  assert_cond( task == NULL || func == NULL, __FILE__, __LINE__);
  assert_cond( task != NULL || func != NULL, __FILE__, __LINE__);

  VlNamedObj* namedobj = task;
  if ( namedobj == NULL ) {
    namedobj = func;
  }

  if ( debug ) {
    dout << endl
	 << "phase2_tf( "
	 << namedobj->full_name()
	 << " ["
	 << hex << reinterpret_cast<ympuint>(namedobj) << dec
	 << "] )" << endl
	 << endl;
  }

  // 宣言要素の生成
  instantiate_decl(namedobj, pt_item->declhead_array());

  // 入出力の生成
  instantiate_iodecl(task, func, pt_item->iohead_array());

  if ( func ) {
    // 関数名と同名の変数の生成
    ElbExpr* left = func->_left_range();
    ElbExpr* right = func->_right_range();
    int left_val = func->left_range_const();
    int right_val = func->right_range_const();
    ElbDeclHead* head = factory().new_DeclHead(func, pt_item,
					       left, right,
					       left_val, right_val);
    ElbDecl* decl = factory().new_Decl(head, pt_item);
    int tag = vpiVariables;
    if ( pt_item->data_type() == kVpiVarNone ) {
      tag = vpiReg;
    }
    reg_decl(tag, decl);

    func->set_ovar(decl);
  }

  if ( debug ) {
    dout << "phase2_tf end" << endl
	 << endl;
  }
}

// @param[in] task/function 内のステートメントの生成を行う．
// @param[in] task task 本体
// @param[in] func 関数本体
// @param[in] pt_item パース木のタスク/関数定義
void
ItemGen::phase3_tf(ElbTask* task,
		   ElbFunction* func,
		   const PtItem* pt_item)
{
  assert_cond( task == NULL || func == NULL, __FILE__, __LINE__);
  assert_cond( task != NULL || func != NULL, __FILE__, __LINE__);

  VlNamedObj* namedobj = task;
  if ( namedobj == NULL ) {
    namedobj = func;
  }

  if ( debug ) {
    dout << endl
	 << "phase3_tf( "
	 << namedobj->full_name()
	 << " ["
	 << hex << reinterpret_cast<ympuint>(namedobj) << dec
	 << "] )" << endl
	 << endl;
  }

  // 本体のステートメントの生成
  ElbTfEnv env(task, func);
  const PtStmt* pt_body = pt_item->body();
  ElbStmt* body = instantiate_stmt(namedobj, NULL, env, pt_body);
  if ( body ) {
    if ( task ) {
      task->set_stmt(body);
    }
    else {
      func->set_stmt(body);
    }
  }

  if ( debug ) {
    dout << "phase3_tf end" << endl
	 << endl;
  }
}

// @brief constant function の生成を行う．
// @param[in] parent 親のスコープ
// @param[in] pt_function 関数定義
ElbFunction*
ItemGen::instantiate_constant_function(const VlNamedObj* parent,
				       const PtItem* pt_function)
{
  // 基本的には phase1_tf(), phase2_tf(), phase3_tf() と同じことを
  // やっているが，ElbConstantFunctionEnv を用いているところが異なる．
  if ( debug ) {
    dout << endl
	 << "instantiate_constant_function( "
	 << parent->full_name()
	 << " ["
	 << hex << reinterpret_cast<ympuint>(parent) << dec
	 << "] , "
	 << pt_function->name() << " )" << endl
	 << endl;
  }

  const PtExpr* pt_left = pt_function->left_range();
  const PtExpr* pt_right = pt_function->right_range();
  ElbExpr* left = NULL;
  ElbExpr* right = NULL;
  int left_val = 0;
  int right_val = 0;
  if ( !instantiate_range(parent, pt_left, pt_right,
			  left, right,
			  left_val, right_val) ) {
    return NULL;
  }
  ElbFunction* func = factory().new_Function(parent,
					     pt_function,
					     left, right,
					     left_val, right_val);
  // 登録しておく．
  reg_constant_function(parent, pt_function->name(), func);

  // parameter の生成
  instantiate_param(func, pt_function->paramhead_array(), false);

  // localparam の生成
  instantiate_param(func, pt_function->localparamhead_array(), true);

  // 宣言要素の生成
  instantiate_decl(func, pt_function->declhead_array());

  // 関数名と同名の変数の生成
  ElbDeclHead* head = factory().new_DeclHead(func, pt_function,
					     left, right,
					     left_val, right_val);
  ElbDecl* decl = factory().new_Decl(head, pt_function);
  int tag = vpiVariables;
  if ( pt_function->data_type() == kVpiVarNone ) {
    tag = vpiReg;
  }
  reg_decl(tag, decl);

  func->set_ovar(decl);

  // 入出力の生成
  instantiate_iodecl(NULL, func, pt_function->iohead_array());

  // 本体のステートメント内部のスコープの生成
  const PtStmt* pt_body = pt_function->body();
  phase1_stmt(func, pt_body, true);

  // 本体のステートメントの生成
  ElbConstantFunctionEnv env(func);
  ElbStmt* body = instantiate_stmt(func, NULL, env, pt_body);
  if ( body ) {
    func->set_stmt(body);
  }

  if ( debug ) {
    dout << "instantiate_constant_function end" << endl
	 << endl;
  }

  return func;
}

// @brief task/function 用のIO宣言要素をインスタンス化する．
// @brief IO宣言要素を実体化する．
// @param[in] task 親のタスク
// @param[in] func 親の function
// @param[in] pt_head_array IO宣言ヘッダの配列
void
ItemGen::instantiate_iodecl(ElbTask* task,
			    ElbFunction* func,
			    PtIOHeadArray pt_head_array)
{
  assert_cond( task != NULL || func != NULL, __FILE__, __LINE__);
  assert_cond( task == NULL || func == NULL, __FILE__, __LINE__);

  VlNamedObj* namedobj = task;
  if ( namedobj == NULL ) {
    namedobj = func;
  }

  ymuint index = 0;
  for (ymuint i = 0; i < pt_head_array.size(); ++ i) {
    const PtIOHead* pt_head = pt_head_array[i];
    tVpiAuxType def_aux_type = pt_head->aux_type();
    bool sign = pt_head->is_signed();

    const PtExpr* pt_left = pt_head->left_range();
    const PtExpr* pt_right = pt_head->right_range();
    ElbExpr* left = NULL;
    ElbExpr* right = NULL;
    int left_val = 0;
    int right_val = 0;
    if ( !instantiate_range(namedobj, pt_left, pt_right,
			    left, right, left_val, right_val) ) {
      continue;
    }

    ElbIOHead* head = NULL;
    if ( task ) {
      head = factory().new_IOHead(task, pt_head);
    }
    else {
      head = factory().new_IOHead(func, pt_head);
    }

    for (ymuint j = 0; j < pt_head->item_num(); ++ j) {
      const PtIOItem* pt_item = pt_head->item(j);

      // IO定義と変数/ネット定義が一致しているか調べる．
      ElbObjHandle* handle = find_obj(namedobj, pt_item->name());
      ElbDecl* decl = NULL;
      if ( handle ) {
	// 同名の要素が見つかった．
	if ( def_aux_type != kVpiAuxNone ) {
	  // なのに IO 宣言の aux_type もある．
	  ostringstream buf;
	  buf << pt_item->name() << " : has an aux-type declaration"
	      << ", while it also has another declaration in "
	      << handle->file_region() << ".";
	  put_msg(__FILE__, __LINE__,
		  pt_item->file_region(),
		  kMsgError,
		  "ELAB",
		  buf.str());
	  continue;
	}
	decl = handle->decl();
	if ( decl ) {
	  tVpiObjType type = decl->type();
	  if ( type != kVpiReg &&
	       type != kVpiIntegerVar &&
	       type != kVpiTimeVar) {
	    decl = NULL;
	  }
	}
	if ( !decl ) {
	  ostringstream buf;
	  buf << handle->full_name()
	      << ": Not a reg or integer/time variable.";
	  put_msg(__FILE__, __LINE__,
		  pt_item->file_region(),
		  kMsgError,
		  "ELAB",
		  buf.str());
	  continue;
	}

	if ( decl->dimension() > 0 ) {
	  ostringstream buf;
	  buf << pt_item->name()
	      << ": Array object shall not be connected to IO port.";
	  put_msg(__FILE__, __LINE__,
		  decl->file_region(),
		  kMsgError,
		  "ELAB",
		  buf.str());
	  continue;
	}

	// decl と型が一致しているか調べる．
	// IEEE 1364-2001 12.3.3 Port declarations
	ElbExpr* left2 = decl->_left_range();
	ElbExpr* right2 = decl->_right_range();
	if ( left2 && right2 ) {
	  if ( left == NULL && right == NULL ) {
	    // decl は範囲を持っているが IO は持っていない．
	    // これはエラーにしなくてよいのだろうか？
	    // たぶんコンパイルオプションで制御すべき
	    if ( allow_empty_io_range() ) {
	      left = left2;
	      right = right2;
	    }
	    else {
	      ostringstream buf;
	      buf << "Conflictive range declaration of \""
		  << pt_item->name() << "\".";
	      put_msg(__FILE__, __LINE__,
		      pt_item->file_region(),
		      kMsgError,
		      "ELAB",
		      buf.str());
	      continue;
	    }
	  }
	  else {
	    int left2_val;
	    bool stat1 = expr_to_int(left2, left2_val);
	    assert_cond(stat1, __FILE__, __LINE__);
	    int right2_val;
	    bool stat2 = expr_to_int(right2, right2_val);
	    assert_cond(stat2, __FILE__, __LINE__);
	    if ( left_val != left2_val || right_val != right2_val ) {
	      ostringstream buf;
	      buf << "Conflictive range declaration of \""
		  << pt_item->name() << "\".";
	      put_msg(__FILE__, __LINE__,
		      pt_item->file_region(),
		      kMsgError,
		      "ELAB",
		      buf.str());
	      continue;
	    }
	  }
	}
	else {
	  if ( left && right ) {
	    // decl は範囲を持っていないが IO は持っている．
	    // エラーとする．
	    ostringstream buf;
	    buf << "Conflictive range declaration of \""
		<< pt_item->name() << "\".";
	    put_msg(__FILE__, __LINE__,
		    pt_item->file_region(),
		    kMsgError,
		    "ELAB",
		    buf.str());
	    continue;
	  }
	}
	// どちらか一方でも符号付きなら両方符号付きにする．
	// ちょっと ad-hoc な仕様
	if ( !decl->is_signed() && sign ) {
	  decl->set_signed();
	}
      }
      else {
	// 同名の要素が見つからなかったので作る必要がある．
	tVpiAuxType aux_type = def_aux_type;
	if ( aux_type == kVpiAuxNone ) {
	  // 型指定が無い場合は reg 型となる．
	  aux_type = kVpiAuxReg;
	}
	
	// task/function の IO 宣言には初期値はない．
	const PtExpr* pt_init = pt_item->init_value();
	assert_cond( pt_init == NULL, __FILE__, __LINE__);
	
	ElbDeclHead* head = NULL;
	if ( task ) {
	  head = factory().new_DeclHead(func, pt_head, aux_type,
					left, right,
					left_val, right_val);
	}
	else {
	  head = factory().new_DeclHead(func, pt_head, aux_type,
					left, right,
					left_val, right_val);
	}

	decl = factory().new_Decl(head, pt_item);
	int tag = 0;
	switch ( aux_type ) {
	case kVpiAuxNet: tag = vpiNet; break;
	case kVpiAuxReg: tag = vpiReg; break;
	case kVpiAuxVar: tag = vpiVariables; break;
	default:
	  assert_not_reached(__FILE__, __LINE__);
	}
	reg_decl(tag, decl);
      }

      if ( task ) {
	task->init_iodecl(index, head, pt_item, decl);
      }
      else {
	func->init_iodecl(index, head, pt_item, decl);
      }
      ++ index;

      ostringstream buf;
      buf << "IODecl(" << pt_item->name() << ")@"
	  << namedobj->full_name() << " created.";
      put_msg(__FILE__, __LINE__,
	      pt_head->file_region(),
	      kMsgInfo,
	      "ELAB",
	      buf.str());
    }
  }
}

END_NAMESPACE_YM_VERILOG
