
/// @file SatEngineMulti.cc
/// @brief SatEngineMulti の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014 Yusuke Matsunaga
/// All rights reserved.


#include "SatEngineMulti.h"

#include "DetectOp.h"
#include "UntestOp.h"
#include "DtpgStats.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "BackTracer.h"
#include "LitMap.h"
#include "logic/SatSolver.h"
#include "logic/SatStats.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
SatEngineMulti::SatEngineMulti()
{
}

// @brief デストラクタ
SatEngineMulti::~SatEngineMulti()
{
}

// @brief テストパタン生成を行なう．
// @param[in] flist 故障リスト
// @param[in] max_id ノード番号の最大値 + 1
void
SatEngineMulti::run(const vector<TpgFault*>& flist,
		    ymuint max_id,
		    BackTracer& bt,
		    DetectOp& dop,
		    UntestOp& uop)
{
  cnf_begin();

  SatSolver solver(sat_type(), sat_option(), sat_outp());

  ymuint nf = flist.size();

  // 故障を活性化するとき true にする変数．
  vector<VarId> flt_var(nf);
  // FnodeInfo を持つノードのリスト
  vector<TpgNode*> fnode_list;

  mDone.clear();
  mDone.resize(max_id, false);
  fnode_list.reserve(nf);
  for (ymuint i = 0; i < nf; ++ i) {
    VarId fvar = solver.new_var();
    flt_var[i] = fvar;
    TpgFault* f = flist[i];
    int fval = f->val();
    TpgNode* node = f->node();
    if ( !mDone[node->id()] ) {
      mDone[node->id()] = true;
      fnode_list.push_back(node);
    }

    if ( f->is_output_fault() ) {
      node->set_ofvar(fval, fvar);
    }
    else {
      ymuint pos = f->pos();
      node->set_ifvar(pos, fval, fvar);
    }
  }

  mark_region(solver, fnode_list, max_id);


  //////////////////////////////////////////////////////////////////////
  // 正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  for (ymuint i = 0; i < tfo_tfi_size(); ++ i) {
    TpgNode* node = tfo_tfi_node(i);
    make_gnode_cnf(solver, node);
  }

  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  for (ymuint i = 0; i < tfo_size(); ++ i) {
    TpgNode* node = tfo_tfi_node(i);

    make_fnode_cnf(solver, node);

    make_dchain_cnf(solver, node);
  }

  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件
  //////////////////////////////////////////////////////////////////////
  if ( mNemesis ) {
    ymuint npo = output_list().size();
    tmp_lits_begin(npo);
    for (ymuint i = 0; i < npo; ++ i) {
      TpgNode* node = output_list()[i];
      Literal dlit(node->dvar(), false);
      tmp_lits_add(dlit);
    }
    tmp_lits_end(solver);
  }

  cnf_end();

  // 個々の故障に対するテスト生成を行なう．
  for (ymuint fid = 0; fid < nf; ++ fid) {
    TpgFault* f = flist[fid];

    if ( f->status() == kFsDetected ) {
      // 他の故障のパタンで検出済みになっている場合がある．
      continue;
    }

    tmp_lits_begin();

    // 該当の故障に対する変数のみ1にする．
    for (ymuint j = 0; j < nf; ++ j) {
      bool inv = (j != fid);
      tmp_lits_add(Literal(flt_var[j], inv));
    }

    TpgNode* fnode = flist[fid]->node();

    // 故障ノードの TFO 以外の dlit を0にする．
    mTmpNodeList.clear();
    mTmpNodeList.reserve(tfo_tfi_size());
    set_tmp_mark(fnode);
    mTmpNodeList.push_back(fnode);
    for (ymuint rpos = 0; rpos < mTmpNodeList.size(); ++ rpos) {
      TpgNode* node = mTmpNodeList[rpos];
      ymuint nfo = node->active_fanout_num();
      for (ymuint i = 0; i < nfo; ++ i) {
	TpgNode* fonode = node->active_fanout(i);
	if ( !tmp_mark(fonode) ) {
	  set_tmp_mark(fonode);
	  mTmpNodeList.push_back(fonode);
	}
      }
    }
    for (ymuint i = 0; i < tfo_tfi_size(); ++ i) {
      TpgNode* node = tfo_tfi_node(i);
      if ( node->has_fvar() && !tmp_mark(node) ) {
	Literal dlit(node->dvar(), true);
	tmp_lits_add(dlit);
      }
    }
    for (vector<TpgNode*>::iterator p = mTmpNodeList.begin();
	 p != mTmpNodeList.end(); ++ p) {
      TpgNode* node = *p;
      clear_tmp_mark(node);
    }
    mTmpNodeList.clear();

    if ( mUseDominator ) {
      // dominator ノードの dvar は1でなければならない．
      for (TpgNode* node = f->node(); node != NULL; node = node->imm_dom()) {
	Literal dlit(node->dvar(), false);
	tmp_lits_add(dlit);
      }
    }
    else {
      tmp_lits_add(Literal(f->node()->dvar(), false));
    }

    solve(solver, f, bt, dop, uop);
  }
  clear_node_mark();

  for (ymuint i = 0; i < fnode_list.size(); ++ i) {
    TpgNode* node = fnode_list[i];
    node->clear_oifvar();
  }
}

END_NAMESPACE_YM_SATPG
