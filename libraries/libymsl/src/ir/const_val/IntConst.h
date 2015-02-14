#ifndef INTCONST_H
#define INTCONST_H

/// @file IntConst.h
/// @brief IntConst のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "ConstVal.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
/// @class IntConst IntConst.h "IntConst.h"
/// @brief 整数定数を表すノード
//////////////////////////////////////////////////////////////////////
class IntConst :
  public ConstVal
{
public:

  /// @brief コンストラクタ
  /// @param[in] type 型
  /// @param[in] val 値
  IntConst(const Type* type,
	   Ymsl_INT val);

  /// @brief デストラクタ
  virtual
  ~IntConst();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 整数値を返す．
  virtual
  Ymsl_INT
  int_val() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 値
  Ymsl_INT mVal;

};

END_NAMESPACE_YM_YMSL

#endif // INTCONST_H
