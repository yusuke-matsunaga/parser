#ifndef FGMGR2_H
#define FGMGR2_H

/// @file FgMgr2.h
/// @brief FgMgr2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "FgMgr.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_YM_SATPG

class FaultAnalyzer;

//////////////////////////////////////////////////////////////////////
/// @class FgMgr2 FgMgr2.h "FgMgr2.h"
/// @brief fault group manager
///
/// 故障グループを管理するクラス
//////////////////////////////////////////////////////////////////////
class FgMgr2 :
  public FgMgr
{
public:

  /// @brief コンストラクタ
  /// @param[in] max_node_id ノード番号の最大値 + 1
  /// @param[in] analyzer 故障解析器
  FgMgr2(ymuint max_node_id,
	 const FaultAnalyzer& analyzer);

  /// @brief デストラクタ
  virtual
  ~FgMgr2();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief クリアする．
  virtual
  void
  clear();

  /// @brief 現在のグループ数を返す．
  virtual
  ymuint
  group_num() const;

  /// @brief 新しいグループを作る．
  /// @return グループ番号を返す．
  virtual
  ymuint
  new_group();

  /// @brief グループを複製する．
  /// @param[in] src_gid 複製元のグループ番号
  /// @return 新しいグループ番号を返す．
  virtual
  ymuint
  duplicate_group(ymuint src_gid);

  /// @brief グループを置き換える．
  /// @param[in] old_gid 置き換え対象のグループ番号
  /// @param[in] new_gid 置き換えるグループ番号
  ///
  /// new_gid は削除される．
  virtual
  void
  replace_group(ymuint old_gid,
		ymuint new_gid);

  /// @brief グループを削除する．
  /// @param[in] gid グループ番号
  virtual
  void
  delete_group(ymuint gid);

  /// @brief 新たな条件なしで追加できる既存グループを見つける．
  /// @param[in] fault 対象の故障
  /// @param[in] group_list 探索最小のグループ番号のリスト
  /// @param[in] first_hit 最初のグループのみを求めるとき true にするフラグ
  /// @param[out] gid_list 対象のグループ番号を収めるリスト
  /// @return 最初のグループ番号を返す．
  ///
  /// 見つからない場合は group_num() を返す．
  /// gid_list は first_hit == true の時，意味を持たない．
  virtual
  ymuint
  find_dom_group(const TpgFault* fault,
		 const vector<ymuint>& group_list,
		 bool first_hit,
		 vector<ymuint>& gid_list);

  /// @brief 追加できる既存グループを見つける．
  /// @param[in] fault 対象の故障
  /// @param[in] group_list 探索最小のグループ番号のリスト
  /// @param[in] first_hit 最初のグループのみを求めるとき true にするフラグ
  /// @param[out] gid_list 対象のグループ番号を収めるリスト
  /// @return 最初のグループ番号を返す．
  ///
  /// 見つからない場合は group_num() を返す．
  /// gid_list は first_hit == true の時，意味を持たない．
  virtual
  ymuint
  find_group(const TpgFault* fault,
	     const vector<ymuint>& group_list,
	     bool first_hit,
	     vector<ymuint>& gid_list);

  /// @brief 既存のグループに故障を追加する．
  /// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
  /// @param[in] fault 故障
  virtual
  void
  add_fault(ymuint gid,
	    const TpgFault* fault);

  /// @brief 故障を取り除く
  /// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
  /// @param[in] fault_list 故障リスト
  virtual
  void
  delete_fault(ymuint gid,
	       const vector<const TpgFault*>& fault_list);

  /// @brief 故障リストを返す．
  /// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
  virtual
  const vector<const TpgFault*>&
  fault_list(ymuint gid) const;

  /// @brief 十分割当リストを返す．
  /// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
  virtual
  const NodeValList&
  sufficient_assignment(ymuint gid) const;

  /// @brief 外部入力上の十分割当リストを返す．
  /// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
  virtual
  const NodeValList&
  pi_sufficient_assignment(ymuint gid) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  // 故障グループを表す構造体
  struct FaultGroup
  {
    /// @brief 故障を追加する．
    void
    add_fault(const TpgFault* fault,
	      const NodeValList& suf_list,
	      const NodeValList& ma_list)
    {
      mFaultList.push_back(fault);
      mFaultSufList.push_back(suf_list);
      mSufList.merge(suf_list);
      mMaList.merge(ma_list);
    }

    // グループ番号
    ymuint mId;

    // 故障リスト
    vector<const TpgFault*> mFaultList;

    // 故障ごとの十分割当リスト
    vector<NodeValList> mFaultSufList;

    // 十分割当リスト
    NodeValList mSufList;

    // 必要割当リスト
    NodeValList mMaList;

  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード番号の最大値
  ymuint mMaxNodeId;

  // 故障解析器
  const FaultAnalyzer& mAnalyzer;

  // 故障グループの配列
  vector<FaultGroup*> mGroupList;

};

END_NAMESPACE_YM_SATPG

#endif // FGMGR2_H
