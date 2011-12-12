
/// @file base_test.cc
/// @brief BDD パッケージのテストその1 --- 基本関数 ---
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_logic/Bdd.h"
#include "ym_logic/BddMgr.h"
#include "ym_logic/BddVarSet.h"

#include "bddtest.h"



BEGIN_NAMESPACE_YM_BDD

// 変数リストの検査
bool
test_var_list(BddMgr& bddmgr,
	      ymuint n)
{
  list<VarId> vlist;
  ymuint nn = bddmgr.var_list(vlist);
  bool result = true;
  if ( nn != n ) {
    result = false;
  }
  else {
    ymuint i = 0;
    for (list<VarId>::const_iterator p = vlist.begin();
	 p != vlist.end(); ++ p) {
      if ( *p != VarId(i) ) {
	result = false;
	break;
      }
      ++ i;
    }
  }
  if ( !result ) {
    cout << "ERROR in var_list" << endl;
    for (list<VarId>::const_iterator p = vlist.begin();
	 p != vlist.end(); ++ p) {
      cout << " " << *p;
    }
    cout << endl;
  }
  return result;
}

// 定数ノードの検査
bool
test_const(BddMgr& bddmgr)
{
  bool stat = true;
  Bdd one = bddmgr.make_one();
  if ( !one.is_one() ) {
    print(cout, one);
    cout << "ERROR[const-1]: !one.is_one()" << endl;
    stat = false;
  }

  Bdd zero = bddmgr.make_zero();
  if ( !zero.is_zero() ) {
    print(cout, zero);
    cout << "ERROR[const-2]: !zero.is_zero()" << endl;
    stat = false;
  }

  Bdd err = bddmgr.make_error();
  if ( !err.is_error() ) {
    print(cout, err);
    cout << "ERROR[const-3]: !err.is_error()" << endl;
    stat = false;
  }

  Bdd ovf = bddmgr.make_overflow();
  if ( !ovf.is_overflow() ) {
    print(cout, ovf);
    cout << "ERROR[const-4]: !ovf.is_overflow()" << endl;
    stat = false;
  }

  return stat;
}

// リテラル関数の生成
bool
test_literal(BddMgr& bddmgr)
{
  bool stat = true;
  Bdd var0 = bddmgr.make_posiliteral(VarId(0));
  if ( !check_bddv(bddmgr, var0, "make_posiliteral(0)", "0|01") )
    stat = false;

  Bdd var0bar = bddmgr.make_negaliteral(VarId(0));
  if ( !check_bddv(bddmgr, var0bar, "make_negaliteral(0)", "0|10") )
    stat = false;

  Bdd var2 = bddmgr.make_literal(VarId(2), kPolPosi);
  if ( !check_bddv(bddmgr, var2, "make_literal(2, kPolPosi)", "2|01") )
    stat = false;

  Bdd var3bar = bddmgr.make_literal(VarId(3), kPolNega);
  if ( !check_bddv(bddmgr, var3bar, "make_literal(3, kPolNega)", "3|10") )
    stat = false;

  Bdd zero = bddmgr.make_zero();
  Bdd one = bddmgr.make_one();
  Bdd var4 = bddmgr.make_bdd(VarId(4), zero, one);
  if ( !check_bddv(bddmgr, var4, "make_bdd(4, zero, one)", "4|01") )
    stat = false;

  return stat;
}

// 単項演算
bool
test_unary(BddMgr& bddmgr)
{
  Bdd var0 = bddmgr.make_posiliteral(VarId(0));

  bool stat = true;

  // って否定しかない
  if ( !check_bddv(bddmgr, ~var0, "~var0", "0|10") )
    stat = false;

  // 否定つき代入
  Bdd bdd1 = var0;
  bdd1.negate();
  if ( !check_bddv(bddmgr, bdd1, "var0.negate()", "0|10") )
    stat = false;

  return stat;
}

