
/// @file DopDrop.cc
/// @brief DopDrop の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013-2014 Yusuke Matsunaga
/// All rights reserved.


#include "DopDrop.h"
#include "AtpgMgr.h"
#include "Fsim.h"
#include "FaultMgr.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief 'drop' タイプを生成する．
// @param[in] mgr AtpgMgr
DetectOp*
new_DopDrop(AtpgMgr& mgr)
{
  return new DopDrop(mgr._fault_mgr(), mgr._fsim3());
}


//////////////////////////////////////////////////////////////////////
// クラス DopDrop
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fmgr 故障マネージャ
// @param[in] fsim 故障シミュレータ
DopDrop::DopDrop(FaultMgr& fmgr,
		 Fsim& fsim) :
  mMgr(fmgr),
  mFsim(fsim)
{
}

// @brief デストラクタ
DopDrop::~DopDrop()
{
}

// @brief 故障リストを設定する．
// @param[in] fault_list 故障リスト
void
DopDrop::set_faults(const vector<TpgFault*>& fault_list)
{
  mFsim.set_faults(fault_list);
}

// @brief テストパタンが見つかった時の処理
// @param[in] f 故障
// @param[in] tv テストパタン
void
DopDrop::operator()(TpgFault* f,
		    TestVector* tv)
{
  vector<TpgFault*> det_faults;
  mFsim.sppfp(tv, det_faults);
  for (vector<TpgFault*>::iterator p = det_faults.begin();
       p != det_faults.end(); ++ p) {
    TpgFault* f = *p;
    mMgr.set_status(f, kFsDetected);
    mFsim.set_skip(f);
  }
}

END_NAMESPACE_YM_SATPG
