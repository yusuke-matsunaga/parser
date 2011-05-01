
/// @file libym_cell/mislib/MislibLex.cc
/// @brief MislibLex の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: MislibLex.cc 2507 2009-10-17 16:24:02Z matsunaga $
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "MislibLex.h"


BEGIN_NAMESPACE_YM_CELL_MISLIB

#include "mislib_grammer.h"


// コンストラクタ
MislibLex::MislibLex(MsgMgr& msg_mgr) :
  mMsgMgr(msg_mgr)
{
}

// デストラクタ
MislibLex::~MislibLex()
{
}

// @brief ファイルを開く
// @param[in] filename ファイル名
// @return 失敗したら false を返す．
bool
MislibLex::open_file(const string& filename)
{
  bool stat = mFileScanner.open_file(filename);
  if ( !stat ) {
    return false;
  }
  mFileDesc = mFdMgr.new_file_desc(filename);
  return true;
}

// トークンを一つとってくる．
int
MislibLex::read_token()
{
  int c;

  mCurString.clear();

 state1:
  c = mFileScanner.get();
  mFirstColumn = mFileScanner.cur_column();
  mLastColumn = mFirstColumn;
  if ( isalpha(c) || (c == '_') ) {
    accept(c);
    goto state2;
  }
  if ( isdigit(c) || (c == '-') ) {
    accept(c);
    goto state3;
  }
  switch (c) {
  case EOF: return EOF;
  case ' ':
  case '\t': goto state1;
  case '\n': goto state1;
  case '.': goto state4;
  case ';': return SEMI;
  case '=': return EQ;
  case '(': return LP;
  case ')': return RP;
  case '!': return NOT;
  case '*': return STAR;
  case '&': return STAR;
  case '+': return PLUS;
  case '|': return PLUS;
  case '^': return HAT;
  case '#': goto state6;
  case '\"': goto state7;
  }
  goto error;

  // 一文字目が[a-zA-Z_]の時
 state2:
  c = mFileScanner.get();
  if ( isalpha(c) || isdigit(c) || (c == '_') ) {
    accept(c);
    goto state2;
  }
  mFileScanner.unget();

  // 予約語（？）の検査
  // 数が少ないのでナイーブな方法をとっている．
  if ( mCurString == "GATE" ) {
    return GATE;
  }
  if ( mCurString == "PIN" ) {
    return PIN;
  }
  if ( mCurString == "NONINV" ) {
    return NONINV;
  }
  if ( mCurString == "INV" ) {
    return INV;
  }
  if ( mCurString == "UNKNOWN" ) {
    return UNKNOWN;
  }
  if ( mCurString == "CONST0" ) {
    return CONST0;
  }
  if ( mCurString == "CONST1" ) {
    return CONST1;
  }
  return STR;

  // 一文字目が[0-9]の時
 state3:
  c = mFileScanner.get();
  if ( isdigit(c) ) {
    accept(c);
    goto state3;
  }
  if ( c == '.' ) {
    accept(c);
    goto state5;
  }
  if ( c == 'E' || c == 'e' ) {
    accept(c);
    goto state8;
  }
  goto state_NUM;

  // 一文字目が"."の時
 state4:
  mCurString = ".";
  c = mFileScanner.get();
  if ( isdigit(c) ) {
    accept(c);
    goto state5;
  }
  goto error;

  // [0-9]*"."を読み終えたところ
 state5:
  c = mFileScanner.get();
  if ( isdigit(c) ) {
    accept(c);
    goto state5;
  }
  if ( c == 'E' || c == 'e' ) {
    accept(c);
    goto state8;
  }
  goto state_NUM;

  // [0-9]*"."[Ee]を読み終えたところ
 state8:
  c = mFileScanner.get();
  if ( c == '-' ) {
    accept(c);
    goto state9;
  }
  if ( isdigit(c) ) {
    accept(c);
    goto state9;
  }
  goto state_NUM;

  // [0-9]*"."[Ee]-?を読み終えたところ
 state9:
  c = mFileScanner.get();
  if ( isdigit(c) ) {
    accept(c);
    goto state9;
  }

 state_NUM:
  mFileScanner.unget();
  return NUM;

  // '#'があったら改行までループする．
 state6:
  c = mFileScanner.get();
  if ( c == '\n' ) {
    goto state1;
  }
  goto state6;

  // "があったら次の"までを強制的に文字列だと思う．
 state7:
  c = mFileScanner.get();
  if ( c == '\"' ) {
    return STR;
  }
  accept(c);
  goto state7;

 error:
  {
    ostringstream buf;
    buf << "Syntax error: illegal input (" << mCurString << ").";
    mMsgMgr.put_msg(__FILE__, __LINE__,
		    cur_loc(), kMsgError,
		    "MISLIB_PARSE", buf.str().c_str());
  }
  return ERROR;
}

// @brief 現在のトークンの位置情報を返す．
FileRegion
MislibLex::cur_loc() const
{
  return FileRegion(mFileDesc,
		    mFileScanner.cur_line(), mFirstColumn,
		    mFileScanner.cur_line(), mLastColumn);
}

// 直前の読み込みを確定させる．
void
MislibLex::accept(int c)
{
  mCurString.put_char(c);
  mLastColumn = mFileScanner.cur_column();
}

END_NAMESPACE_YM_CELL_MISLIB
