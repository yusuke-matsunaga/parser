#ifndef GBMBDDCEGARONEHOT_H
#define GBMBDDCEGARONEHOT_H

/// @file GbmBddCegarOneHot.h
/// @brief GbmBddCegarOneHot のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "GbmSolver.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class GbmBddCegarOneHot GbmBddCegarOneHot.h "GbmBddCegarOneHot.h"
/// @brief BDD/CEGAR ベースの GbmSolver
//////////////////////////////////////////////////////////////////////
class GbmBddCegarOneHot :
  public GbmSolver
{
public:

  /// @brief コンストラクタ
  GbmBddCegarOneHot();

  /// @brief デストラクタ
  virtual
  ~GbmBddCegarOneHot();


private:
  //////////////////////////////////////////////////////////////////////
  // GbmSolver の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力順を考慮したマッチング問題を解く
  /// @param[in] network RcfNetwork
  /// @param[in] output Reconfigurable Network の出力
  /// @param[in] func マッチング対象の関数
  /// @param[in] rep 関数の対称変数の代表番号を収める配列
  ///            rep[pos] に pos 番めの入力の代表番号が入る．
  /// @param[out] conf_bits configuration ビットの値を収める配列
  /// @param[out] iorder 入力順序
  ///             iorder[pos] に network の pos 番めの入力に対応した
  ///             関数の入力番号が入る．
  bool
  _solve(const RcfNetwork& network,
	 const TvFunc& func,
	 const vector<ymuint>& rep,
	 vector<bool>& conf_bits,
	 vector<ymuint>& iorder);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

};

END_NAMESPACE_YM


#endif // GBMBDDCEGARONEHOT_H