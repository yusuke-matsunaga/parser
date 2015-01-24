
/// @file AstMemberRef.cc
/// @brief AstMemberRef の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "AstMemberRef.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
// クラス AstMemberRef
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] body 本体の式
// @param[in] member メンバ名
// @param[in] loc ファイル位置
AstMemberRef::AstMemberRef(AstLeaf* body,
			   AstSymbol* member,
			   const FileRegion& loc) :
  AstLeaf(loc),
  mBody(body),
  mMember(member)
{
}

// @brief デストラクタ
AstMemberRef::~AstMemberRef()
{
}

// @brief 種類を返す．
LeafType
AstMemberRef::leaf_type() const
{
  return kMemberRef;
}

// @brief 本体の式を返す．
//
// kMemberRef, kArrayRef, kFuncCall のみ有効
const AstLeaf*
AstMemberRef::body() const
{
  return mBody;
}

// @brief メンバ名を返す．
//
// kMemberRef のみ有効
const AstSymbol*
AstMemberRef::symbol() const
{
  return mMember;
}

END_NAMESPACE_YM_YMSL
