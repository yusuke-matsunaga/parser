#ifndef YM_CELL_CELLCLASS_H
#define YM_CELL_CELLCLASS_H

/// @file libym_cell/CellClass.h
/// @brief CellClass のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_cell/cell_nsdef.h"
#include "ym_logic/TvFunc.h"


BEGIN_NAMESPACE_YM_CELL

//////////////////////////////////////////////////////////////////////
/// @class CellClass CellClass.h "ym_cell/CellClass.h"
/// @brief NPN同値なセルグループの集合を表すクラス
//////////////////////////////////////////////////////////////////////
class CellClass
{
public:

  /// @brief コンストラクタ
  CellClass() { }

  /// @brief デストラクタ
  virtual
  ~CellClass() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 機能情報を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力ピン数の取得
  virtual
  ymuint
  input_num() const = 0;

  /// @brief 出力ピン数の取得
  virtual
  ymuint
  output_num() const = 0;

  /// @brief 入出力ピン数の取得
  virtual
  ymuint
  inout_num() const = 0;

  /// @brief 論理セルの場合に true を返す．
  virtual
  bool
  is_logic() const = 0;

  /// @brief トライステートセルの場合に true を返す．
  /// @note もちろん論理セルでもある．
  /// @note 複数出力のうち1つでもトライステートなら true を返す．
  virtual
  bool
  is_tristate() const = 0;

  /// @brief FFセルの場合に true を返す．
  virtual
  bool
  is_ff() const = 0;

  /// @brief ラッチセルの場合に true を返す．
  virtual
  bool
  is_latch() const = 0;

  /// @brief 順序セル(非FF/非ラッチ)の場合に true を返す．
  virtual
  bool
  is_seq() const = 0;

  /// @brief 出力の論理関数を返す．
  /// @param[in] pos 出力番号 ( 0 <= pos < output_num() )
  /// @note FF/ラッチの場合は状態変数の変数(Q, XQ)が2つ入力に加わる．
  virtual
  TvFunc
  logic_function(ymuint pos) const = 0;

  /// @brief 出力のトライステート条件関数を返す．
  /// @param[in] pos 出力番号 ( 0 <= pos < output_num() )
  /// @note トライステートでない場合には定数0関数を返す．
  virtual
  TvFunc
  tristate_function(ymuint pos) const = 0;

  /// @brief FFセルの場合に次状態関数を返す．
  /// @note それ以外の型の場合の返り値は不定
  virtual
  TvFunc
  next_state_function() const = 0;

  /// @brief FFセルの場合にクロックのアクティブエッジを表す関数を返す．
  /// @note それ以外の型の場合の返り値は不定
  virtual
  TvFunc
  clock_function() const = 0;

  /// @brief ラッチセルの場合にデータ入力関数を返す．
  /// @note それ以外の型の場合の返り値は不定
  virtual
  TvFunc
  data_in_function() const = 0;

  /// @brief ラッチセルの場合にイネーブル条件を表す関数を返す．
  /// @note それ以外の型の場合の返り値は不定
  virtual
  TvFunc
  enable_function() const = 0;

  /// @brief FFセル/ラッチセルの場合にクリア端子を持っていたら true を返す．
  virtual
  bool
  has_clear() const = 0;

  /// @brief FFセル/ラッチセルの場合にクリア条件を表す関数を返す．
  /// @note クリア端子がない場合の返り値は不定
  virtual
  TvFunc
  clear_function() const = 0;

  /// @brief FFセル/ラッチセルの場合にプリセット端子を持っていたら true を返す．
  virtual
  bool
  has_preset() const = 0;

  /// @brief FFセル/ラッチセルの場合にプリセット条件を表す関数を返す．
  /// @note プリセット端子がない場合の返り値は不定
  virtual
  TvFunc
  preset_function() const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // このクラスに属しているセルグループの情報を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief CellGroup の数を返す．
  virtual
  ymuint
  group_num() const = 0;

  /// @brief CellGroup を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < func_num() )
  virtual
  const CellGroup*
  cell_group(ymuint pos) const = 0;

};

END_NAMESPACE_YM_CELL

#endif // YM_CELL_CELLCLASS_H
