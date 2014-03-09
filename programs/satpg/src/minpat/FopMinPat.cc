
/// @file FopMinPat.cc
/// @brief FopMinPat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2013-2014 Yusuke Matsunaga
/// All rights reserved.


#include "FopMinPat.h"
#include "Fsim.h"
#include "FaultMgr.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス FopMinPat
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fsim 故障シミュレータ
// @param[in] fmgr 故障マネージャ
FopMinPat::FopMinPat(Fsim& fsim,
		     FaultMgr& fmgr) :
  mFsim(fsim),
  mLimit(1),
  mFinfoArray(fmgr.all_num())
{
  mFsim.set_faults(fmgr.det_list());
}

// @brief デストラクタ
FopMinPat::~FopMinPat()
{
}

// @brief 故障を検出したときの処理
// @param[in] f 故障
// @param[in] dpat 検出したパタンを表すビットベクタ
void
FopMinPat::operator()(TpgFault* f,
		      PackedVal dpat)
{
  ymuint f_id = f->id();
  FaultInfo& finfo = mFinfoArray[f_id];
  for (ymuint i = 0; i < kPvBitLen; ++ i) {
    if ( dpat & (1UL << i) ) {
      finfo.mPatList.push_back(mCurPatList[i]);
      ++ finfo.mDetCount;
      if ( finfo.mDetCount >= mLimit ) {
	// 規定回数以上検出されたので以後のシミュレーションではスキップする．
	mFsim.set_skip(f);
	break;
      }
    }
  }
}

// @brief 検出回数をクリアする．
void
FopMinPat::clear_count()
{
  ymuint n = mFinfoArray.size();
  for (ymuint i = 0; i < n; ++ i) {
    FaultInfo& finfo = mFinfoArray[i];
    finfo.mDetCount = 0;
    finfo.mPatList.clear();
  }
}

// @brief 検出回数のしきい値をセットする．
void
FopMinPat::set_limit(ymuint limit)
{
  mLimit = limit;
}

// @brief パタンをセットする．
void
FopMinPat::set_pattern(const vector<TestVector*>& pat_list)
{
  mCurPatList = pat_list;
}

// @brief 故障に対するパタンのリストを返す．
// @param[in] f_id 故障の ID
const vector<TestVector*>&
FopMinPat::pat_list(ymuint f_id)
{
  return mFinfoArray[f_id].mPatList;
}

END_NAMESPACE_YM_SATPG
