#ifndef LIBYM_LOGIC_BDD_BMM_BDDMGRMODERN_H
#define LIBYM_LOGIC_BDD_BMM_BDDMGRMODERN_H

/// @file libym_logic/bdd/bmm/BddMgrModern.h
/// @brief BddMgrModern のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_logic/Bdd.h"

#include "base/BddMgrImpl.h"
#include "BmmNode.h"


BEGIN_NAMESPACE_YM_BDD

//////////////////////////////////////////////////////////////////////
// クラス名の前方参照用の宣言および型名の定義
//////////////////////////////////////////////////////////////////////

class BmmVar;
class BmmNode;
class BmmCompTbl;
class BmmCompTbl1;
class BmmCompTbl2;
class BmmCompTbl3;
class BmmIsopTbl;


//////////////////////////////////////////////////////////////////////
// BddMgr に共通な関数を提供するためのベースクラス
//////////////////////////////////////////////////////////////////////
class BddMgrModern :
  public BddMgrImpl
{
public:
  typedef BmmVar Var;
  typedef BmmNode Node;
  typedef BmmCompTbl CompTbl;
  typedef BmmCompTbl1 CompTbl1;
  typedef BmmCompTbl2 CompTbl2;
  typedef BmmCompTbl3 CompTbl3;
  typedef BmmIsopTbl IsopTbl;

public:

  /// @brief コンストラクタ
  /// @param[in] name 名前
  /// @param[in] option オプション
  BddMgrModern(const string& name = string(),
	       const string& option = string());

  // デストラクタ
  virtual
  ~BddMgrModern();


  //////////////////////////////////////////////////////////////////////
  // BDDの根の枝の参照回数関数
  //////////////////////////////////////////////////////////////////////

  // e の参照回数を増やす．
  virtual
  void
  inc_rootref(BddEdge e);

  // e の参照回数を減らす．
  virtual
  void
  dec_rootref(BddEdge e);


  //////////////////////////////////////////////////////////////////////
  // 変数番号とレベルの対応づけ
  //////////////////////////////////////////////////////////////////////

  // Var 側に節点テーブルを持つとき true を返す．
  bool
  is_reorderable() const;

  // 変数を確保する．
  // 確保に失敗したら false を返す．
  // 最後の変数の後ろに挿入される．
  virtual
  bool
  new_var(VarId varid);

  // 現在登録されている変数をそのレベルの昇順で返す．
  virtual
  tVarSize
  var_list(list<VarId>& vlist) const;

  // 変数番号からレベルを得る．
  // もしもレベルが割り当てられていない場合にはエラーとなる．
  virtual
  tLevel
  level(VarId varid) const;

  // レベルから変数番号を得る．
  virtual
  VarId
  varid(tLevel level) const;

  // 動的変数順変更を許可する．
  virtual
  void
  enable_DVO();

  // 動的変数順変更を禁止する．
  virtual
  void
  disable_DVO();


  //////////////////////////////////////////////////////////////////////
  // BDD 生成用関数
  //////////////////////////////////////////////////////////////////////

  // 肯定のリテラル関数を作る
  virtual
  BddEdge
  make_posiliteral(VarId varid);


  //////////////////////////////////////////////////////////////////////
  // built-in タイプの論理演算
  //////////////////////////////////////////////////////////////////////

  // src1 & src2 を計算する．
  virtual
  BddEdge
  and_op(BddEdge e1,
	 BddEdge e2);

  // src1 ^ src2 を計算する．
  virtual
  BddEdge
  xor_op(BddEdge e1,
	 BddEdge e2);

  // src1 と src2 の共通部分があれば kEdge1 を返す．
  virtual
  BddEdge
  check_intersect(BddEdge e1,
		  BddEdge e2);

  // Davio展開のモーメント項($f_{\overline{x}} \oplus f_x$)を
  // 求める処理
  virtual
  BddEdge
  xor_moment(BddEdge e,
	     VarId idx);

  // bdd がキューブの時 true を返す．
  virtual
  bool
  check_cube(BddEdge e);

  // bdd が肯定リテラルのみからなるキューブの時 true を返す．
  virtual
  bool
  check_posi_cube(BddEdge e);

  // 変数xとyが対称(交換可能)な時にtrueを返す．
  virtual
  bool
  check_symmetry(BddEdge e,
		 VarId x,
		 VarId y,
		 tPol pol);

  // 一つの変数に対する cofactor を計算する．
  virtual
  BddEdge
  scofactor(BddEdge e1,
	    VarId id,
	    tPol pol);

  // generalized cofactor を計算する．
  virtual
  BddEdge
  gcofactor(BddEdge e1,
	    BddEdge e2);

  // if-then-else 演算を計算する．
  virtual
  BddEdge
  ite_op(BddEdge e1,
	 BddEdge e2,
	 BddEdge e3);

  // multiple compose 演算を行うために最初に呼ばれる関数．
  virtual
  void
  compose_start();

  // multiple compose 演算を行う変数と置き換え関数を登録する関数
  virtual
  void
  compose_reg(VarId id,
	      BddEdge e);

  // multiple compose 演算の本体
  virtual
  BddEdge
  compose(BddEdge e);

  // x_level の変数を y_level まで「押し込む」．
  // pol が kPolNega の時は 0-枝と 1-枝を取り替える．
  virtual
  BddEdge
  push_down(BddEdge e,
	    tLevel x_level,
	    tLevel y_level,
	    tPol pol);

  // smoothing(elimination)
  // svars に含まれる変数を消去する．
  virtual
  BddEdge
  esmooth(BddEdge e1,
	  BddEdge e2);

  // src1 と src2 の論理積を計算して src3 の変数を消去する．
  virtual
  BddEdge
  and_exist(BddEdge e1,
	    BddEdge e2,
	    BddEdge e3);

  // lower と upper で指定された不完全指定論理関数の非冗長積和形を求める．
  virtual
  BddEdge
  isop(BddEdge l,
       BddEdge u,
       LogExpr& cover);

  // lower と upper で指定された不完全指定論理関数の主項カバーを求める．
  virtual
  LogExpr
  prime_cover(BddEdge l,
	      BddEdge u);

  // lower と upper で指定された不完全指定論理関数の極小サポート集合をすべて
  // 列挙する．解は論理関数の形で返される．そのなかの主項がサポート集合に
  // 対応している．
  virtual
  BddEdge
  minimal_support(BddEdge l,
		  BddEdge u);

  // smallest cube containing F 演算
  virtual
  BddEdge
  SCC(BddEdge e);


  //////////////////////////////////////////////////////////////////////
  // BDD の構造に関係したメソッド
  //////////////////////////////////////////////////////////////////////

  // 根の節点の変数に基づいてShannon展開を行なう．
  // 戻り値として根の節点の変数番号を返し，その変数を0/1に固定した
  // 時の cofactor をそれぞれ f0, f1 に入れる．
  // もともと定数値(葉)のBDDの場合，kVarIdMax を返し，
  // f0, f1 には自分自身を代入する．
  virtual
  VarId
  root_decomp(BddEdge e,
	      BddEdge& e0,
	      BddEdge& e1);

  // 根の変数番号インデックスを取り出す．
  // 定数節点の場合には kVarIdMax を返す．
  virtual
  VarId
  root_var(BddEdge e);

  // 0枝の指している cofactor を返す．
  // 定数節点の場合には自分自身を返す．
  virtual
  BddEdge
  edge0(BddEdge e);

  // 1枝の指している cofactor を返す．
  // 定数節点の場合には自分自身を返す．
  virtual
  BddEdge
  edge1(BddEdge e);

  // e の参照回数が0なら true を返す．
  // e が終端ノードの場合には false を返す．
  virtual
  bool
  check_noref(BddEdge e);


  //////////////////////////////////////////////////////////////////////
  // 1へ至るパスを求める関数
  //////////////////////////////////////////////////////////////////////

  // 1パスを求める．
  // 結果はその経路のみのBDDとなる．
  virtual
  BddEdge
  onepath(BddEdge e);

  // 最短の1パスを求める．
  // 結果はその経路のみのBDDとなる．
  virtual
  BddEdge
  shortest_onepath(BddEdge e);

  // 最短の1パスの長さを求める．
  virtual
  tVarSize
  shortest_onepath_len(BddEdge e);


  //////////////////////////////////////////////////////////////////////
  // ノード数の計数や真理値表密度の計算など
  //////////////////////////////////////////////////////////////////////

  // e を根とするBDDのノード数を数える．
  virtual
  size_t
  size(BddEdge e);

  // edge_list に登録されたBDDのノード数を数える．
  virtual
  size_t
  size(const list<BddEdge>& edge_list);

  // BDD の表す論理関数の minterm の数を返す．
  // 無限長精度の整数(mpz_class)を用いて計算する．
  // n は論理関数の変数の数
  virtual
  mpz_class
  minterm_count(BddEdge e,
		tVarSize n);

  // Walsh 変換の0次の係数を計算する．
  // n は論理関数の変数の数
  virtual
  mpz_class
  walsh0(BddEdge e,
	 tVarSize n);

  // Walsh 変換の1次の係数を計算する．
  // n は論理関数の変数の数
  virtual
  mpz_class
  walsh1(BddEdge e,
	 VarId var,
	 tVarSize n);


  //////////////////////////////////////////////////////////////////////
  // サポート関係の関数
  //////////////////////////////////////////////////////////////////////

  // e を根とするBDDのサポートに印をつける．
  virtual
  tVarSize
  mark_support(BddEdge e);

  // edge_list に登録されたBDDのサポートに印をつける．
  virtual
  tVarSize
  mark_support(const list<BddEdge>& edge_list);

  // 印のついた変数をベクタに変換する．
  virtual
  tVarSize
  mark_to_vector(VarVector& support);

  // 印のついた変数をリストに変換する．
  virtual
  tVarSize
  mark_to_list(VarList& support);

  // 印のついた変数をBDD(キューブ)に変換する．
  virtual
  BddEdge
  mark_to_bdd();


  //////////////////////////////////////////////////////////////////////
  // VarSet の内部で用いられる演算
  //////////////////////////////////////////////////////////////////////

  // src1 と src2 が変数集合の時に共通部分を求める．
  virtual
  BddEdge
  vscap(BddEdge e1,
	BddEdge e2);

  // src1 と src2 が変数集合の時に集合差を求める．
  virtual
  BddEdge
  vsdiff(BddEdge e1,
	 BddEdge e2);

  // src1 と src2 が変数集合の時のインターセクションチェック
  virtual
  bool
  vsintersect(BddEdge e1,
	      BddEdge e2);


  //////////////////////////////////////////////////////////////////////
  // LitSet の内部で用いられる演算
  //////////////////////////////////////////////////////////////////////

  // src1 と src2 がリテラル集合の時に共通部分を求める．
  virtual
  BddEdge
  lscap(BddEdge e1,
	BddEdge e2);

  // src1 と src2 がリテラル集合の時に集合差を求める．
  virtual
  BddEdge
  lsdiff(BddEdge e1,
	 BddEdge e2);

  // src1 と src2 がリテラル集合の時のインターセクションチェック
  virtual
  bool
  lsintersect(BddEdge e1,
	      BddEdge e2);

  // LitSet 用のBDDからリテラルのベクタを作る．
  virtual
  tVarSize
  to_literalvector(BddEdge e,
		   LiteralVector& dst);

  // LitSet 用のBDDからリテラルのリストを作る．
  virtual
  tVarSize
  to_literallist(BddEdge e,
		 LiteralList& dst);


  //////////////////////////////////////////////////////////////////////
  // 内部動作の設定を行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ガーベージコレクションを許可する．
  virtual
  void
  enable_gc();

  /// @brief ガーベージコレクションを禁止する．
  virtual
  void
  disable_gc();

  // ガーベージコレクションを行なう．
  // shrink_nodetable = true の時, 可能なら節点テーブルのサイズを縮小する．
  virtual
  void
  gc(bool shrink_nodetable);

  // GC 前の sweep 処理を行うためのバインダーを登録する．
  virtual
  void
  reg_sweep_binder(EventBinder* binder);

  // パラメータを設定する．設定したい項目のマスクビットを1とする．
  virtual
  void
  param(const BddMgrParam& param,
	ymuint32 mask);

  // パラメータを取得する．
  virtual
  void
  param(BddMgrParam& param) const;

  // 節点テーブルの拡張を制御するパラメータを得る．
  double
  nt_load_limit() const;

  // 名前を得る．
  virtual
  const
  string& name() const;

  // 使用メモリ量(in bytes)を得る．
  virtual
  size_t
  used_mem() const;

  // 節点テーブルに登録されているノードの数を得る．
  virtual
  size_t
  node_num() const;

  // GC で回収される(フリーになる)ノード数を得る．
  virtual
  size_t
  garbage_num() const;

  // 利用可能なフリーノード数を得る．
  virtual
  size_t
  avail_num() const;

  // GC の起動された回数を得る．
  virtual
  size_t
  gc_count() const;


  //////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////

  Var*
  alloc_var(VarId varid);

  // 左右の枝が同じ場合にはその枝自身を返し，それ以外の場合には，
  // 与えられた枝とインデックスを持つノードを返す．
  BddEdge
  new_node(Var* var,
	   BddEdge l,
	   BddEdge h);


  //////////////////////////////////////////////////////////////////////
  // 上記の XXX_op() の内部で用いられる再帰関数
  //////////////////////////////////////////////////////////////////////

  // and_op() の下請け関数
  BddEdge
  and_step(BddEdge e1,
	   BddEdge e2);

  // xor_op() の下請け関数
  BddEdge
  xor_step(BddEdge e1,
	   BddEdge e2);

  // compose 演算を行う．
  BddEdge
  compose_step(BddEdge f);

  // generalized cofactorの内部版．
  BddEdge
  gcofactor_step(BddEdge f,
		 BddEdge c);

  // キューブによる割り算．答は一意に決まる．
  BddEdge
  cube_division(BddEdge f,
		BddEdge c);

  // gcofactor の c がキューブの場合の特別版
  BddEdge
  cubediv_step(BddEdge f);

  // xor cofactor 演算
  BddEdge
  xcofactor_step(BddEdge f);

  // existential qualification を行う．
  BddEdge
  esmooth_step(BddEdge e1);

  // and exist 演算を行う．
  BddEdge
  andexist_step(BddEdge e1,
		BddEdge e2);

  // 最短の1パスを求める．
  BddEdge
  sp_step(BddEdge e,
	  hash_map<BddEdge, BddEdge>& sp_map);

  // ISOP を求める．
  BddEdge
  isop_step(BddEdge l,
	    BddEdge u,
	    LogExpr& cov);

  // prime cover を求める．
  BddEdge
  pc_step(BddEdge l,
	  BddEdge u,
	  LogExpr& cov);

  // minimal support を求める演算
  BddEdge
  ms_step(BddEdge l,
	  BddEdge u);

  // check_symmetry の内部で使われる関数
  BddEdge
  cs_step2(BddEdge e);
  BddEdge
  cs_step1(BddEdge e1,
	   BddEdge e2,
	   tPol sympol);
  BddEdge
  cs_step(BddEdge e,
	  tPol sympol);

  // support 関係の共通処理
  void
  sup_step(BddEdge e);

  // SCC で用いられる関数
  void
  scc_step(BddEdge e,
		BddEdge s);

  // size() の中で用いられる関数
  void
  count1(BddEdge e);

  // density の下請関数
  static
  double
  dens_step(BddEdge e,
	    hash_map<BddEdge, double>& dens_assoc);

  // mpz_class 版の minterm_count の下請関数
  static
  mpz_class
  mterm_step(BddEdge e,
	     hash_map<BddEdge, mpz_class>& mc_map);

  // int 版の minterm_count の下請関数
  static
  ymuint
  mterm_step(BddEdge e,
	     hash_map<BddEdge, ymuint>& mc_map);

  // Walsh spectrumの0次の係数を求める処理
  static
  mpz_class
  wt0_step(BddEdge e,
	   hash_map<Node*, mpz_class>& result_map);

  // Walsh spectrumの0次の係数を求める処理
  // こちらは int 版
  static
  ymint
  wt0_step(BddEdge e,
	   hash_map<Node*, ymint>& result_map);

  // Walsh spectrumの1次の係数を求める処理
  static
  mpz_class
  wt1_step(BddEdge e,
	   hash_map<Node*, mpz_class>& result_map);

  // Walsh spectrumの1次の係数を求める処理
  // int 版
  static
  ymint
  wt1_step(BddEdge e,
	   hash_map<Node*, ymint>& result_map);

  // scan の下請関数
  static
  void
  scan_step(BddEdge e);

  // bdd のレベル level の *** n_mark のついた *** 節点の数を数える．
  // ただし，n_mark の付いていない節点があった場合には UINT_MAX を返す．
  static
  size_t
  count_at_step(BddEdge e,
		tLevel level);

  // sp_step 中で用いられる関数
  static
  ymint
  sp_len(BddEdge e);

  // shortest_onepath_len() 中で用いられる下請関数
  static
  ymint
  spl_step(BddEdge e,
	   hash_map<BddEdge, ymint>& assoc);


  //////////////////////////////////////////////////////////////////////
  // 内部メンバの管理用関数
  //////////////////////////////////////////////////////////////////////

  // 節点テーブルのサイズを返す．
  // メモリ確保に失敗したら false を返す．
  bool
  resize(size_t new_size);

  // 次のリミット値を計算する
  void
  set_next_limit_size();

  // ノードのリンク数を増やし，もしロックされていなければロックする
  void
  activate(BddEdge vd);

  // ノードのリンク数を減らし，他のリンクがなければロックを外す
  void
  deactivate(BddEdge vd);

  // vp と vp の子孫のノードをロックする
  void
  lockall(Node* vp);

  // vp と vp の子孫ノードを(他に参照されていないもののみ)ロックを外す
  void
  unlockall(Node* vp);

  // 演算結果テーブルを登録する．
  void
  add_table(CompTbl* tbl);

  // mVarTable 中のマークを消す．
  void
  clear_varmark();

  // level の変数を取り出す．
  Var*
  var_at(tLevel level) const;

  // varid の変数を取出す．
  Var*
  var_of(VarId varid) const;

  // Var を登録する．
  void
  reg_var(Var* var);


  //////////////////////////////////////////////////////////////////////
  // メモリ管理用の関数
  //////////////////////////////////////////////////////////////////////

  // 節点を確保する．
  Node*
  alloc_node();

  // 節点チャンクをスキャンして参照されていない節点をフリーリストにつなぐ
  // ただし，チャンク全体が参照されていなかった場合にはフリーリストには
  // つながない．その場合には true を返す．
  bool
  scan_nodechunk(Node* blk,
		 size_t blk_size,
		 Node**& prev);

  // 変数テーブル用のメモリを確保する．
  // size はバイト単位ではなくエントリ数．
  Var**
  alloc_vartable(size_t size);

  // 変数テーブル用のメモリを解放する．
  // size はバイト単位ではなくエントリ数
  void
  dealloc_vartable(Var** table,
		   size_t size);

  // 節点テーブル用のメモリを確保する．
  // size はバイト単位ではなくエントリ数
  Node**
  alloc_nodetable(size_t size);

  // 節点テーブル用のメモリを解放する．
  // size はバイト単位ではなくエントリ数
  void
  dealloc_nodetable(Node** table,
		    size_t size);

  // 節点チャンク用のメモリを確保する．
  Node*
  alloc_nodechunk();

  // 節点チャンク用のメモリを解放する．
  void
  dealloc_nodechunk(Node* chunk);

  // このマネージャで使用するメモリ領域を確保する．
  void*
  allocate(size_t size);

  // このマネージャで確保したメモリを解放する．
  void
  deallocate(void* ptr,
	     size_t size);


  //////////////////////////////////////////////////////////////////////
  // BddEdge を操作するクラスメソッド
  //////////////////////////////////////////////////////////////////////

  // 枝の値からそれが指しているノードのポインタをとり出す
  static
  Node*
  get_node(BddEdge e);

  // ノードのポインタと極性から枝の情報を作り出す
  static
  BddEdge
  combine(Node* n,
	  tPol p);

  // p-mark が付いた節点のマークを消す．
  static
  void
  clear_pmark(BddEdge e);

  // n-mark が付いた節点のマークを消す．
  static
  void
  clear_nmark(BddEdge e);

  // vdで示された枝の子孫の全てのマークを消す
  static
  void
  clear_pnmark(BddEdge e);

  // vdの指すノードのマークを調べ，マークされていればtrueを返す．
  // 枝に極性がなければマークは1種類でいいが，極性があるので，
  // 肯定の枝から指された場合の p-mark と否定の枝から指された場
  // 合の n-mark の2種類があるので，枝に応じて切替える．
  static
  bool
  mark(BddEdge vd);

  // vdの指すノードにマークをつける
  static
  void
  setmark(BddEdge vd);

  // idx が top に等しいときには e の子供を e_0, e_1 にセットする．
  // 等しくなければ e をセットする．
  static
  void
  split1(tLevel top,
	 tLevel level,
	 BddEdge e,
	 const Node* vp,
	 tPol pol,
	 BddEdge& e_0,
	 BddEdge& e_1);

  // f と g のノードの子供のノードとレベルを求める．
  static
  Var*
  split(BddEdge f,
	BddEdge g,
	BddEdge& f_0,
	BddEdge& f_1,
	BddEdge& g_0,
	BddEdge& g_1);


private:
  //////////////////////////////////////////////////////////////////////
  // デバッグ用の名前
  //////////////////////////////////////////////////////////////////////

  string mName;


  //////////////////////////////////////////////////////////////////////
  // ユーザーが設定するパラメータ
  // 設定は専用のメソッドを用いる．
  //////////////////////////////////////////////////////////////////////

  // ガーベージの割合がこの値を越えるとGCを起こす．
  double mGcThreshold;

  // ただし，全体のノード数がこの数以下の時はGCは起こさない．
  size_t mGcNodeLimit;

  // 節点テーブル拡張時の制限値を決めるパラメータ
  double mNtLoadLimit;

  // 演算結果テーブル拡張時の制限値を決めるパラメータ
  double mRtLoadLimit;

  // 使用メモリ量の上限
  size_t mMemLimit;


  //////////////////////////////////////////////////////////////////////
  // パッケージ内部の情報
  // 読み出しのみ可能
  //////////////////////////////////////////////////////////////////////

  // 全てのノード数
  size_t mNodeNum;

  // ゴミ（誰からも参照されていない）ノード数
  size_t mGarbageNum;

  // 使用メモリ量
  size_t mUsedMem;

  // GCの起こった回数
  size_t mGcCount;


  //////////////////////////////////////////////////////////////////////
  // 変数に関連した情報を格納しておくエリア
  //////////////////////////////////////////////////////////////////////

  // レベルをキーにして変数のポインタを格納している配列
  // 必要に応じて拡張される．
  Var** mVarTable;

  // mVarTable 用に確保されたサイズ(単位はエントリ数)
  tVarSize mVarTableSize;

  // 確保された変数の数(<= mVarTableSize)
  tVarSize mVarNum;

  // 変数番号をキーにして変数のポインタを格納しているハッシュ表
  Var** mVarHashTable;


  //////////////////////////////////////////////////////////////////////
  // 節点テーブル関係のメンバ
  //////////////////////////////////////////////////////////////////////

  // テーブルサイズ
  size_t mTableSize;

  // テーブルサイズ - 1
  size_t mTableSize_1;

  // ノード数がこの数を越えたらテーブルサイズを拡張する．
  size_t mNextLimit;

  // テーブル本体
  Node** mNodeTable;


  //////////////////////////////////////////////////////////////////////
  // 演算結果テーブル
  //////////////////////////////////////////////////////////////////////

  // この BddMgr に登録されているテーブルの先頭
  CompTbl* mTblTop;

  CompTbl2* mAndTable;
  CompTbl2* mXorTable;
  CompTbl2* mIntTable;

  CompTbl3* mIteTable;
  CompTbl1* mCmpTable;
  CompTbl2* mCofacTable;
  CompTbl1* mXcofactorTable;

  CompTbl1* mSmTable;
  CompTbl2* mAeTable;
  CompTbl1* mCubedivTable;

  CompTbl2* mMinsupTable;
  IsopTbl* mIsopTable;
  IsopTbl* mPcTable;

  CompTbl2* mCsTable;
  CompTbl3* mCs1Table;
  CompTbl2* mCs2Table;


  //////////////////////////////////////////////////////////////////////
  // メモリブロック管理用のメンバ
  //////////////////////////////////////////////////////////////////////

  // フリーな節点リストの先頭
  Node* mFreeTop;

  // フリーな節点数
  size_t mFreeNum;

  // 今までに確保したメモリブロックの先頭
  Node* mTopBlk;

  // 現在使用中のブロック
  Node* mCurBlk;

  // mCurBlk の何番目まで使用しているかを示すインデックス
  size_t mCurIdx;


  //////////////////////////////////////////////////////////////////////
  // garbage collection 用の制御用変数
  //////////////////////////////////////////////////////////////////////

  // GCの起動を制御する変数
  int mGcEnable;

  // ノード数がこの数を越えたら mGcEnable は常に true だと思う
  size_t mDangerousZone;

  // GC 前に sweep 処理を行うオブジェクトを管理するマネージャ
  EventBindMgr mSweepMgr;


  //////////////////////////////////////////////////////////////////////
  // 内部的に用いられる作業領域
  //////////////////////////////////////////////////////////////////////

  // サポート演算中で用いられる作業領域
  list<Var*> mVarSet;

  // dump/size で節点数を数えるための作業領域
  size_t mNum;

  // smooth 用変数の最大レベル
  tLevel mLastLevel;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// リンク数を増やし，もしロックされていなければロックする
// 多くの場合, lockall() が呼ばれることが少ないので inline にしている．
inline
void
BddMgrModern::activate(BddEdge vd)
{
  Node* vp = get_node(vd);
  if ( vp && vp->linkinc() == 1 ) {
    lockall(vp);
  }
}

// リンク数を減らし，他のリンクがなければロックを外す
// 多くの場合, unlockall() が呼ばれることが少ないので inline にしている．
inline
void
BddMgrModern::deactivate(BddEdge vd)
{
  Node* vp = get_node(vd);
  if ( vp && vp->linkdec() == 0 ) {
    unlockall(vp);
  }
}

// 枝の値からそれが指しているノードのポインタをとり出す
inline
BmmNode*
BddMgrModern::get_node(BddEdge e)
{
  return reinterpret_cast<Node*>(e.get_ptr());
}

// ノードのポインタと極性から枝の情報を作り出す
inline
BddEdge
BddMgrModern::combine(Node* n,
		      tPol p)
{
  return BddEdge(n, p);
}

// vdの指すノードのマークを調べ，マークされていればtrueを返す．
// 枝に極性がなければマークは1種類でいいが，極性があるので，
// 肯定の枝から指された場合の p-mark と否定の枝から指された場
// 合の n-mark の2種類があるので，枝に応じて切替える．
inline
bool
BddMgrModern::mark(BddEdge vd)
{
  Node* vp = get_node(vd);
  return ( vd.pol() == kPolPosi ) ? vp->pmark() : vp->nmark();
}

// vdの指すノードにマークをつける
inline
void
BddMgrModern::setmark(BddEdge vd)
{
  Node* vp = get_node(vd);
  if ( vd.pol() == kPolPosi ) {
    vp->pmark(1);
  }
  else {
    vp->nmark(1);
  }
}

// idx が top に等しいときには e の子供を e_0, e_1 にセットする．
// 等しくなければ e をセットする．
inline
void
BddMgrModern::split1(tLevel top,
		     tLevel level,
		     BddEdge e,
		     const Node* vp,
		     tPol pol,
		     BddEdge& e_0,
		     BddEdge& e_1)
{
  if ( level == top ) {
    e_0 = vp->edge0(pol);
    e_1 = vp->edge1(pol);
  }
  else {
    e_0 = e_1 = e;
  }
}

// f と g のノードの子供のノードとレベルを求める．
inline
BmmVar*
BddMgrModern::split(BddEdge f,
		    BddEdge g,
		    BddEdge& f_0,
		    BddEdge& f_1,
		    BddEdge& g_0,
		    BddEdge& g_1)
{
  Node* f_vp = get_node(f);
  Node * g_vp = get_node(g);
  tPol f_pol = f.pol();
  tPol g_pol = g.pol();
  Var* f_var = f_vp->var();
  Var* g_var = g_vp->var();
  tLevel f_level = f_var->level();
  tLevel g_level = g_var->level();
  tLevel level = f_level;
  Var* var = f_var;
  if ( g_level < level ) {
    level = g_level;
    var = g_var;
  }
  split1(level, f_level, f, f_vp, f_pol, f_0, f_1);
  split1(level, g_level, g, g_vp, g_pol, g_0, g_1);
  return var;
}

// 節点テーブルの拡張を制御するパラメータを得る．
inline
double
BddMgrModern::nt_load_limit() const
{
  return mNtLoadLimit;
}

END_NAMESPACE_YM_BDD

#endif // LIBYM_LOGIC_BDD_BMC_BDDMGRMODERN_H
