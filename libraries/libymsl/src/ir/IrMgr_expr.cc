
/// @file IrMgr.cc
/// @brief IrMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "IrMgr.h"

#include "AstExpr.h"
#include "AstSymbol.h"

#include "Function.h"
#include "Var.h"
#include "Scope.h"
#include "Type.h"
#include "IrHandle.h"
#include "IrNode.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
// クラス IrMgr
//////////////////////////////////////////////////////////////////////

// @brief 右辺式の実体化を行う．
// @param[in] ast_expr 式を表す構文木
// @param[in] scope 現在のスコープ
IrNode*
IrMgr::elab_expr(const AstExpr* ast_expr,
		 Scope* scope)
{
  IrNode* node = NULL;
  switch ( ast_expr->expr_type() ) {
  case AstExpr::kTrue:
    return new_ConstNode(new_True());

  case AstExpr::kFalse:
    return new_ConstNode(new_False());

  case AstExpr::kIntConst:
    return new_ConstNode(new_IntConst(ast_expr->int_val()));

  case AstExpr::kFloatConst:
    return new_ConstNode(new_FloatConst(ast_expr->float_val()));

  case AstExpr::kStringConst:
    return new_ConstNode(new_StringConst(ast_expr->string_val()));

  case AstExpr::kSymbolExpr:
  case AstExpr::kArrayRef:
  case AstExpr::kMemberRef:
    {
      IrHandle* h = elab_primary(ast_expr, scope);
      if ( h == NULL ) {
	return NULL;
      }
      return new_Load(h);
    }

  case AstExpr::kFuncCall:
    {
      const AstExpr* func_expr = ast_expr->func();
      ymuint n = ast_expr->arglist_num();
      vector<IrNode*> arglist(n);
      for (ymuint i = 0; i < n; ++ i) {
	IrNode* arg = elab_expr(ast_expr->arglist_elem(i), scope);
	// arg->type() と func->type()->function_input_type(i) をチェック
	// 必要に応じてキャストノードを挿入する．
	arglist[i] = arg;
      }
      node = new_FuncCall(NULL, arglist);
      mFuncCallList.push_back(FuncCallStub(func_expr, scope, node));
      return node;
    }
    break;

  case AstExpr::kUniOp:
    {
      IrNode* op0 = elab_expr(ast_expr->operand(0), scope);
      if ( op0 == NULL ) {
	return NULL;
      }
      OpCode opcode = ast_expr->opcode();
      const Type* op0_type = op0->value_type();
      const Type* op0_rtype;
      const Type* type = mTypeMgr.calc_type1(opcode, op0_type, op0_rtype);
      if ( type == NULL ) {
	// type mismatch
	return NULL;
      }

      if ( op0_type != op0_rtype ) {
	// キャストノードの挿入
	// 結果を op0 に代入
      }

      return new_UniOp(opcode, type, op0);
    }

  case AstExpr::kBinOp:
    {
      IrNode* op0 = elab_expr(ast_expr->operand(0), scope);
      if ( op0 == NULL ) {
	return NULL;
      }
      IrNode* op1 = elab_expr(ast_expr->operand(1), scope);
      if ( op1 == NULL ) {
	return NULL;
      }
      OpCode opcode = ast_expr->opcode();
      const Type* op0_type = op0->value_type();
      const Type* op1_type = op1->value_type();
      const Type* op0_rtype;
      const Type* op1_rtype;
      const Type* type = mTypeMgr.calc_type2(opcode, op0_type, op1_type, op0_rtype, op1_rtype);
      if ( type == NULL ) {
	// type mismatch
	cerr << "type mismatch" << endl;
	return NULL;
      }

      if ( op0_rtype != op0_type ) {
	// キャストノードの挿入
      }

      if ( op1_rtype != op1_type ) {
	// キャストノードの挿入
      }

      return new_BinOp(opcode, type, op0, op1);
    }

  case AstExpr::kTriOp:
    {
      IrNode* op0 = elab_expr(ast_expr->operand(0), scope);
      if ( op0 == NULL ) {
	return NULL;
      }
      IrNode* op1 = elab_expr(ast_expr->operand(1), scope);
      if ( op1 == NULL ) {
	return NULL;
      }
      IrNode* op2 = elab_expr(ast_expr->operand(2), scope);
      if ( op2 == NULL ) {
	return NULL;
      }
      OpCode opcode = ast_expr->opcode();
      const Type* op0_type = op0->value_type();
      const Type* op1_type = op1->value_type();
      const Type* op2_type = op2->value_type();
      const Type* op0_rtype;
      const Type* op1_rtype;
      const Type* op2_rtype;
      const Type* type = mTypeMgr.calc_type3(opcode, op0_type, op1_type, op2_type, op0_rtype, op1_rtype, op2_rtype);
      if ( type == NULL ) {
	// type mismatch
	return NULL;
      }

      if ( op0_rtype != op0_type ) {
	// キャストノードの挿入
      }

      if ( op1_rtype != op1_type ) {
	// キャストノードの挿入
      }

      if ( op2_rtype != op2_type ) {
	// キャストノードの挿入
      }

      return new_TriOp(opcode, type, op0, op1, op2);
    }
  }

  ASSERT_NOT_REACHED;
  return NULL;
}

// @brief プライマリ式の解析を行う．
// @param[in] ast_expr 式を表す構文木
// @param[in] scope 現在のスコープ
IrHandle*
IrMgr::elab_primary(const AstExpr* ast_expr,
		    Scope* scope)
{
  switch ( ast_expr->expr_type() ) {
  case AstExpr::kSymbolExpr:
    {
      const AstSymbol* symbol = ast_expr->symbol();
      IrHandle* h = scope->find(symbol->str_val());
      if ( h == NULL ) {
	// symbol not found
	return NULL;
      }
      return h;
    }

  case AstExpr::kArrayRef:
    {
      // 配列本体やインデックス値自体は右辺値
      IrHandle* h = elab_primary(ast_expr->body(), scope);
      if ( h == NULL ) {
	return NULL;
      }
      IrNode* base = new_Load(h);
      if ( base->value_type()->type_id() != kArrayType ) {
	// base is not an array
	return NULL;
      }

      IrNode* offset = elab_expr(ast_expr->index(), scope);
      if ( offset->value_type()->type_id() != kIntType ) {
	// offset is not an integer
	return NULL;
      }

      return new_ArrayRef(base, offset);
    }

  case AstExpr::kMemberRef:
    {
      const AstExpr* body = ast_expr->body();
      const AstSymbol* member_symbol = ast_expr->member();
      IrHandle* h = elab_primary(body, scope);
      switch ( h->handle_type() ) {
      case IrHandle::kScope:
	{
	  Scope* scope1 = h->scope();
	  IrHandle* h1 = scope1->find(member_symbol->str_val());
	  if ( h1 == NULL ) {
	    // member_symbol not found
	    return NULL;
	  }
	  return h1;
	}

      case IrHandle::kVar:
	{
	  const Var* var = h->var();
	  const Type* type = var->value_type();
	  ShString member_name = member_symbol->str_val();
	  // type のメンバに member_name があることを確認する．
	  IrNode* base = new_Load(h);
	  const Var* member;
	  return new_MemberRef(base, member);
	}

      default:
	ASSERT_NOT_REACHED;
	break;
      }
    }
    break;

  default:
    break;
  }

  ASSERT_NOT_REACHED;
  return NULL;
}

END_NAMESPACE_YM_YMSL