
/// @file AstMgr.cc
/// @brief AstMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "AstMgr.h"

#include "YmslScanner.h"

#include "AstArrayRef.h"
#include "AstArrayType.h"
#include "AstAssignment.h"
#include "AstBinOp.h"
#include "AstBlockStmt.h"
#include "AstBoolConst.h"
#include "AstBreak.h"
#include "AstCaseItem.h"
#include "AstContinue.h"
#include "AstDoWhile.h"
#include "AstEnumConst.h"
#include "AstEnumDecl.h"
#include "AstExpr.h"
#include "AstExprStmt.h"
#include "AstFloatConst.h"
#include "AstFor.h"
#include "AstFuncCall.h"
#include "AstFuncDecl.h"
#include "AstGoto.h"
#include "AstIf.h"
#include "AstImport.h"
#include "AstIntConst.h"
#include "AstIteOp.h"
#include "AstLabel.h"
#include "AstList.h"
#include "AstMapType.h"
#include "AstModule.h"
#include "AstNamedType.h"
#include "AstParam.h"
#include "AstPrimary.h"
#include "AstPrimType.h"
#include "AstReturn.h"
#include "AstSetType.h"
#include "AstStringConst.h"
#include "AstSwitch.h"
#include "AstSymbol.h"
#include "AstToplevel.h"
#include "AstUniOp.h"
#include "AstVarDecl.h"
#include "AstWhile.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
// クラス AstMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] debug デバッグフラグ
AstMgr::AstMgr(bool debug)
{
  mScanner = NULL;
  mDebug = debug;
}

// @brief デストラクタ
AstMgr::~AstMgr()
{
}

// @brief ソースファイルを読み込む．
// @param[in] ido 入力データ
// @return 読み込みに成功したら true を返す．
bool
AstMgr::read_source(IDO& ido)
{
  int yyparser(AstMgr&);

  mScanner = new YmslScanner(ido);
  mToplevel = NULL;

  int stat = yyparse(*this);

  delete mScanner;
  mScanner = NULL;

  return (stat == 0);
}

// @brief トップレベルのASTを返す．
AstStatement*
AstMgr::toplevel() const
{
  return mToplevel;
}

// @brief 根のノードをセットする．
// @param[in] stmt_list ステートメントリスト
// @param[in] loc ファイル位置
void
AstMgr::set_root(AstStmtList* stmt_list,
		 const FileRegion& loc)
{
  ASSERT_COND ( mToplevel == NULL );
  void* p = mAlloc.get_memory(sizeof(AstToplevel));
  mToplevel = new (p) AstToplevel(stmt_list, loc);
}

// @brief yylex とのインターフェイス
// @param[out] lval 値を格納する変数
// @param[out] lloc 位置情報を格納する変数
// @return 読み込んだトークンの id を返す．
int
AstMgr::scan(YYSTYPE& lval,
	     FileRegion& lloc)
{
  int id = mScanner->read_token(lloc);

  switch ( id ) {
  case SYMBOL:
    lval.symbol_type = new_Symbol(ShString(mScanner->cur_string()), lloc);
    break;

  case STRING_VAL:
    lval.expr_type = new_StringConst(mScanner->cur_string(), lloc);
    break;

  case INT_VAL:
    lval.expr_type = new_IntConst(mScanner->cur_int(), lloc);
    break;

  case FLOAT_VAL:
    lval.expr_type = new_FloatConst(mScanner->cur_float(), lloc);
    break;

  default:
    break;
  }
  if ( mDebug ) {
    RsrvWordDic dic;
    switch ( id ) {
    case SYMBOL:     cout << "SYMBOL[" << mScanner->cur_string() << "]"; break;
    case INT_VAL:    cout << "INT[" << mScanner->cur_int() << "]"; break;
    case FLOAT_VAL:  cout << "FLOAT[" << mScanner->cur_float() << "]"; break;
    case STRING_VAL: cout << "STRING[" << mScanner->cur_string() << "]"; break;
    case EOF:        cout << "EOF"; break;
    default:         cout << dic.str(id); break;
    }
    cout << endl;
  }
  return id;
}

// @brief import 用のモジュール記述を作る
// @param[in] module モジュール名
// @param[in] alias エイリアス名
// @param[in] loc ファイル位置
AstModule*
AstMgr::new_Module(AstSymbol* module,
		   AstSymbol* alias,
		   const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstModule));
  return new (p) AstModule(module, alias, loc);
}

// @brief import 文を作る．
// @param[in] module_list モジュールのリスト
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_Import(AstModuleList* module_list,
		   const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstImport));
  return new (p) AstImport(module_list, loc);
}

// @brief enum 定義を作る．
// @param[in] name 型名
// @param[in] const_list 定数リスト
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_EnumDecl(AstSymbol* name,
		     AstEnumConstList* const_list,
		     const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstEnumDecl));
  return new (p) AstEnumDecl(name, const_list, loc);
}

// @brief enum 定数を作る．
// @param[in] name 定数名
// @param[in] expr 値を表す式
// @param[in] loc ファイル位置
AstEnumConst*
AstMgr::new_EnumConst(AstSymbol* name,
		      AstExpr* expr,
		      const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstEnumConst));
  return new (p) AstEnumConst(name, expr, loc);
}

