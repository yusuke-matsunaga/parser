#ifndef SMTTERMMGR_H
#define SMTTERMMGR_H

/// @file SmtTermMgr.h
/// @brief SmtTermMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#include "ym_smtlibv2/smtlibv2_nsdef.h"
#include "ym_utils/ShString.h"
#include "ym_utils/Alloc.h"

#include "ym_smtlibv2/SmtSortedVar.h"
#include "ym_smtlibv2/SmtVarBinding.h"


BEGIN_NAMESPACE_YM_SMTLIBV2

//////////////////////////////////////////////////////////////////////
/// @class SmtTermMgr SmtTermMgr.h "SmtTermMgr.h"
/// @brief SmtTerm に対する操作を行うクラス
//////////////////////////////////////////////////////////////////////
class SmtTermMgr
{
public:

  /// @brief コンストラクタ
  /// @param[in] alloc メモリアロケータ
  SmtTermMgr(Alloc& alloc);

  /// @brief デストラクタ
  ~SmtTermMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief <numeric> 型の term を作る．
  /// @param[in] val 値
  const SmtTerm*
  make_numeric(ymuint32 val);

  /// @brief <decimal> 型の term を作る．
  /// @param[in] val 値
  const SmtTerm*
  make_decimal(const ShString& val);

  /// @brief <hexadecimal> 型の term を作る．
  /// @param[in] val 値
  const SmtTerm*
  make_hexadecimal(const ShString& val);

  /// @brief <binary> 型の term を作る．
  /// @param[in] val 値
  const SmtTerm*
  make_binary(const ShString& val);

  /// @brief <string> 型の term を作る．
  /// @param[in] val 値
  const SmtTerm*
  make_string(const ShString& val);

  /// @brief <symbol> 型の term を作る．
  /// @param[in] val 値
  const SmtTerm*
  make_symbol(const ShString& val);

  /// @brief <keyword> 型の term を作る．
  /// @param[in] val 値
  const SmtTerm*
  make_keyword(const ShString& val);

  /// @brief <identifier> 型の term を作る．
  /// @param[in] id 識別子
  const SmtTerm*
  make_identifier(const SmtId* id);

  /// @brief <qualified identifier> 型の term を作る．
  /// @param[in] id 識別子
  /// @param[in] sort 型
  const SmtTerm*
  make_qual_identifier(const SmtId* id,
		       const SmtSort* sort);

  /// @brief function term を作る．
  /// @param[in] function 関数
  /// @param[in] input_list 入力のリスト
  const SmtTerm*
  make_fun(const SmtFun* function,
	   const vector<const SmtTerm*>& input_list);

  /// @brief let 文を作る．
  /// @param[in] var_binding 変数割り当てのリスト
  /// @param[in] body 本体
  const SmtTerm*
  make_let(const vector<SmtVarBinding>& var_binding,
	   const SmtTerm* body);

  /// @brief forall 文を作る．
  /// @param[in] var_list 変数リスト
  /// @param[in] body 本体
  const SmtTerm*
  make_forall(const vector<SmtSortedVar>& var_list,
	      const SmtTerm* body);

  /// @brief exists 文を作る．
  /// @param[in] var_list 変数リスト
  /// @param[in] body 本体
  const SmtTerm*
  make_exists(const vector<SmtSortedVar>& var_list,
	      const SmtTerm* body);

  /// @brief attr 文を作る．
  /// @param[in] body 本体
  /// @param[in] attr_list 属性リスト
  const SmtTerm*
  make_attr(const SmtTerm* body,
	    const vector<SmtAttr>& attr_list);

  /// @brief list term を作る．
  /// @param[in] term_list 要素のリスト
  const SmtTerm*
  make_list(const vector<const SmtTerm*>& term_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // メモリアロケータ
  Alloc& mAlloc;

};

END_NAMESPACE_YM_SMTLIBV2

#endif // SMTTERMMGR_H