// 簡単な二項演算の検査
bool
test_binary(BddMgr& bddmgr)
{
  Bdd var0 = bddmgr.make_posiliteral(VarId(0));
  Bdd var1 = bddmgr.make_posiliteral(VarId(1));

  bool stat = true;

  if ( !check_bddv(bddmgr, var0 & var1, "var0 & var1", "0,1|0001") )
    stat = false;

  if ( !check_bddv(bddmgr, var0 & ~var1, "var0 & ~var1", "0,1|0010") )
    stat = false;

  if ( !check_bddv(bddmgr, var0 & ~var1, "var0 & ~var1", "1,0|0100") )
    stat = false;

  if ( !check_bddv(bddmgr, var0 | var1, "var0 | var1", "0,1|0111") )
    stat = false;

  if ( !check_bddv(bddmgr, var0 ^ var1, "var0 ^ var1", "0,1|0110") )
    stat = false;

  Bdd bdd1 = var0;
  if ( !check_bddv(bddmgr, bdd1 &= var1, "var0 &= var1", "0,1|0001") )
    stat = false;

  Bdd bdd2 = var0;
  if ( !check_bddv(bddmgr, bdd2 |= var1, "var0 |= var1", "0,1|0111") )
    stat = false;

  Bdd bdd3 = var0;
  if ( !check_bddv(bddmgr, bdd3 ^= var1, "var0 ^= var1", "0,1|0110") )
    stat = false;

  return stat;
}

// LogExpr や文字列からBDDを作る関数の検査
bool
test_make_bdd(BddMgr& bddmgr)
{
  bool stat = true;

  LogExpr expr1 = LogExpr::make_posiliteral(VarId(0)) &
    LogExpr::make_negaliteral(VarId(1)) |
    LogExpr::make_posiliteral(VarId(2)) &
    LogExpr::make_posiliteral(VarId(1));
  Bdd bdd1 = bddmgr.expr_to_bdd(expr1);
  if ( !check_bddv(bddmgr, bdd1, "0 & ~1 | 2 & 1", "0, 1, 2|00011101" ) ) {
    stat = false;
  }

  VarVarMap vvmap;
  vvmap.insert(make_pair(VarId(0), 1));
  vvmap.insert(make_pair(VarId(1), 2));
  vvmap.insert(make_pair(VarId(2), 0));
  Bdd bdd3 = bddmgr.expr_to_bdd(expr1, vvmap);
  if ( !check_bddv(bddmgr, bdd3, "(0 & ~1 | 2 & 1)(0->1, 1->2, 2->0)",
		  "0, 1, 2|00100111") ) {
    stat = false;
  }

  VarVarMap vvmap2;
  vvmap2.insert(make_pair(VarId(0), 1));
  vvmap2.insert(make_pair(VarId(1), 0));
  Bdd bdd5 = bddmgr.expr_to_bdd(expr1, vvmap2);
  if ( !check_bddv(bddmgr, bdd5, "(0 & ~1 | 2 & 1)(0->1, 1->0)",
		  "0, 1, 2|00110101") ) {
    stat = false;
  }

  return stat;
}

// ite_op のテスト
bool
test_ite(BddMgr& bddmgr)
{
  bool stat = true;

  if ( !check_ite(bddmgr, "0", "1", "2") ) {
    stat = false;
  }
  if ( !check_ite(bddmgr, "1 & ~3", "1 | 2 & 3", "2 & 3") ) {
    stat = false;
  }
  return stat;;
}

// compose のテスト
bool
test_compose(BddMgr& bddmgr)
{
  bool stat = true;

  Bdd bdd1 = str2bdd(bddmgr, "0 & 1 & 2 & 3");
  Bdd bdd2 = str2bdd(bddmgr, "~4 | 5");

  Bdd bdd4 = bdd1.compose(VarId(0), bdd2);
  if ( !check_bdde(bddmgr, bdd4, "0 & 1 & 2 & 3 compose(0, ~4 | 5)",
		   "1 & 2 & 3 & (~4 | 5)") ) {
    stat = false;
  }

  return true;;
}

// (a|e)smooth のテスト
bool
test_smooth(BddMgr& bddmgr)
{
  bool stat = true;

  Bdd bdd1 = str2bdd(bddmgr, "0 & 1 | ~0 & 2");
  VarList vl;
  vl.push_back(VarId(0));
  BddVarSet vs1(bddmgr, vl);
  Bdd bdd2 = bdd1.esmooth(vs1);
  if ( !check_bdde(bddmgr, bdd2, "0 & 1 | ~0 & 2 esmooth(0)", "1 | 2") ) {
    stat = false;
  }
  Bdd bdd3 = bdd1.asmooth(vs1);
  if ( !check_bdde(bddmgr, bdd3, "0 & 1 | ~0 & 2 asmooth(0)", "1 & 2") ) {
    stat = false;
  }

  return true;;
}

