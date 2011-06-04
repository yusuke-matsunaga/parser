
/// @file libym_networks/cmn/CmnMgrImpl.cc
/// @brief CmnMgrImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: LutNetwork.cc 2274 2009-06-10 07:45:29Z matsunaga $
///
/// Copyright (C) 2005-2010 Yusuke Matsunaga
/// All rights reserved.


#include "CmnMgrImpl.h"
#include "ym_networks/CmnNode.h"
#include "ym_networks/CmnDff.h"
#include "ym_networks/CmnLatch.h"


BEGIN_NAMESPACE_YM_CMN

///////////////////////////////////////////////////////////////////////
// クラス CmnMgrImpl
///////////////////////////////////////////////////////////////////////

#if 0
// コンストラクタ
CmnMgr::CmnMgr() :
  mAlloc(4096),
  mAlloc2(4096)
{
}

// コピーコンストラクタ
CmnMgr::CmnMgr(const CmnMgr& src) :
  mAlloc(4096),
  mAlloc2(4096)
{
  vector<CmnNode*> nodemap;
  copy(src, nodemap);
}

// 代入演算子
const CmnMgr&
CmnMgr::operator=(const CmnMgr& src)
{
  if ( &src != this ) {
    clear();
    vector<CmnNode*> nodemap;
    copy(src, nodemap);
  }
  return *this;
}

// デストラクタ
CmnMgr::~CmnMgr()
{
  clear();
}

// 複製する．
void
CmnMgr::copy(const CmnMgr& src,
	      vector<CmnNode*>& nodemap)
{
  mName = src.mName;

  ymuint n = src.max_node_id();
  nodemap.clear();
  nodemap.resize(n);

  // 外部入力の生成
  const CmnNodeList& input_list = src.input_list();
  for (CmnNodeList::const_iterator p = input_list.begin();
       p != input_list.end(); ++ p) {
    CmnNode* src_node = *p;
    CmnNode* dst_node = new_input();
    nodemap[src_node->id()] = dst_node;
  }

  // DFFノードの生成
  const CmnNodeList& dff_list = src.dff_list();
  for (CmnNodeList::const_iterator p = dff_list.begin();
       p != dff_list.end(); ++ p) {
    CmnNode* src_node = *p;
    CmnNode* dst_node = new_dff();
    nodemap[src_node->id()] = dst_node;
  }

  // セルノードの生成
  vector<CmnNode*> node_list;
  src.sort(node_list);
  ymuint nl = node_list.size();
  for (ymuint i = 0; i < nl; ++ i) {
    CmnNode* src_node = node_list[i];
    ymuint ni = src_node->ni();
    vector<CmnNode*> dst_inputs(ni);
    for (ymuint j = 0; j < ni; ++ j) {
      CmnNode* src_inode = src_node->fanin(j);
      CmnNode* input = nodemap[src_inode->id()];
      assert_cond(input, __FILE__, __LINE__);
      dst_inputs[j] = input;
    }
    const Cell* cell = src_node->cell();
    CmnNode* dst_node = new_cellnode(dst_inputs, cell);
    if ( src_node->pomark() ) {
      dst_node->mFlags |= CmnNode::kPoMask;
    }
    nodemap[src_node->id()] = dst_node;
  }

  // DFFノードの入力の接続
  for (CmnNodeList::const_iterator p = dff_list.begin();
       p != dff_list.end(); ++ p) {
    CmnNode* src_node = *p;
    CmnNode* dst_node = nodemap[src_node->id()];
    CmnNode* src_inode = src_node->fanin(0);
    CmnNode* dst_inode = nodemap[src_inode->id()];
    set_dff_input(dst_node, dst_inode);
  }

  // 外部出力の生成
  const CmnNodeList& output_list = src.output_list();
  for (CmnNodeList::const_iterator p = output_list.begin();
       p != output_list.end(); ++ p) {
    CmnNode* src_onode = *p;
    CmnNode* src_inode = src_onode->fanin(0);
    CmnNode* dst_inode = NULL;
    if ( src_inode ) {
      dst_inode = nodemap[src_inode->id()];
    }
    (void) new_output(dst_inode);
  }

  // ポートの複製
  ymuint np = src.port_num();
  for (ymuint i = 0; i < np; ++ i) {
    const CmnPort* src_port = src.port(i);
    ymuint nb = src_port->bit_width();
    vector<CmnNode*> tmp(nb);
    for (ymuint j = 0; j < nb; ++ j) {
      const CmnNode* src_node = src_port->bit(j);
      CmnNode* dst_node = nodemap[src_node->id()];
      tmp[j] = dst_node;
    }
    add_port(src_port->name(), tmp);
  }
}

