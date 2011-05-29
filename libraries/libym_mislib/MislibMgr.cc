
/// @file libym_cell/mislib/MislibMgr.cc
/// @brief MislibMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: MislibMgr.cc 2507 2009-10-17 16:24:02Z matsunaga $
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_mislib/MislibMgr.h"
#include "MislibMgrImpl.h"


BEGIN_NAMESPACE_YM_MISLIB

//////////////////////////////////////////////////////////////////////
// クラス MislibMgr
//////////////////////////////////////////////////////////////////////

// コンストラクタ
MislibMgr::MislibMgr() :
  mImpl(new MislibMgrImpl)
{
}

// デストラクタ
MislibMgr::~MislibMgr()
{
  delete mImpl;
}

// @brief 確保したオブジェクトをすべて削除する．
void
MislibMgr::clear()
{
  mImpl->clear();
}

// @brief ゲートのリストを返す．
const MislibNode*
MislibMgr::gate_list() const
{
  return mImpl->gate_list();
}

END_NAMESPACE_YM_MISLIB
