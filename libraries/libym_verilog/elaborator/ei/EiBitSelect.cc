
/// @file libym_verilog/elb_impl/EiBitSelect.cc
/// @brief EiBitSelect の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: EiBitSelect.cc 2507 2009-10-17 16:24:02Z matsunaga $
///
/// Copyright (C) 2005-2010 Yusuke Matsunaga
/// All rights reserved.


#include "EiFactory.h"
#include "EiBitSelect.h"
#include "ElbDecl.h"

#include "ym_verilog/BitVector.h"
#include "ym_verilog/pt/PtExpr.h"



BEGIN_NAMESPACE_YM_VERILOG

//////////////////////////////////////////////////////////////////////
// EiFactory の生成関数
//////////////////////////////////////////////////////////////////////

// @brief 固定ビット選択式を生成する．
// @param[in] pt_expr パース木の定義要素
// @param[in] base_expr 本体のオブジェクト
// @param[in] index_expr ビット選択式
// @param[in] index_val ビット選択式の値
ElbExpr*
EiFactory::new_BitSelect(const PtExpr* pt_expr,
			 ElbExpr* base_expr,
			 const PtExpr* index_expr,
			 int index_val)
{
  void* p = mAlloc.get_memory(sizeof(EiConstBitSelect));
  ElbExpr* expr = new (p) EiConstBitSelect(pt_expr, base_expr,
					   index_expr, index_val);

  return expr;
}

// @brief 固定ビット選択式を生成する．
// @param[in] pt_expr パース木の定義要素
// @param[in] base_expr 本体の式
// @param[in] bit_index_val ビット選択式の値
ElbExpr*
EiFactory::new_BitSelect(const PtExpr* pt_expr,
			 ElbExpr* base_expr,
			 int index_val)
{
  void* p = mAlloc.get_memory(sizeof(EiConstBitSelect));
  ElbExpr* expr = new (p) EiConstBitSelect(pt_expr, base_expr, NULL, index_val);

  return expr;
}

// @brief 可変ビット選択式を生成する．
// @param[in] pt_expr パース木の定義要素
// @param[in] base_expr 本体のオブジェクト
// @param[in] bit_index ビット選択式
ElbExpr*
EiFactory::new_BitSelect(const PtExpr* pt_expr,
			 ElbExpr* base_expr,
			 ElbExpr* index_expr)
{
  void* p = mAlloc.get_memory(sizeof(EiBitSelect));
  ElbExpr* expr = new (p) EiVarBitSelect(pt_expr, base_expr, index_expr);

  return expr;
}


//////////////////////////////////////////////////////////////////////
// クラス EiBitSelect
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] pt_expr パース木の定義要素
// @param[in] base_expr 対象の式
EiBitSelect::EiBitSelect(const PtExpr* pt_expr,
			 ElbExpr* base_expr) :
  EiExprBase(pt_expr),
  mBaseExpr(base_expr)
{
}

// @brief デストラクタ
EiBitSelect::~EiBitSelect()
{
}

// @brief 型の取得
tVpiObjType
EiBitSelect::type() const
{
  switch ( parent_expr()->type() ) {
  case kVpiNet: return kVpiNetBit;
  case kVpiReg: return kVpiRegBit;
  default: break;
  }
  return kVpiBitSelect;
}

// @brief 式のタイプを返す．
tVpiValueType
EiBitSelect::value_type() const
{
  return pack(kVpiValueUS, 1);
}

// @brief 定数の時 true を返す．
// @note 参照している要素の型によって決まる．
bool
EiBitSelect::is_const() const
{
  return parent_expr()->is_const() && is_constant_select();
}

// @brief ビット指定の時に true を返す．
bool
EiBitSelect::is_bitselect() const
{
  return true;
}

// @brief 宣言要素への参照の場合，対象のオブジェクトを返す．
// @note 宣言要素に対するビット選択，部分選択の場合にも意味を持つ．
const VlDecl*
EiBitSelect::decl_obj() const
{
  return parent_expr()->decl_obj();
}

// @brief 宣言要素への参照の場合，対象のオブジェクトを返す．
// @note 宣言要素に対するビット選択，部分選択の場合にも意味を持つ．
const VlDeclArray*
EiBitSelect::declarray_obj() const
{
  return parent_expr()->declarray_obj();
}

// @brief 配列型宣言要素への参照の場合，配列の次元を返す．
// @note それ以外では 0 を返す．
ymuint
EiBitSelect::declarray_dimension() const
{
  return parent_expr()->declarray_dimension();
}

// @brief 配列型宣言要素への参照の場合，配列のインデックスを返す．
// @param[in] pos 位置番号 ( 0 <= pos < declarray_dimension() )
// @note それ以外では NULL を返す．
const VlExpr*
EiBitSelect::declarray_index(ymuint pos) const
{
  return parent_expr()->declarray_index(pos);
}

// @brief 親の式を返す．
const VlExpr*
EiBitSelect::parent_expr() const
{
  return mBaseExpr;
}

// @brief 要求される式の型を計算してセットする．
// @param[in] type 要求される式の型
// @note 必要であればオペランドに対して再帰的に処理を行なう．
void
EiBitSelect::set_reqsize(tVpiValueType type)
{
}


//////////////////////////////////////////////////////////////////////
// クラス EiConstBitSelect
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] pt_expr パース木の定義要素
// @param[in] base_expr 対象の式
// @param[in] index_expr ビット選択式
// @param[in] index_val ビット選択式の値
EiConstBitSelect::EiConstBitSelect(const PtExpr* pt_expr,
				   ElbExpr* base_expr,
				   const PtExpr* index_expr,
				   int index_val) :
  EiBitSelect(pt_expr, base_expr),
  mIndexExpr(index_expr),
  mIndexVal(index_val)
{
}

// @brief デストラクタ
EiConstBitSelect::~EiConstBitSelect()
{
}

// @brief 固定選択子の時 true を返す．
// @note ビット選択，部分選択の時，意味を持つ．
bool
EiConstBitSelect::is_constant_select() const
{
  return true;
}

// @brief インデックス式を返す．
const VlExpr*
EiConstBitSelect::index() const
{
  return NULL;
}

// @brief インデックス値を返す．
// @note 式に対するビット選択の時，意味を持つ．
int
EiConstBitSelect::index_val() const
{
  return mIndexVal;
}


//////////////////////////////////////////////////////////////////////
// クラス EiVarBitSelect
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] pt_expr パース木の定義要素
// @param[in] base_expr 対象の式
// @param[in] index_expr ビット選択式
EiVarBitSelect::EiVarBitSelect(const PtExpr* pt_expr,
			       ElbExpr* base_expr,
			       ElbExpr* index_expr) :
  EiBitSelect(pt_expr, base_expr),
  mIndexExpr(index_expr)
{
}

// @brief デストラクタ
EiVarBitSelect::~EiVarBitSelect()
{
}

// @brief 固定選択子の時 true を返す．
// @note ビット選択，部分選択の時，意味を持つ．
bool
EiVarBitSelect::is_constant_select() const
{
  return mIndexExpr->is_const();
}

// @brief インデックス式を返す．
const VlExpr*
EiVarBitSelect::index() const
{
  return mIndexExpr;
}

// @brief インデックス値を返す．
// @note 式に対するビット選択の時，意味を持つ．
int
EiVarBitSelect::index_val() const
{
  return 0;
}

END_NAMESPACE_YM_VERILOG
