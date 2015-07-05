﻿#ifndef COFOP_H
#define COFOP_H

/// @file CofOp.h
/// @brief CofOp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "BddUniOp.h"


BEGIN_NAMESPACE_YM_BDD

class BddBinOp;

//////////////////////////////////////////////////////////////////////
/// @class CofOp CofOp.h "CofOp.h"
/// @brief cofactor 演算を行うクラス
//////////////////////////////////////////////////////////////////////
class CofOp :
  public BddUniOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] mgr マネージャ
  CofOp(BddMgrImpl* mgr);

  /// @brief デストラクタ
  virtual
  ~CofOp();


public:
  //////////////////////////////////////////////////////////////////////
  // メインの関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 演算を行う関数
  /// @param[in] left オペランド
  /// @param[in] id コファクタをとる変数番号
  /// @param[in] inv 極性
  /// @return 演算結果を返す．
  virtual
  BddEdge
  apply(BddEdge left,
	VarId id,
	bool inv);


private:
  //////////////////////////////////////////////////////////////////////
  // 下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 実際の演算を行う関数
  BddEdge
  apply_step(BddEdge f);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // コファクターをとる変数のレベル
  ymuint32 mLevel;

  // コファクターの極性
  bool mInv;

};

END_NAMESPACE_YM_BDD

#endif // COFOP_H
