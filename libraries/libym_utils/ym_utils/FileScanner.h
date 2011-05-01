#ifndef YM_UTILS_FILESCANNER_H
#define YM_UTILS_FILESCANNER_H

/// @file ym_utils/FileScanner.h
/// @brief FileScanner のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: DotlibParser.h 2507 2009-10-17 16:24:02Z matsunaga $
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ymtools.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class FileScanner FileScanner.h "ym_utils/FileScanner.h"
/// @brief 位置情報のトラッキング付きのファイル入力
///
/// このクラスで行える処理は
/// - ファイルのオープン (open_file)
/// - ファイルのクローズ (close_file)
/// - 一文字読み出し     (get)
/// - 直前の読み出しの無効化 (unget)
///
/// で，直前の get() に対応するファイル上の位置を
/// cur_line, cur_column で返す．
//////////////////////////////////////////////////////////////////////
class FileScanner
{
public:

  /// @brief コンストラクタ
  FileScanner();

  /// @brief デストラクタ
  ~FileScanner();


public:

  /// @brief ファイルをオープンする．
  /// @param[in] filename ファイル名
  /// @return ファイルのオープンに失敗したら false を返す．
  bool
  open_file(const string& filename);

  /// @brief ファイルをクローズする．
  void
  close_file();

  /// @brief 一文字読み出す．
  int
  get();

  /// @brief 直前の get() を無かったことにする．
  void
  unget();

  /// @brief 現在の行番号を返す．
  int
  cur_line() const;

  /// @brief 現在のコラム位置を返す．
  int
  cur_column() const;

  /// @brief 最後の文字の行番号を返す．
  /// @note 通常は cur_line() と同一だが，
  /// unget() の直後は一つ前の行番号を返す．
  int
  last_line() const;

  /// @brief 最後の文字のコラム位置を返す．
  /// @note 通常は cur_column() と同一だが，
  /// unget() の直後は一つ前のコラム位置を返す．
  int
  last_column() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化を行う．
  void
  init();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // UNIX のファイル記述子
  int mFd;

  // バッファ
  char mBuff[4096];

  // バッファ中の読み出し位置
  ymuint32 mReadPos;

  // バッファの末尾
  ymuint32 mEndPos;

  // 直前の文字が \r の時に true となるフラグ
  bool mCR;

  // 現在の文字
  int mCurChar;

  // 現在の行番号
  ymuint32 mCurLine;

  // 現在のコラム位置
  ymuint32 mCurColumn;

  // 最後の文字の行番号
  ymuint32 mLastLine;

  // 最後の文字のコラム位置
  ymuint32 mLastColumn;

  // unget() 用の行番号
  ymuint32 mUngetLine;

  // unget() 用のコラム位置
  ymuint32 mUngetColumn;

  // 新しい文字を読み込む必要がある時 true となるフラグ
  bool mNeedUpdate;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// 現在の行番号を返す．
inline
int
FileScanner::cur_line() const
{
  return mCurLine;
}

// 現在のコラム位置を返す．
inline
int
FileScanner::cur_column() const
{
  return mCurColumn;
}

// @brief 最後の文字の行番号を返す．
inline
int
FileScanner::last_line() const
{
  return mLastLine;
}

// @brief 最後の文字のコラム位置を返す．
inline
int
FileScanner::last_column() const
{
  return mLastColumn;
}

END_NAMESPACE_YM

#endif // YM_UTILS_FILESCANNER_H