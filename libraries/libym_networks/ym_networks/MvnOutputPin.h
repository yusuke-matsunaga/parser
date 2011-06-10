#ifndef YM_NETWORKS_MVNOUTPUTPIN_H
#define YM_NETWORKS_MVNOUTPUTPIN_H

/// @file ym_networks/MvnOutputPin.h
/// @brief MvnOutputPin のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_networks/mvn_nsdef.h"


BEGIN_NAMESPACE_YM_MVN

//////////////////////////////////////////////////////////////////////
/// @class MvnOutputPin MvPin.h "ym_networks/MvPin.h"
/// @brief ノードの出力ピンを表すクラス
//////////////////////////////////////////////////////////////////////
class MvnOutputPin
{
  friend class MvnMgr;
  friend class MvnNodeBase;

private:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ / デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  MvnOutputPin();

  /// @brief デストラクタ
  ~MvnOutputPin();


public:

  /// @breif 属しているノードを得る．
  MvnNode*
  node() const;

  /// @brief ビット幅を得る．
  ymuint
  bit_width() const;

  /// @brief ノードの何番目のピンかを返す．
  /// @note 入力ピンと出力ピンは区別される．
  ymuint
  pos() const;

  /// @brief 接続している入力ピンのリストを得る．
  const MvnInputPinList&
  dst_pin_list() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 設定用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 設定用の関数
  void
  init(MvnNode* node,
       ymuint pos);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 親のノード
  MvnNode* mNode;

  // 位置
  ymuint32 mPos;

  // ビット幅
  ymuint32 mBitWidth;

  // 接続している入力ピンのリスト
  MvnInputPinList mDstPinList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @breif 属しているノードを得る．
inline
MvnNode*
MvnOutputPin::node() const
{
  return mNode;
}

// @brief ビット幅を得る．
inline
ymuint
MvnOutputPin::bit_width() const
{
  return mBitWidth;
}

// @brief ノードの何番目のピンかを返す．
// @note 入力ピンと出力ピンは区別される．
inline
ymuint
MvnOutputPin::pos() const
{
  return mPos;
}

// @brief 接続している入力ピンのリストを得る．
inline
const MvnInputPinList&
MvnOutputPin::dst_pin_list() const
{
  return mDstPinList;
}

// @brief 設定用の関数
inline
void
MvnOutputPin::init(MvnNode* node,
		   ymuint pos)
{
  mNode = node;
  mPos = pos;
  mBitWidth = 1;
}

END_NAMESPACE_YM_MVN

#endif // YM_NETWORKS_MVNPIN_H