// @brief 変数宣言を作る．
// @param[in] name 変数名
// @param[in] type 型
// @param[in] init_expr 初期化式
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_VarDecl(AstSymbol* name,
		    AstType* type,
		    AstExpr* init_expr,
		    const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstVarDecl));
  return new (p) AstVarDecl(name->str_val(), type, init_expr, loc);
}

// @brief パラメータ宣言を作る．
// @param[in] name 変数名
// @param[in] type 型
// @param[in] init_expr 初期化式
// @param[in] loc ファイル位置
AstParam*
AstMgr::new_Param(AstSymbol* name,
		  AstType* type,
		  AstExpr* init_expr,
		  const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstParam));
  return new (p) AstParam(name->str_val(), type, init_expr, loc);
}

// @brief 関数宣言を作る．
// @param[in] name 変数名
// @param[in] type 型
// @param[in] param_list パラメータリスト
// @param[in] stmt 本体の文
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_FuncDecl(AstSymbol* name,
		     AstType* type,
		     AstParamList* param_list,
		     AstStatement* stmt,
		     const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstFuncDecl));
  return  new (p) AstFuncDecl(name->str_val(), type, param_list, stmt, loc);
}

// @brief 代入文を作る．
// @param[in] stmt_type 文の種類
// @param[in] left 左辺
// @param[in] right 右辺
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_Assignment(StmtType stmt_type,
		       AstExpr* left,
		       AstExpr* right,
		       const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstAssignment));
  return new (p) AstAssignment(stmt_type, left, right, loc);
}

// @brief if 文を作る．
// @param[in] expr 条件式
// @param[in] then_stmt 条件が成り立った時実行される文のリスト
// @param[in] else_stmt 条件が成り立たなかった時実行される文
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_If(AstExpr* expr,
	       AstStatement* then_stmt,
	       AstStatement* else_stmt,
	       const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstIf));
  return new (p) AstIf(expr, then_stmt, else_stmt, loc);
}

// @brief for 文を作る．
// @param[in] init 初期化文
// @param[in] cond 条件式
// @param[in] next 増加文
// @param[in] stmt 本体の文
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_For(AstStatement* init,
		AstExpr* cond,
		AstStatement* next,
		AstStatement* stmt,
		const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstFor));
  return new (p) AstFor(init, cond, next, stmt, loc);
}

// @brief while 文を作る．
// @param[in] cond 条件式
// @param[in] stmt 本体の文
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_While(AstExpr* cond,
		  AstStatement* stmt,
		  const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstWhile));
  return new (p) AstWhile(cond, stmt, loc);
}

// @brief do-while 文を作る．
// @param[in] stmt 本体の文
// @param[in] cond 条件式
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_DoWhile(AstStatement* stmt,
		    AstExpr* cond,
		    const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstDoWhile));
  return new (p) AstDoWhile(stmt, cond, loc);
}

// @brief switch 文を作る．
// @param[in] expr 条件式
// @param[in] case_list caseリスト
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_Switch(AstExpr* expr,
		   AstCaseList* case_list,
		   const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstSwitch));
  return new (p) AstSwitch(expr, case_list, loc);
}

// @brief case-item を作る．
// @param[in] label ラベル
// @param[in] stmt 本体の文
// @param[in] loc ファイル位置
AstCaseItem*
AstMgr::new_CaseItem(AstExpr* label,
		     AstStatement* stmt,
		     const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstCaseItem));
  return new (p) AstCaseItem(label, stmt, loc);
}

// @brief goto 文を作る．
// @param[in] label ラベル
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_Goto(AstSymbol* label,
		 const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstGoto));
  return new (p) AstGoto(label, loc);
}

// @brief ラベルを作る．
// @param[in] label ラベル
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_Label(AstSymbol* label,
		  const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstLabel));
  return new (p) AstLabel(label, loc);
}

// @brief break 文を作る．
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_Break(const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstBreak));
  return new (p) AstBreak(loc);
}

// @brief continue 文を作る．
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_Continue(const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstContinue));
  return new (p) AstContinue(loc);
}

// @brief return 文を作る．
// @param[in] expr 値
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_Return(AstExpr* expr,
		   const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstReturn));
  return new (p) AstReturn(expr, loc);
}

// @brief ブロック文を作る．
// @param[in] stmt_list 本体の文
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_BlockStmt(AstStmtList* stmt_list,
		      const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstBlockStmt));
  return new (p) AstBlockStmt(stmt_list, loc);
}

// @brief 式文を作る．
// @param[in] expr 式
// @param[in] loc ファイル位置
AstStatement*
AstMgr::new_ExprStmt(AstExpr* expr,
		     const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstExprStmt));
  return new (p) AstExprStmt(expr, loc);
}

// @brief 単項演算式を作る．
// @param[in] op 演算子のタイプ
// @param[in] left オペランド
// @param[in] loc ファイル位置
AstExpr*
AstMgr::new_UniOp(ExprType op,
		  AstExpr* left,
		  const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstUniOp));
  return new (p) AstUniOp(op, left, loc);
}

