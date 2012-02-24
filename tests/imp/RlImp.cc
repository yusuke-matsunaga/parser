
/// @file RlImp.cc
/// @brief RlImp の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "RlImp.h"
#include "ImpMgr.h"
#include "StrNode.h"
#include "SnInput.h"
#include "SnAnd.h"
#include "SnXor.h"
#include "ImpInfo.h"
#include "ym_networks/BdnMgr.h"
#include "ym_networks/BdnNode.h"


BEGIN_NAMESPACE_YM_NETWORKS

//////////////////////////////////////////////////////////////////////
// クラス RlImp
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
RlImp::RlImp()
{
  mLevel = 0;
}

// @brief デストラクタ
RlImp::~RlImp()
{
}

// @brief ラーニングのレベルを設定する．
void
RlImp::set_learning_level(ymuint level)
{
  mLevel = level;
}

// @brief ネットワーク中の間接含意を求める．
// @param[in] network 対象のネットワーク
// @param[in] imp_info 間接含意のリスト
void
RlImp::learning(const BdnMgr& network,
		ImpInfo& imp_info)
{
  ymuint n = network.max_node_id();

  imp_info.set_size(n);

  // BDN の情報を ImpMgr にコピーする．
  ImpMgr imp_mgr;
  imp_mgr.set(network);

#if 1
  imp_mgr.print_network(cout);
#endif

  for (ymuint i = 0; i < n; ++ i) {
    StrNode* node = imp_mgr.node(i);
    if ( node == NULL ) continue;

    ymuint src_id = node->id();

    // node に値を割り当てる．
    for (ymuint val = 0; val < 2; ++ val) {
      vector<ImpCell> imp_list;
      bool ok = make_all_implication(imp_mgr, node, val, mLevel, imp_list);
      if ( ok ) {
	imp_info.put(src_id, val, imp_list);
      }
    }
  }
#if 1
  cout << "RECURSIVE LERNING IMPLICATION" << endl;
  imp_info.print(cout);
#endif
}

// @brief recursive learning を行なう．
// @param[in] imp_mgr ImpMgr
// @param[in] node ノード
// @param[in] val 値
// @param[in] level レベル
// @param[in] imp_list 含意のリスト
bool
RlImp::make_all_implication(ImpMgr& imp_mgr,
			    StrNode* node,
			    ymuint val,
			    ymuint level,
			    vector<ImpCell>& imp_list)
{
  imp_list.clear();
  bool ok = imp_mgr.assert(node, val, imp_list);
  if ( ok ) {
    if ( level > 0 ) {
      vector<StrNode*> unode_list;
      imp_mgr.get_unodelist(unode_list);
      // 共通の割り当てを調べるための配列
      vector<bool> vmark(imp_mgr.max_node_id(), false);
      for (vector<StrNode*>::iterator p = unode_list.begin();
	   p != unode_list.end(); ++ p) {
	StrNode* unode = *p;
	ymuint np = unode->justification_num();
	bool first = true;
	vector<ymuint> common_val(imp_mgr.max_node_id(), 2);
	vector<ymuint> common_list;
	for (ymuint i = 0; i < np; ++ i) {
	  ImpCell imp = unode->get_justification(i);
	  StrNode* inode = imp_mgr.node(imp.dst_id());
	  ymuint ival = imp.dst_val();
	  vector<ImpCell> imp_list1;
	  bool ok1 = make_all_implication(imp_mgr, inode, ival, level - 1,
					  imp_list1);
	  if ( ok1 ) {
	    if ( first ) {
	      first = false;
	      for (vector<ImpCell>::iterator p = imp_list1.begin();
		   p != imp_list1.end(); ++ p) {
		const ImpCell& imp = *p;
		ymuint dst_id = imp.dst_id();
		ymuint val = imp.dst_val();
		if ( vmark[dst_id] ) {
		  common_val[dst_id] = 2;
		}
		else {
		  common_val[dst_id] = val;
		  common_list.push_back(dst_id);
		}
	      }
	    }
	    else {
	      vector<bool> vmark1(imp_mgr.max_node_id(), false);
	      for (vector<ImpCell>::iterator p = imp_list1.begin();
		   p != imp_list1.end(); ++ p) {
		const ImpCell& imp = *p;
		ymuint dst_id = imp.dst_id();
		ymuint val = imp.dst_val();
		if ( common_val[dst_id] != val ) {
		  common_val[dst_id] = 2;
		}
		vmark1[dst_id] = true;
	      }
	      for (vector<ymuint>::iterator p = common_list.begin();
		   p != common_list.end(); ++ p) {
		ymuint dst_id = *p;
		if ( !vmark1[dst_id] ) {
		  common_val[dst_id] = 2;
		}
	      }
	    }
	  }
	}
	ymuint nc = common_list.size();
	for (ymuint i = 0; i < nc; ++ i) {
	  ymuint dst_id = common_list[i];
	  if ( common_val[dst_id] == 2 ) continue;
	  imp_list.push_back(ImpCell(dst_id, common_val[dst_id]));
	  vmark[dst_id] = true;
	}
      }
    }
  }
  else {
    // 単一の割り当てで矛盾が起こった．
    // node は !val 固定
  }
  imp_mgr.backtrack();

  return ok;
}

END_NAMESPACE_YM_NETWORKS