// @brief 入出力ノードに関連づけられたポートを得る．
// @param[in] node 入出力ノード
const CmnPort*
CmnMgr::port(const CmnNode* node) const
{
  if ( node->is_input() ) {
    return mInputPortArray[node->subid()].mPort;
  }
  else if ( node->is_output() ) {
    return mOutputPortArray[node->subid()].mPort;
  }
  else {
    return NULL;
  }
}

// @brief 入出力ノードのポートにおけるビット位置を得る．
// @param[in] node 入出力ノード
ymuint
CmnMgr::port_pos(const CmnNode* node) const
{
  if ( node->is_input() ) {
    return mInputPortArray[node->subid()].mPos;
  }
  else if ( node->is_output() ) {
    return mOutputPortArray[node->subid()].mPos;
  }
  else {
    return NULL;
  }
}

// @brief 入力ノードと DFF ノードのリストを得る．
// @param[out] node_list ノードを格納するリスト
// @return 要素数を返す．
ymuint
CmnMgr::ppi_list(list<CmnNode*>& node_list) const
{
  node_list.clear();
  for (CmnNodeList::const_iterator p = input_list().begin();
       p != input_list().end(); ++ p) {
    node_list.push_back(*p);
  }
  for (CmnNodeList::const_iterator p = dff_list().begin();
       p != dff_list().end(); ++ p) {
    node_list.push_back(*p);
  }
  return node_list.size();
}

// @brief 出力ノードと DFF ノードのリストを得る．
// @param[out] node_list ノードを格納するリスト
// @return 要素数を返す．
ymuint
CmnMgr::ppo_list(list<CmnNode*>& node_list) const
{
  node_list.clear();
  for (CmnNodeList::const_iterator p = output_list().begin();
       p != output_list().end(); ++ p) {
    node_list.push_back(*p);
  }
  for (CmnNodeList::const_iterator p = dff_list().begin();
       p != dff_list().end(); ++ p) {
    node_list.push_back(*p);
  }
  return node_list.size();
}


BEGIN_NONAMESPACE

// node のファンアウトのうちファンインがすべてマークされているノード
// をリストに加える．
void
sort_sub(CmnNode* node,
	 vector<bool>& mark,
	 vector<CmnNode*>& node_list)
{
  const CnEdgeList& fo_list = node->fanout_list();
  for (CnEdgeList::const_iterator p = fo_list.begin();
       p != fo_list.end(); ++ p) {
    CnEdge* e = *p;
    CmnNode* onode = e->to();
    if ( mark[onode->id()] || !onode->is_cellnode() ) continue;
    bool ready = true;
    ymuint ni = onode->ni();
    for (ymuint i = 0; i < ni; ++ i) {
      CmnNode* inode0 = onode->fanin(i);
      if ( !mark[inode0->id()] ) {
	ready = false;
	break;
      }
    }
    if ( ready ) {
      mark[onode->id()] = true;
      node_list.push_back(onode);
    }
  }
}

END_NONAMESPACE

