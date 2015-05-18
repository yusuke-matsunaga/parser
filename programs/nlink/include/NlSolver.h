#ifndef NLSOLVER_H
#define NLSOLVER_H

/// @file NlSolver.h
/// @brief NlSolver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "nlink_nsdef.h"
#include "YmLogic/SatSolver.h"


BEGIN_NAMESPACE_YM_NLINK

//////////////////////////////////////////////////////////////////////
/// @class NlSolver NlSolver.h "NlSolver.h"
/// @brief number link を解くためのクラス
//////////////////////////////////////////////////////////////////////
class NlSolver
{
public:

  /// @brief コンストラクタ
  NlSolver();

  /// @brief デストラクタ
  ~NlSolver();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 問題を解く
  /// @param[in] problem 問題
  /// @param[out] solution 解
  void
  solve(const NlProblem& problem,
	NlSolution& solution);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いいられるデータ構造
  //////////////////////////////////////////////////////////////////////

  // 辺を表すデータ構造
  struct Edge
  {

    // 隣接する辺のリスト
    vector<Edge*> mAdjList;

    // 変数番号
    VarId mVar;

  };

  // ノードを表すデータ構造
  struct Node
  {

    // 端点番号
    // 0 で無印
    ymuint mEndMark;

    // 接続する辺のリスト
    vector<Edge*> mEdgeList;

    // 変数番号の配列
    vector<VarId> mVarArray;

  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をクリアする．
  void
  clear();

  /// @brief 問題を設定する．
  /// @param[in] problem 問題
  void
  set_problem(const NlProblem& problem);

  /// @brief 基本的な制約を作る．
  void
  make_base_cnf(SatSolver& solver);

  /// @brief ノードを得る．
  Node*
  _node(ymuint x,
	ymuint y);

  /// @brief 左の辺を得る．
  /// @param[in] x, y 辺の右端の座標
  Edge*
  left_edge(ymuint x,
	    ymuint y);

  /// @brief 右の辺を得る．
  /// @param[in] x, y 辺の左端の座標
  Edge*
  right_edge(ymuint x,
	     ymuint y);

  /// @brief 上の辺を得る．
  /// @param[in] x, y 辺の下端の座標
  Edge*
  upper_edge(ymuint x,
	     ymuint y);

  /// @brief 下の辺を得る．
  /// @param[in] x, y 辺の上端の座標
  Edge*
  lower_edge(ymuint x,
	     ymuint y);

  /// @brief 解を出力する．
  /// @param[in] model SATの解
  /// @param[in] solution 解
  void
  setup_solution(const vector<Bool3>& model,
		 NlSolution& solution);

  /// @brief 解を出力する．
  /// @param[in] s 出力先のストリーム
  /// @param[in] model SATの解
  void
  print_solution(ostream& s,
		 const vector<Bool3>& model);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 幅
  ymuint mWidth;

  // 高さ
  ymuint mHeight;

  // 線分数
  ymuint mNum;

  // 横の辺の配列
  vector<Edge*> mHarray;

  // 縦の辺の配列
  vector<Edge*> mVarray;

  // ノードの配列
  vector<Node*> mNodeArray;


};

END_NAMESPACE_YM_NLINK

#endif // NLSOLVER_H
