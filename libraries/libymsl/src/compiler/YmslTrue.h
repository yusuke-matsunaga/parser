#ifndef YMSLTRUE_H
#define YMSLTRUE_H

/// @file YmslTrue.h
/// @brief YmslTrue のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "YmslExpr.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
/// @class YmslTrue YmslTrue.h "YmslTrue.h"
/// @brief true を表すクラス
//////////////////////////////////////////////////////////////////////
class YmslTrue :
  public YmslExpr
{
public:

  /// @brief コンストラクタ
  YmslTrue();

  /// @brief デストラクタ
  virtual
  ~YmslTrue();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を返す．
  virtual
  ExprType
  expr_type() const;

};

END_NAMESPACE_YM_YMSL

#endif // YMSLTRUE_H