// @brief ソートされたノードのリストを得る．
void
CmnMgr::sort(vector<CmnNode*>& node_list) const
{
  node_list.clear();
  node_list.reserve(cellnode_num());

  ymuint n = max_node_id();
  vector<bool> mark(n, false);

  // 外部入力とDFFのみをファンインにするノードを node_list に追加する．
  list<CmnNode*> tmp_list;
  ppi_list(tmp_list);
  for (list<CmnNode*>::const_iterator p = tmp_list.begin();
       p != tmp_list.end(); ++ p) {
    CmnNode* node = *p;
    mark[node->id()] = true;
    sort_sub(node, mark, node_list);
  }

  // 入力を持たないノード(定数ノード)を node_list に追加する．
  for (CmnNodeList::const_iterator p = mCellnodeList.begin();
       p != mCellnodeList.end(); ++ p) {
    CmnNode* node = *p;
    if ( node->ni() == 0 && !mark[node->id()] ) {
      mark[node->id()] = true;
      node_list.push_back(node);
    }
  }

  ymuint rpos = 0;
  while ( rpos < node_list.size() ) {
    CmnNode* node = node_list[rpos];
    ++ rpos;
    sort_sub(node, mark, node_list);
  }
  assert_cond(node_list.size() == cellnode_num(), __FILE__, __LINE__);
}

// @brief モジュール名を設定する．
void
CmnMgr::set_name(const string& name)
{
  mName = name;
}

// @brief ポートを追加する(ベクタ版)．
// @param[in] name ポート名
// @param[in] io_node_vec 対応する入出力ノードのベクタ
void
CmnMgr::add_port(const string& name,
		  const vector<CmnNode*>& io_node_vec)
{
  ymuint bw = io_node_vec.size();
  void* q = mAlloc.get_memory(sizeof(const CmnNode*) * bw);
  const CmnNode** bit_array = new (q) const CmnNode*[bw];
  for (ymuint i = 0; i < bw; ++ i) {
    bit_array[i] = io_node_vec[i];
  }
  void* p = mAlloc.get_memory(sizeof(CmnPort));
  CmnPort* port = new (p) CmnPort(name, bw, bit_array);
  mPortArray.push_back(port);
  ymuint n = io_node_vec.size();
  for (ymuint i = 0; i < n; ++ i) {
    CmnNode* node = io_node_vec[i];
    if ( node->is_input() ) {
      PortInfo& port_info = mInputPortArray[node->subid()];
      port_info.mPort = port;
      port_info.mPos = i;
    }
    else if ( node->is_output() ) {
      PortInfo& port_info = mOutputPortArray[node->subid()];
      port_info.mPort = port;
      port_info.mPos = i;
    }
    else {
      assert_not_reached(__FILE__, __LINE__);
    }
  }
}

// @brief 入力ノードを作る．
// @return 作成したノードを返す．
CmnNode*
CmnMgr::new_input()
{
  CmnNode* node = new_node(0);

  // 入力ノード配列に登録
  ymuint subid = mInputArray.size();
  mInputArray.push_back(node);
  // ダミーの place-holder を追加する．
  mInputPortArray.push_back(PortInfo());

  // 入力リストに登録
  mInputList.push_back(node);

  node->set_input(subid);

  return node;
}

// @brief 出力ノードを作る．
// @param[in] inode 入力のノード
// @return 作成したノードを返す．
CmnNode*
CmnMgr::new_output(CmnNode* inode)
{
  CmnNode* node = new_node(1);

  // 出力ノード配列に登録
  ymuint subid = mOutputArray.size();
  mOutputArray.push_back(node);
  // ダミーの place-holder を追加する．
  mOutputPortArray.push_back(PortInfo());

  // 出力リストに登録
  mOutputList.push_back(node);

  node->set_output(subid);
  connect(inode, node, 0);

  return node;
}

