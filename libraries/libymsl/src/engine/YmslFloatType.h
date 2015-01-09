#ifndef YMSLFLOATTYPE_H
#define YMSLFLOATTYPE_H

/// @file YmslFloatType.h
/// @brief YmslFloatType のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmslType.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
/// @class YmslFloatType YmslFloatType.h "YmslFloatType.h"
/// @brief float 型を表す YmslType
//////////////////////////////////////////////////////////////////////
class YmslFloatType :
  public YmslType
{
public:

  /// @brief コンストラクタ
  YmslFloatType();

  /// @brief デストラクタ
  virtual
  ~YmslFloatType();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を得る．
  virtual
  TypeId
  type() const;

  /// @brief 内容を出力する．
  /// @param[in] s 出力先のストリーム
  virtual
  void
  print(ostream& s) const;

};

END_NAMESPACE_YM_YMSL

#endif // YMSLFLOATTYPE_H
