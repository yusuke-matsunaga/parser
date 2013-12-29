
/// @file IdMgr.cc
/// @brief IdMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#include "IdMgr.h"
#include "IdImpl.h"


BEGIN_NAMESPACE_YM_SMTLIBV2

BEGIN_NONAMESPACE

// インデックスが等しいかチェックする．
// 等しければ true を返す．
bool
check_index(SmtId* id,
	    const vector<ymuint32>& index_list)
{
  ymuint index_size = id->index_size();
  if ( index_list.size() != index_size ) {
    return false;
  }
  for (ymuint i = 0; i < index_size; ++ i) {
    if ( id->index(i) != index_list[i] ) {
      return false;
    }
  }
  return true;
}

// ハッシュ関数
ymuint
hash_func(const ShString& name,
	  const vector<ymuint32>& index_list)
{
  ymuint h = name.hash();
  for (vector<ymuint32>::const_iterator p = index_list.begin();
       p != index_list.end(); ++ p) {
    h = h * 127 + (*p);
  }
  return h;
}

// ハッシュ関数
ymuint
hash_func(SmtId* id)
{
  ymuint h = id->name().hash();
  for (ymuint i = 0; i < id->index_size(); ++ i) {
    h = h * 127 + id->index(i);
  }
  return h;
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス IdMgr
//
// よくあるハッシュ表の実装だが，面倒くさいのはハッシュ値の計算方法
// SmtId になる前は vector<ymuint32> の形でインデックスが与えられる．
// これに対してハッシュ表を拡大する時の再ハッシュ時にはインデックスは
// 配列に入っている．
// これらが同じハッシュ値を生成するように気をつける必要がある．
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] alloc メモリアロケータ
IdMgr::IdMgr(Alloc& alloc) :
  mAlloc(alloc)
{
  mNum = 0;
  mTableSize = 0;
  mHashTable = NULL;

  expand_table(1024);
}

// @brief デストラクタ
IdMgr::~IdMgr()
{
  delete [] mHashTable;
}


// @brief 識別子に変換する．
// @param[in] name 名前
// @param[in] index_list インデックスリスト
const SmtId*
IdMgr::make_id(const ShString& name,
	       const vector<ymuint32>& index_list)
{
  ymuint h = hash_func(name, index_list);
  ymuint idx = h % mTableSize;
  for (IdImpl* id = mHashTable[idx]; id != NULL; id = id->mLink) {
    if ( id->name() == name && check_index(id, index_list) ) {
      return id;
    }
  }

  if ( mNum >= mNextLimit ) {
    expand_table(mTableSize * 2);
    idx = h % mTableSize;
  }

  IdImpl* id = NULL;
  if ( index_list.empty() ) {
    void* p = mAlloc.get_memory(sizeof(IdImpl));
    id = new (p) IdImpl(name);
  }
  else {
    ymuint index_size = index_list.size();
    void* p = mAlloc.get_memory(sizeof(IndexId) + sizeof(ymuint32) * (index_size - 1));
    IndexId* id1 = new (p) IndexId(name, index_size);
    for (ymuint i = 0; i < index_size; ++ i) {
      id1->mIndexList[i] = index_list[i];
    }
    id = id1;
  }

  id->mId = mNum;
  ++ mNum;

  id->mLink = mHashTable[idx];
  mHashTable[idx] = id;

  return id;
}

// @brief ハッシュ表を拡大する．
// @param[in] req_size 新しいサイズ
void
IdMgr::expand_table(ymuint req_size)
{
  ymuint old_size = mTableSize;
  IdImpl** old_table = mHashTable;

  mTableSize = req_size;
  mHashTable = new IdImpl*[mTableSize];
  for (ymuint i = 0; i < mTableSize; ++ i) {
    mHashTable[i] = NULL;
  }
  mNextLimit = static_cast<ymuint32>(mTableSize * 1.8);

  if ( old_size > 0 ) {
    for (ymuint i = 0; i < old_size; ++ i) {
      for (IdImpl* id = old_table[i]; id != NULL; ) {
	IdImpl* tmp_id = id;
	id = id->mLink;

	ymuint h = hash_func(tmp_id) % mTableSize;
	tmp_id->mLink = mHashTable[h];
	mHashTable[h] = tmp_id;
      }
    }
    delete [] old_table;
  }
}

END_NAMESPACE_YM_SMTLIBV2