// && 演算子のテスト
bool
test_intersect(BddMgr& bddmgr)
{
  bool stat = true;

  const char* str1 = "0 | 1";
  const char* str2 = "0 & 1";

  Bdd bdd1 = str2bdd(bddmgr, "0 & 1 | 2 & 3");
  Bdd bdd2 = str2bdd(bddmgr, str1);
  if ( !(bdd1 && bdd2) ) {
    print(cout, bdd1);
    print(cout, bdd2);
    cout << "ERROR[44]: bdd1 && bdd2 failed" << endl;
    stat = false;
  }

  Bdd bdd3 = str2bdd(bddmgr, str2);
  if ( !(bdd1 >= bdd3) ) {
    print(cout, bdd1);
    print(cout, bdd3);
    cout << "ERROR[45]: bdd1 >= bdd3 failed" << endl;
    stat = false;
  }

  return stat;;
}

//  / 演算子のテスト
bool
test_cofactor(BddMgr& bddmgr)
{
  bool stat = true;

  const char* str = "0 & 2";

  Bdd bdd1 = str2bdd(bddmgr, "0 & 1 | 2 & 3");
  Bdd bdd2 = str2bdd(bddmgr, str);
  Bdd bdd3 = bdd1 / bdd2;
  if ( !check_bdde(bddmgr, bdd3, "0 & 1 | 1 & 2 / 0 & 2", "1 | 3") ) {
    stat = false;
  }
  return stat;
}

// and_exist演算
bool
test_and_exist(BddMgr& bddmgr)
{
  bool stat = true;

  const char* str = "0 | 2";
  VarList vl;
  vl.push_back(VarId(0));

  Bdd bdd1 = str2bdd(bddmgr, "0 & 1 | ~0 & 2");
  Bdd bdd2 = str2bdd(bddmgr, str);
  BddVarSet svars1(bddmgr, vl);
  Bdd bdd3 = and_exist(bdd1, bdd2, svars1);
  if ( !check_bdde(bddmgr, bdd3, "and_exist(bdd1, bdd2, svars1)", "1 | 2") ) {
    stat = false;
  }

  return stat;;
}

// isop, prime_cover, minimal_support のテスト
bool
test_isop(BddMgr& bddmgr)
{
  bool stat = true;

  const char* str = "0 & 2 | 1 & 3 | ~1 & ~3 | ~0 & ~2 & ~3";
  Bdd bdd1 = str2bdd(bddmgr, "0 & 1 & 2 | ~0 & 1 & 3 | ~1 & ~2 & ~3");

  Bdd bdd2 = str2bdd(bddmgr, str);
  LogExpr cover1;
  Bdd bdd3 = isop(bdd1, bdd2, cover1);
  if ( !check_bdde(bddmgr, bdd3, "isop(bdd1, bdd2, cover1)",
		   "0 & 2 | 1 & 3 | ~1 & ~3") ) {
    stat = false;
  }

  LogExpr ans_cover1 = prime_cover(bdd1, bdd2);
  Bdd bdd6 = bddmgr.expr_to_bdd(ans_cover1);
  if ( !check_bdde(bddmgr, bdd6, "prime_cover(bdd1, bdd2)",
		   "0 & 2 | 1 & 3 | ~1 & ~3") ) {
    stat = false;
  }

  Bdd bdd8 = minimal_support(bdd1, bdd2);
  if ( !check_bdde(bddmgr, bdd8, "minimal_support(bdd1, bdd2)", "0 & 1 & 2 & 3") ) {
    stat = false;
  }

  return stat;
}

// support のテスト
bool
test_support(BddMgr& bddmgr)
{
  bool stat = true;

  if ( !check_support(bddmgr, "0") ) {
    stat = false;
  }
  if ( !check_support(bddmgr, "0 | 1") ) {
    stat = false;
  }
  if ( !check_support(bddmgr, "0 & 1 | ~0 & 3") ) {
    stat = false;
  }

  return stat;
}

