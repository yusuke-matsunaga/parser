#ifndef VARIABLE_H
#define VARIABLE_H

/// @file Variable.h
/// @brief Variable のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#include "igf_nsdef.h"
#include "RegVect.h"


BEGIN_NAMESPACE_YM_IGF

//////////////////////////////////////////////////////////////////////
/// @class Variable Variable.h "Variable.h"
/// @brief 通常の変数と合成変数を表すクラス
//////////////////////////////////////////////////////////////////////
class Variable
{
public:

  /// @brief 通常の変数用のコンストラクタ
  /// @param[in] vid 変数番号
  Variable(ymuint vid);

  /// @brief 合成変数用のコンストラクタ
  /// @param[in] vid_list 変数番号のリスト
  Variable(const vector<ymuint>& vid_list);

  /// @brief デストラクタ
  ~Variable();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ベクタを分類する．
  /// @param[in] vect 対象のベクタ
  ymuint
  classify(const RegVect* vect) const;

  /// @brief 内容を出力する．
  void
  dump(ostream& s) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数番号のリスト
  vector<ymuint> mVidList;

};

END_NAMESPACE_YM_IGF

#endif // VARIABLE_H
