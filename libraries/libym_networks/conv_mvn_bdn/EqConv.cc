
/// @file libym_networks/EqConv.cc
/// @brief EqConv の実装クラス
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "EqConv.h"
#include "ym_networks/MvnNode.h"
#include "ym_networks/MvnBdnMap.h"
#include "ym_networks/BdnMgr.h"
#include "ym_networks/BdnNodeHandle.h"


BEGIN_NAMESPACE_YM_MVNBDNCONV

// @brief コンストラクタ
EqConv::EqConv()
{
}

// @brief デストラクタ
EqConv::~EqConv()
{
}

// @brief MvnNode を BdnMgr に変換する．
// @param[in] node ノード
// @param[in] bdnetwork 変換結果の BdnMgr
// @param[in] nodemap ノードの対応関係を表すマップ
// @retval true このクラスで変換処理を行った．
// @retval false このクラスでは変換処理を行わなかった．
bool
EqConv::operator()(const MvnNode* node,
		   BdnMgr& bdnetwork,
		   MvnBdnMap& nodemap)
{
  if ( node->type() == MvnNode::kEq ) {
    const MvnInputPin* ipin0 = node->input(0);
    const MvnOutputPin* src_pin0 = ipin0->src_pin();
    const MvnNode* src_node0 = src_pin0->node();

    const MvnInputPin* ipin1 = node->input(1);
    const MvnOutputPin* src_pin1 = ipin1->src_pin();
    const MvnNode* src_node1 = src_pin1->node();

    ymuint bw = src_pin0->bit_width();
    assert_cond( src_pin1->bit_width() == bw, __FILE__, __LINE__);
    vector<BdnNodeHandle> input_array(bw);
    for (ymuint i = 0; i < bw; ++ i) {
      BdnNodeHandle handle0 = nodemap.get(src_node0, i);
      BdnNodeHandle handle1 = nodemap.get(src_node1, i);
      input_array[i] = bdnetwork.new_xnor(handle0, handle1);
    }

    BdnNodeHandle handle = bdnetwork.new_and(input_array);
    nodemap.put(node, 0, handle);

    return true;
  }
  return false;
}

END_NAMESPACE_YM_MVNBDNCONV