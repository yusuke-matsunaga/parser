#ifndef MINPATDSATUR2_H
#define MINPATDSATUR2_H

/// @file MinPatDsatur2.h
/// @brief MinPatDsatur2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "MinPatBase.h"
#include "FaultAnalyzer.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class MinPatDsatur2 MinPatDsatur2.h "MinPatDsatur2.h"
/// @brief 'Dsatur' っぽい MinPat
//////////////////////////////////////////////////////////////////////
class MinPatDsatur2 :
  public MinPatBase
{
public:

  /// @brief コンストラクタ
  MinPatDsatur2();

  /// @brief デストラクタ
  ~MinPatDsatur2();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化を行う．
  /// @param[in] network 対象のネットワーク
  /// @param[in] tvmgr テストベクタマネージャ
  /// @param[in] fmgr 故障マネージャ
  /// @param[in] fsim2 2値の故障シミュレータ(検証用)
  /// @param[in] tv_list テストベクタのリスト
  /// @return 支配故障数を返す．
  virtual
  ymuint
  init(TpgNetwork& network,
       TvMgr& tvmgr,
       FaultMgr& fmgr,
       Fsim& fsim2,
       vector<TestVector*>& tv_list);

  /// @brief 最初の故障を選ぶ．
  virtual
  TpgFault*
  get_first_fault();

  /// @brief 次に処理すべき故障を選ぶ．
  /// @param[in] fgmgr 故障グループを管理するオブジェクト
  ///
  /// 故障が残っていなければ NULL を返す．
  virtual
  TpgFault*
  get_next_fault(FgMgr& fgmgr);

  /// @brief 故障を追加するグループを選ぶ．
  /// @param[in] fgmgr 故障グループを管理するオブジェクト
  /// @param[in] fault 故障
  ///
  /// グループが見つからなければ fgmgr.group_num() を返す．
  virtual
  ymuint
  find_group(FgMgr& fgmgr,
	     TpgFault* fault);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  // 故障ごとの情報
  struct FaultStruct
  {
    // 故障
    TpgFault* mFault;

    // 選択済みマーク
    bool mSelected;

    // 衝突している故障番号のリスト
    vector<ymuint> mConflictList;

    // 衝突数
    ymuint mConflictNum;

    // 衝突したグループ番号のマップ
    vector<bool> mConflictMap;

  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード番号の最大値
  ymuint mMaxNodeId;

  // 故障番号の最大値
  ymuint mMaxFaultId;

  // 故障解析器
  FaultAnalyzer mAnalyzer;

  // 故障用の作業領域のリスト
  vector<FaultStruct> mFaultStructList;

  // 故障番号から FaultStruct の位置を引くためのマップ
  vector<ymuint> mFaultMap;

  // 未処理の故障数
  ymuint mRemainNum;

  // 前回選ばれた故障
  ymuint mPrevFpos;

  // 前回選ばれたグループ番号
  ymuint mPrevGid;
};

END_NAMESPACE_YM_SATPG

#endif // MINPATDSATUR2_H
