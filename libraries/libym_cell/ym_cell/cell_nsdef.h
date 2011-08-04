#ifndef YM_CELL_CELL_NSDEF_H
#define YM_CELL_CELL_NSDEF_H

/// @file ym_cell/cell_nsdef.h
/// @brief libym_cell 用の名前空間の定義
///
/// @author Yusuke Matsunaga
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.

/// @defgroup CellGroup セルライブラリ
///
/// セルライブラリを扱うためのクラスライブラリ

#include "ymtools.h"


//////////////////////////////////////////////////////////////////////
// 名前空間を定義するためのマクロ
//////////////////////////////////////////////////////////////////////

/// @brief ym_cell の名前空間の開始
#define BEGIN_NAMESPACE_YM_CELL \
BEGIN_NAMESPACE_YM \
BEGIN_NAMESPACE(nsCell)

/// @brief ym_cell の名前空間の終了
#define END_NAMESPACE_YM_CELL \
END_NAMESPACE(nsCell) \
END_NAMESPACE_YM

/// @namespace nsYm::nsCell
/// @brief セルライブラリ関係のクラスが属する名前空間
///
/// ただし，主要なものは nsYm にインポートされている．
/// 実装用のローカルな要素だけがこの名前空間に存在する．


BEGIN_NAMESPACE_YM_CELL

//////////////////////////////////////////////////////////////////////
// クラスの前方参照
//////////////////////////////////////////////////////////////////////

class CellArea;
class CellTime;
class CellCapaciance;
class CellResistance;

class CellLibrary;
class Cell;
class CellPin;
class CellBusType;
class CellBus;
class CellBundle;
class CellTiming;
class CellLutTemplate;
class CellLut;

class CellMislibReader;
class CellDotlibReader;

END_NAMESPACE_YM_CELL

BEGIN_NAMESPACE_YM

using nsCell::CellArea;
using nsCell::CellTime;
using nsCell::CellCapaciance;
using nsCell::CellResistance;

using nsCell::CellLibrary;
using nsCell::Cell;
using nsCell::CellPin;
using nsCell::CellBusType;
using nsCell::CellBus;
using nsCell::CellBundle;
using nsCell::CellTiming;
using nsCell::CellLutTemplate;
using nsCell::CellLut;

using nsCell::CellMislibReader;
using nsCell::CellDotlibReader;

END_NAMESPACE_YM

#endif // YM_CELL_CELL_NSDEF_H