// test_scc のサブルーティン
bool
check_scc(BddMgr& bddmgr,
	  string expr_str,
	  const char* scc_str)
{
  Bdd bdd = str2bdd(bddmgr, expr_str.c_str());
  Bdd scc = bdd.SCC();
  string tmp = "SCC(" + expr_str + ")";

  return check_bdde(bddmgr, scc, tmp.c_str(), scc_str);
}

// SCC のテスト
bool
test_scc(BddMgr& bddmgr)
{
  bool stat = true;

  if ( !check_scc(bddmgr, "0 & 1 & ~2", "0 & 1 & ~2") ) {
    stat = false;
  }
  if ( !check_scc(bddmgr, "0 & 1 | ~0 & ~2", "one") ) {
    stat = false;
  }
  if ( !check_scc(bddmgr, "(0 ^ 1) & 2", "2") ) {
    stat = false;
  }
  if ( !check_scc(bddmgr, "0 & 1 & ~2 | ~0 & 1 & 2", "1") ) {
    stat = false;
  }
  if ( !check_scc(bddmgr, "0 | 1 | 2", "one") ) {
    stat = false;
  }

  return stat;
}

// test_symmetry のサブルーティン
bool
check_sym(BddMgr& bddmgr,
	  string expr_str)
{
  bool stat = true;

  Bdd bdd = str2bdd(bddmgr, expr_str.c_str());
  VarVector sup;
  ymuint ni = support(bdd, sup);
  if ( ni < 2 ) {
    // 無意味
    return stat;
  }

  for (ymuint i = 0; i < ni - 1; ++ i) {
    VarId pos1 = sup[i];
    Bdd bdd_0 = bdd.cofactor(pos1, kPolNega);
    Bdd bdd_1 = bdd.cofactor(pos1, kPolPosi);
    for (ymuint j = i + 1; j < ni; ++ j) {
      VarId pos2 = sup[j];
      Bdd bdd_01 = bdd_0.cofactor(pos2, kPolPosi);
      Bdd bdd_10 = bdd_1.cofactor(pos2, kPolNega);
      bool expected_result1 = (bdd_01 == bdd_10);
      if ( bdd.check_symmetry(pos1, pos2, kPolPosi) != expected_result1 ) {
	cout << "ERROR[test_symmetry(positive)]" << endl;
	print(cout, bdd);
	cout << "pos1 : " << pos1 << ", pos2 : " << pos2 << endl;
	cout << "expected result = ";
	if ( expected_result1 ) {
	  cout << "true" << endl;
	}
	else {
	  cout << "false" << endl;
	}
	stat = false;
      }
      Bdd bdd_00 = bdd_0.cofactor(pos2, kPolNega);
      Bdd bdd_11 = bdd_1.cofactor(pos2, kPolPosi);
      bool expected_result2 = (bdd_00 == bdd_11);
      if ( bdd.check_symmetry(pos1, pos2, kPolNega) != expected_result2 ) {
	cout << "ERROR[test_invsymmetry(negative)]" << endl;
	print(cout, bdd);
	cout << "pos1 : " << pos1 << ", pos2 : " << pos2 << endl;
	cout << "expected result = ";
	if ( expected_result2 ) {
	  cout << "true" << endl;
	}
	else {
	  cout << "false" << endl;
	}
	stat = false;
      }
    }
  }
  return stat;
}

// check_symmetry のテスト
bool
test_symmetry(BddMgr& bddmgr)
{
  bool stat = true;

  if ( !check_sym(bddmgr, "0 & 2 | ~0 & 1 & ~2") ) {
    stat = false;
  }
  if ( !check_sym(bddmgr, "0 & 1 | ~0 & ~1 & 2") ) {
    stat = false;
  }
  if ( !check_sym(bddmgr, "0 & 2 | 0 & ~1 | ~0 & 1 | ~0 & ~1 & ~2") ) {
    stat = false;
  }
  if ( !check_sym(bddmgr, "~0 & 1 & ~2 | 0 & ~1 | 0 & 2") ) {
    stat = false;
  }
  if ( !check_sym(bddmgr, "~0 & ~1 & 2 | 0 & 1 & ~2") ) {
    stat = false;
  }
  if ( !check_sym(bddmgr, "~0 & 1 & 2 | 0 & ~1 & ~2") ) {
    stat = false;
  }
  if ( !check_sym(bddmgr, "0 & 2 | ~0 & 1 & ~2") ) {
    stat = false;
  }

  return stat;
}