// @brief 二項演算式を作る．
// @param[in] op 演算子のタイプ
// @param[in] left, right オペランド
AstExpr*
AstMgr::new_BinOp(ExprType op,
		  AstExpr* left,
		  AstExpr* right)
{
  void* p = mAlloc.get_memory(sizeof(AstBinOp));
  return new (p) AstBinOp(op, left, right);
}

// @brief ITE演算式を作る．
// @param[in] opr1, opr2, opr3 オペランド
AstExpr*
AstMgr::new_IteOp(AstExpr* opr1,
		  AstExpr* opr2,
		  AstExpr* opr3)
{
  void* p = mAlloc.get_memory(sizeof(AstIteOp));
  return new (p) AstIteOp(opr1, opr2, opr3);
}

// @brief 配列参照を作る．
// @param[in] id 配列名
// @param[in] index インデックス
// @param[in] loc ファイル位置
AstExpr*
AstMgr::new_ArrayRef(AstExpr* id,
		     AstExpr* index,
		     const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstArrayRef));
  return new (p) AstArrayRef(id, index, loc);
}

// @brief 関数呼び出しを作る．
// @param[in] id 関数名
// @param[in] expr_list 引数のリスト
// @param[in] loc ファイル位置
AstExpr*
AstMgr::new_FuncCall(AstExpr* id,
		     AstExprList* expr_list,
		     const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstFuncCall));
  return new (p) AstFuncCall(id, expr_list, loc);
}

// @brief true 定数式を作る．
// @param[in] loc ファイル位置
AstExpr*
AstMgr::new_TrueConst(const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstBoolConst));
  return new (p) AstBoolConst(kTrue, loc);
}

// @brief false 定数式を作る．
// @param[in] loc ファイル位置
AstExpr*
AstMgr::new_FalseConst(const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstBoolConst));
  return new (p) AstBoolConst(kFalse, loc);
}

// @brief 整数定数式を作る．
// @param[in] val 値
// @param[in] loc ファイル位置
AstExpr*
AstMgr::new_IntConst(int val,
		     const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstIntConst));
  return new (p) AstIntConst(val, loc);
}

// @brief 浮動小数点定数式を作る．
// @param[in] val 値
// @param[in] loc ファイル位置
AstExpr*
AstMgr::new_FloatConst(double val,
		       const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstFloatConst));
  return new (p) AstFloatConst(val, loc);
}

// @brief 文字列定数式を作る．
// @param[in] val 値
// @param[in] loc ファイル位置
AstExpr*
AstMgr::new_StringConst(const char* val,
			const FileRegion& loc)
{
  ymuint n = 0;
  if ( val != NULL ) {
    n = strlen(val);
  }
  void* q = mAlloc.get_memory(n + 1);
  char* dup_str = new (q) char[n + 1];
  for (ymuint i = 0; i < n; ++ i) {
    dup_str[i] = val[i];
  }
  dup_str[n] = '\0';

  void* p = mAlloc.get_memory(sizeof(AstStringConst));
  return new (p) AstStringConst(dup_str, loc);
}

// @brief プライマリを作る．
// @param[in] symbol_list 変数名
// @param[in] loc ファイル位置
AstExpr*
AstMgr::new_Primary(AstSymbolList* symbol_list,
		    const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstPrimary));
  return new (p) AstPrimary(symbol_list, loc);
}

// @brief プリミティブ型を作る．
// @param[in] type 型
// @param[in] loc ファイル位置
AstType*
AstMgr::new_PrimType(TypeId type,
		     const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstPrimType));
  return new (p) AstPrimType(type, loc);
}

// @brief 名前付きの型を作る．
// @param[in] type_name 型名
AstType*
AstMgr::new_NamedType(AstSymbol* type_name)
{
  void* p = mAlloc.get_memory(sizeof(AstNamedType));
  return new (p) AstNamedType(type_name);
}

// @brief array 型を作る．
// @param[in] elem_type 要素の型
// @param[in] loc ファイル位置
AstType*
AstMgr::new_ArrayType(AstType* elem_type,
		      const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstArrayType));
  return new (p) AstArrayType(elem_type, loc);
}

// @brief set 型を作る．
// @param[in] elem_type 要素の型
// @param[in] loc ファイル位置
AstType*
AstMgr::new_SetType(AstType* elem_type,
		    const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstSetType));
  return new (p) AstSetType(elem_type, loc);
}

// @brief map 型を作る．
// @param[in] key_type キーの型
// @param[in] elem_type 要素の型
// @param[in] loc ファイル位置
AstType*
AstMgr::new_MapType(AstType* key_type,
		    AstType* elem_type,
		    const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstMapType));
  return new (p) AstMapType(key_type, elem_type, loc);
}

// @brief シンボルを作る．
// @param[in] str シンボル名
// @param[in] loc ファイル位置
AstSymbol*
AstMgr::new_Symbol(ShString str,
		   const FileRegion& loc)
{
  void* p = mAlloc.get_memory(sizeof(AstSymbol));
  return new (p) AstSymbol(str, loc);
}

END_NAMESPACE_YM_YMSL