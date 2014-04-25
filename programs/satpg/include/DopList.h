#ifndef DOPLIST_H
#define DOPLIST_H

/// @file DopList.h
/// @brief DopList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "DetectOp.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DopList DopList.h "DopList.h"
/// @brief DetectOp のリストを保持するクラス
//////////////////////////////////////////////////////////////////////
class DopList :
  public DetectOp
{
public:

  /// @brief コンストラクタ
  DopList();

  /// @brief デストラクタ
  virtual
  ~DopList();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dop を追加する．
  void
  add(DetectOp* dop);

  /// @brief 故障リストを設定する．
  /// @param[in] fault_list 故障リスト
  virtual
  void
  set_faults(const vector<TpgFault*>& fault_list);

  /// @brief テストパタンが見つかった時の処理
  /// @param[in] f 故障
  /// @param[in] tv テストパタン
  virtual
  void
  operator()(TpgFault* f,
	     TestVector* tv);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // dop のリスト
  vector<DetectOp*> mDopList;

};

END_NAMESPACE_YM_SATPG

#endif // DOPLIST_H