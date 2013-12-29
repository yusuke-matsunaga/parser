#ifndef BACKTRACER_H
#define BACKTRACER_H

/// @file BackTracer.h
/// @brief BackTracer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2013 Yusuke Matsunaga
/// All rights reserved.


#include "satpg_nsdef.h"
#include "ym_logic/Bool3.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class BackTracer BackTracer.h "BackTracer.h"
/// @brief テストパタンを求めるためのバックトレースを行なうファンクター
//////////////////////////////////////////////////////////////////////
class BackTracer
{
public:

  /// @brief デストラクタ
  virtual
  ~BackTracer() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが実装する仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief バックトレースを行なう．
  /// @param[in] fnode 故障のあるノード
  /// @param[in] model SATの値の割り当て結果を収めた配列
  /// @param[in] input_list テストパタンに関係のある入力のリスト
  /// @param[in] output_list 故障伝搬の可能性のある出力のリスト
  /// @return テストベクタを返す．
  virtual
  TestVector*
  operator()(TpgNode* fnode,
	     const vector<Bool3>& model,
	     const vector<TpgNode*>& input_list,
	     const vector<TpgNode*>& output_list) = 0;

};

/// @brief 'Simple' タイプの生成を行なう．
/// @param[in] tvmgr TvMgr
/// @param[in] max_id ノードの最大 ID + 1 ( = TpgNetwork::node_num() )
BackTracer*
new_BtSimple(TvMgr& tvmgr,
	     ymuint max_id);

/// @brief 'Just1' タイプの生成を行なう．
/// @param[in] tvmgr TvMgr
/// @param[in] max_id ノードの最大 ID + 1 ( = TpgNetwork::node_num() )
BackTracer*
new_BtJust1(TvMgr& tvmgr,
	    ymuint max_id);

/// @brief 'Just2' タイプの生成を行なう．
/// @param[in] tvmgr TvMgr
/// @param[in] max_id ノードの最大 ID + 1 ( = TpgNetwork::node_num() )
BackTracer*
new_BtJust2(TvMgr& tvmgr,
	    ymuint max_id);

/// @brief 'Zdd' タイプの生成を行なう．
/// @param[in] tvmgr TvMgr
/// @param[in] max_id ノードの最大 ID + 1 ( = TpgNetwork::node_num() )
BackTracer*
new_BtZdd(TvMgr& tvmgr,
	  ymuint max_id);

END_NAMESPACE_YM_SATPG

#endif // BACKTRACER_H