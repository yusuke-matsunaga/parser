#ifndef YM_NETWORKS_TGBLIFREADER_H
#define YM_NETWORKS_TGBLIFREADER_H

/// @file ym_networks/TgBlifReader.h
/// @brief TgBlifReader のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: TgBlifReader.h 1978 2009-02-06 12:29:16Z matsunaga $
///
/// Copyright (C) 2005-2010 Yusuke Matsunaga
/// All rights reserved.


#include "ym_networks/tgnet_nsdef.h"
#include "ym_blif/BlifParser.h"


BEGIN_NAMESPACE_YM_TGNET

class TgBlifHandler;

//////////////////////////////////////////////////////////////////////
/// @class TgBlifReader TgBlifReader.h <ym_networks/TgBlifReader.h>
/// @brief blif 形式のファイルを読み込むためのクラス
//////////////////////////////////////////////////////////////////////
class TgBlifReader
{
public:

  /// @brief コンストラクタ
  TgBlifReader();

  /// @brief デストラクタ
  ~TgBlifReader();


public:

  /// @brief blif ファイルを読み込む．
  /// @param[in] filename ファイル名
  /// @param[in] network 設定対象のネットワーク
  bool
  operator()(const string& filename,
	     TgNetwork& network);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // Blif ファイルを読み込むためのパーサ
  BlifParser mParser;

  // Blif ファイルを TgNetwork に変換するためのハンドラ
  TgBlifHandler* mHandler;

};

END_NAMESPACE_YM_TGNET

#endif // YM_NETWORKS_TGBLIFREADER_H