// minterm_count のテスト
bool
test_minterm_count(BddMgr& bddmgr)
{
  bool stat = true;

  Bdd bdd = str2bdd(bddmgr, "0 & 2 | 1 & 3 | ~1 & ~3");
  mpz_class mc = bdd.minterm_count(4);
  if ( mc != 10 ) {
    cout << "minterm_count(0 & 2 | 1 & 3 | ~1 & ~3) != 10" << endl;
    stat = false;
  }

  bdd = bddmgr.make_one();
  for (ymuint i = 0; i < 100; ++ i) {
    Bdd bdd1 = bddmgr.make_posiliteral(VarId(i));
    bdd &= bdd1;
  }
  mc = bdd.minterm_count(100);
  if ( mc != 1 ) {
    cout << "mc != 1" << endl;
    cout << "mc = " << mc.get_str() << endl;
    stat = false;
  }
  Bdd bdd2 = ~bdd;
  mpz_class mc2 = bdd2.minterm_count(100);
  if ( mc2 != (mpz_class(1) << 100) - mpz_class(1) ) {
    cout << "mc != 2^100 - 1" << endl;
    stat = false;
  }

  return stat;
}

// dump/restore のテスト
bool
test_dump(BddMgr& bddmgr)
{
  bool stat = true;

  Bdd bdd = str2bdd(bddmgr, "0 & 2 | 1 & 3 | ~1 & ~3");
  const char* fn = "/tmp/magus_bdd_base_test";
  {
    ofstream ofs(fn);
    if ( !ofs ) {
      cout << "cannot open output file: " << fn << endl;
      stat = false;
    }
    BinOStream bos(ofs);
    dump(bos, bdd);
  }
  Bdd bdd2;
  {
    ifstream ifs(fn);
    if ( !ifs ) {
      cout << "cannont open input file: " << fn << endl;
      stat = false;
    }
    BinIStream bis(ifs);
    bdd2 = bddmgr.restore(bis);
  }
  if ( bdd != bdd2 ) {
    cout << "ERROR[test_dump]" << endl;
    print(cout, bdd);
    print(cout, bdd2);
    stat = false;
  }

  return stat;
}

bool
test(BddMgr& bddmgr)
{
  bool stat = true;

  for (ymuint i = 0; i < 10; ++ i) {
    bddmgr.new_var(VarId(i));
  }

  if ( !test_var_list(bddmgr, 10) )
    stat = false;

  if ( !test_const(bddmgr) )
    stat = false;

  if ( !test_unary(bddmgr) )
    stat = false;

  if ( !test_binary(bddmgr) )
    stat = false;

  if ( !test_make_bdd(bddmgr) )
    stat = false;

  if ( !test_ite(bddmgr) )
    stat = false;

  if ( !test_compose(bddmgr) )
    stat = false;

  if ( !test_smooth(bddmgr) )
    stat = false;

  if ( !test_intersect(bddmgr) )
    stat = false;

  if ( !test_cofactor(bddmgr) )
    stat = false;

  if ( !test_and_exist(bddmgr) )
    stat = false;

  if ( !test_isop(bddmgr) )
    stat = false;

  if ( !test_support(bddmgr) )
    stat = false;

  if ( !test_scc(bddmgr) )
    stat = false;

  if ( !test_minterm_count(bddmgr) )
    stat = false;

  if ( !test_symmetry(bddmgr) )
    stat = false;

  if ( !test_dump(bddmgr) )
    stat = false;

  return stat;
}

END_NAMESPACE_YM_BDD


int
main(int argc,
     char** argv)
{
  using namespace std;
  using nsYm::AssertError;
  using nsYm::BddMgr;

#if !defined(YM_DEBUG)
  try {
#endif
    BddMgr bddmgr1("bmc", "classic bddmgr");
    if ( !test(bddmgr1) ) {
      return 1;
    }
    BddMgr bddmgr2("bmm", "fixed order bddmgr");
    if ( !test(bddmgr2) ) {
      return 2;
    }
    BddMgr bddmgr3("bmm", "reorder bddmgr", "reorder");
    if ( !test(bddmgr3) ) {
      return 3;
    }
#if !defined(YM_DEBUG)
  }
  catch ( AssertError a ) {
    cerr << a << endl;
    return 255;
  }
#endif

  return 0;
}
