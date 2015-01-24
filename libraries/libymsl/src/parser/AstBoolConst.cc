
/// @file AstBoolConst.cc
/// @brief AstBoolConst の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "AstBoolConst.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
// クラス AstBoolConst
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] leaf_type 式の種類 (kTrue/kFalse)
// @param[in] loc ファイル位置
AstBoolConst::AstBoolConst(LeafType leaf_type,
			   const FileRegion& loc) :
  AstLeaf(loc),
  mType(leaf_type)
{
}

// @brief デストラクタ
AstBoolConst::~AstBoolConst()
{
}

// @brief 種類を返す．
LeafType
AstBoolConst::leaf_type() const
{
  return mType;
}

END_NAMESPACE_YM_YMSL
