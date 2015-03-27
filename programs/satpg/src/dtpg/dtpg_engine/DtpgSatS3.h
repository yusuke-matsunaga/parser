﻿#ifndef DTPGSATS3_H
#define DTPGSATS3_H

/// @file DtpgSatS3.h
/// @brief DtpgSatS3 のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014, 2015 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgSatBaseS.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DtpgSatS3 DtpgSatS3.h "DtpgSatS3.h"
/// @brief 1つの故障を対象とした CNF を生成する DtpgSat
//////////////////////////////////////////////////////////////////////
class DtpgSatS3 :
  public DtpgSatBaseS
{
public:

  /// @brief コンストラクタ
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] bt バックトレーサー
  /// @param[in] dop パタンが求められた時に実行されるファンクタ
  /// @param[in] uop 検出不能と判定された時に実行されるファンクタ
  DtpgSatS3(const string& sat_type,
	    const string& sat_option,
	    ostream* sat_outp,
	    BackTracer& bt,
	    DetectOp& dop,
	    UntestOp& uop);

  /// @brief デストラクタ
  virtual
  ~DtpgSatS3();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  /// @param[in] network 対象のネットワーク
  /// @param[in] f_tgt 対象の故障
  virtual
  void
  run_single(TpgNetwork& network,
	     TpgFault* f_tgt);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // バックトレーサー
  BackTracer* mBt;

};

END_NAMESPACE_YM_SATPG

#endif // DTPGSATS3_H