// @brief セルノードを作る．
// @param[in] inodes 入力ノードのベクタ
// @param[in] cell セル
CmnNode*
CmnMgr::new_cellnode(const vector<CmnNode*>& inodes,
		      const Cell* cell)
{
  ymuint ni = inodes.size();
  CmnNode* node = new_node(ni);

  // セルノードリストに登録
  mCellnodeList.push_back(node);

  // セルノードタイプに設定
  node->set_cell(cell);

  // ファンインの設定
  for (ymuint i = 0; i < ni; ++ i) {
    connect(inodes[i], node, i);
  }

  return node;
}

// @brief DFFノードを作る．
// @return 作成したノードを返す．
CmnNode*
CmnMgr::new_dff()
{
  CmnNode* node = new_node(4);

  // DFFノードリストに登録
  mDffList.push_back(node);

  // DFFノードタイプに設定
  node->set_dff();

  return node;
}

// @brief DFFノードの入力を設定する．
// @param[in] node 対象の DFF ノード
// @param[in] inode 入力のノード
void
CmnMgr::set_dff_input(CmnNode* node,
		       CmnNode* inode)
{
  assert_cond(node->is_dff(), __FILE__, __LINE__);

  connect(inode, node, 0);
}

// @brief DFFノードのクロック入力を設定する．
// @param[in] node 対象の DFF ノード
// @param[in] inode 入力のノード
void
CmnMgr::set_dff_clock(CmnNode* node,
		       CmnNode* inode)
{
  assert_cond(node->is_dff(), __FILE__, __LINE__);

  connect(inode, node, 1);
}

// @brief DFFノードのセット入力を設定する．
// @param[in] node 対象の DFF ノード
// @param[in] inode 入力のノード
void
CmnMgr::set_dff_set(CmnNode* node,
		     CmnNode* inode)
{
  assert_cond(node->is_dff(), __FILE__, __LINE__);

  connect(inode, node, 2);
}

// @brief DFFノードのリセット入力を設定する．
// @param[in] node 対象の DFF ノード
// @param[in] inode 入力のノード
void
CmnMgr::set_dff_rst(CmnNode* node,
		     CmnNode* inode)
{
  assert_cond(node->is_dff(), __FILE__, __LINE__);

  connect(inode, node, 3);
}

// 新しいノードを作成する．
// 作成されたノードを返す．
CmnNode*
CmnMgr::new_node(ymuint ni)
{
  CmnNode* node = NULL;

  // 空いているIDを探してノード配列へ登録
  int id = mItvlMgr.avail_num();
  mItvlMgr.erase(id);

  ymuint uid = static_cast<ymuint>(id);
  while( mNodeArray.size() <= uid ) {
    // mNodeArray の大きさが小さいときのための埋め草
    void* p = mAlloc.get_memory(sizeof(CmnNode));
    CmnNode* node = new (p) CmnNode;
    mNodeArray.push_back(node);
  }
  node = mNodeArray[uid];
  node->mNi = ni;
  if ( ni ) {
    void* q = mAlloc2.get_memory(sizeof(CnEdge) * ni);
    node->mFanins = new (q) CnEdge[ni];
    for (ymuint i = 0; i < ni; ++ i) {
      node->mFanins[i].set_to(node, i);
    }
  }
  else {
    node->mFanins = NULL;
  }
  node->mId = uid;

  return node;
}

// 空にする．
void
CmnMgr::clear()
{
  mName.clear();

  // まず最初に接続を切る．
  for (CmnNodeList::iterator p = mOutputList.begin();
       p != mOutputList.end(); ++ p) {
    CmnNode* node = *p;
    connect(NULL, node, 0);
  }
  for (CmnNodeList::iterator p = mCellnodeList.begin();
       p != mCellnodeList.end(); ++ p) {
    CmnNode* node = *p;
    ymuint ni = node->ni();
    for (ymuint i = 0; i < ni; ++ i) {
      connect(NULL, node, i);
    }
  }
  for (CmnNodeList::iterator p = mDffList.begin();
       p != mDffList.end(); ++ p) {
    CmnNode* node = *p;
    for (ymuint i = 0; i < 4; ++ i) {
      connect(NULL, node, i);
    }
  }

  for (CmnNodeList::iterator p = mInputList.begin();
       p != mInputList.end(); ) {
    // p は実際にはノード内のメンバをアクセスするので delete する前に
    // 進めておく必要がある．
    CmnNode* node = *p;
    ++ p;
    delete_input(node);
  }
  assert_cond(mInputList.empty(), __FILE__, __LINE__);

  for (CmnNodeList::iterator p = mOutputList.begin();
       p != mOutputList.end(); ) {
    // p は実際にはノード内のメンバをアクセスするので delete する前に
    // 進めておく必要がある．
    CmnNode* node = *p;
    ++ p;
    delete_output(node);
  }
  assert_cond(mOutputList.empty(), __FILE__, __LINE__);

  for (CmnNodeList::iterator p = mCellnodeList.begin();
       p != mCellnodeList.end(); ) {
    // p は実際にはノード内のメンバをアクセスするので delete する前に
    // 進めておく必要がある．
    CmnNode* node = *p;
    ++ p;
    delete_cellnode(node);
  }
  assert_cond(mCellnodeList.empty(), __FILE__, __LINE__);

  for (CmnNodeList::iterator p = mDffList.begin();
       p != mDffList.end(); ) {
    // p は実際にはノード内のメンバをアクセスするので delete する前に
    // 進めておく必要がある．
    CmnNode* node = *p;
    ++ p;
    delete_dff(node);
  }
  assert_cond(mDffList.empty(), __FILE__, __LINE__);

  mInputArray.clear();
  mOutputArray.clear();
}

// 入力ノードの削除
void
CmnMgr::delete_input(CmnNode* node)
{
  assert_cond(node->is_input(), __FILE__, __LINE__);
  mInputList.erase(node);
  delete_node(node);
}

// 出力ノードの削除
void
CmnMgr::delete_output(CmnNode* node)
{
  assert_cond(node->is_output(), __FILE__, __LINE__);
  mOutputList.erase(node);
  delete_node(node);
}

// セルノードの削除
void
CmnMgr::delete_cellnode(CmnNode* node)
{
  assert_cond(node->is_cellnode(), __FILE__, __LINE__);
  assert_cond(node->fanout_num() == 0, __FILE__, __LINE__);
  ymuint ni = node->ni();
  for (ymuint i = 0; i < ni; ++ i) {
    connect(NULL, node, i);
  }

  mCellnodeList.erase(node);
  delete_node(node);
}

// @brief DFFノードを削除する．
// @param[in] node 対象のノード
// @note node のファンアウトは空でなければならない．
void
CmnMgr::delete_dff(CmnNode* node)
{
  assert_cond(node->is_dff(), __FILE__, __LINE__);
  assert_cond(node->fanout_num() == 0, __FILE__, __LINE__);
  connect(NULL, node, 0);

  mDffList.erase(node);
  delete_node(node);
}

// node を削除する．
void
CmnMgr::delete_node(CmnNode* node)
{
  // new_node の逆の処理を行なう．
  mItvlMgr.add(static_cast<int>(node->mId));
  // 本当は CnEdge のデストラクタを起動する必要があるが中身がないので
  // 省略する．
  if ( node->ni() ) {
    mAlloc2.put_memory(sizeof(CnEdge)* node->ni(), node->mFanins);
  }
}

// from を to の pos 番目のファンインとする．
// to の pos 番目にすでに接続があった場合には自動的に削除される．
void
CmnMgr::connect(CmnNode* from,
		 CmnNode* to,
		 ymuint pos)
{
  // CmnNode::mFaoutList を変更するのはここだけ

  CnEdge* edge = to->fanin_edge(pos);
  CmnNode* old_from = edge->from();
  if ( old_from ) {
    old_from->mFanoutList.erase(edge);
    old_from->scan_po();
  }
  edge->set_from(from);
  if ( from ) {
    from->mFanoutList.push_back(edge);
    from->scan_po();
  }
}
#endif

END_NAMESPACE_YM_CMN
