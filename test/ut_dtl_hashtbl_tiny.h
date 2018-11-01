#ifndef _UT_RX_DTL_HASHTBL_TINY_H_
#define _UT_RX_DTL_HASHTBL_TINY_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_tdd_tick.h"
#include "../rx_dtl_hashtbl_raw.h"
#include "../rx_dtl_hashtbl_tiny.h"

namespace rx_ut
{
    //-----------------------------------------------------
    const uint32_t tmp_hashtbl_tiny_msize=7;

    //-----------------------------------------------------
    //哈希集合基础测试
    template<class cntr_t>
    inline void raw_tinyhashset_base_loop1(rx_tdd_t &rt,const cntr_t& s)
    {
        for(typename cntr_t::iterator i= s.begin();i!=s.end();++i)
            rt.tdd_assert(*i!=0);
    }
    inline void raw_tinyhashset_base_1(rx_tdd_t &rt)
    {
        typedef rx::tiny_hashset_t<tmp_hashtbl_tiny_msize,int32_t> cntr_t;
        cntr_t s;
        rt.tdd_assert(s.capacity() == tmp_hashtbl_tiny_msize);
        rt.tdd_assert(s.size() == 0);
        rt.tdd_assert(s.stat().collision_count == 0);

        rt.tdd_assert(s.insert(1));
        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.stat().collision_count == 0);

        rt.tdd_assert(s.insert(1));
        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.stat().collision_count == 0);

        rt.tdd_assert(s.insert(2));
        rt.tdd_assert(s.size() == 2);
        rt.tdd_assert(s.stat().collision_count == 0);

        rt.tdd_assert(s.insert(3));
        rt.tdd_assert(s.size() == 3);
        rt.tdd_assert(s.stat().collision_count == 0);

        rt.tdd_assert(s.insert(4));
        rt.tdd_assert(s.size() == 4);
        rt.tdd_assert(s.stat().collision_count == 0);

        raw_tinyhashset_base_loop1(rt,s);

        rt.tdd_assert(s.find(4)&&s[4]);
        rt.tdd_assert(s.find(2)&&s[2]);
        rt.tdd_assert(s.erase(2));
        rt.tdd_assert(s.size() == 3);
        rt.tdd_assert(s.stat().collision_count == 0);
        rt.tdd_assert(!s.find(2)&&!s[2]);
    }

    //-----------------------------------------------------
    //哈希表基础测试
    template<class cntr_t>
    inline void raw_tinyhashtbl_base_loop1(rx_tdd_t &rt,const cntr_t& s)
    {
        for(typename cntr_t::iterator i= s.begin();i!=s.end();++i)
            rt.tdd_assert(*i!=0&&(i()+1)==*i);
    }

    inline void raw_tinyhashtbl_base_1(rx_tdd_t &rt)
    {
        typedef rx::tiny_hashtbl_t<tmp_hashtbl_tiny_msize,int,int> cntr_t;
        cntr_t s;
        rt.tdd_assert(s.capacity() == tmp_hashtbl_tiny_msize);
        rt.tdd_assert(s.size() == 0);
        rt.tdd_assert(s.stat().collision_count == 0);

        rt.tdd_assert(s.insert(1,2)!=s.end());
        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.stat().collision_count == 0);

        rt.tdd_assert(s.insert(1,2) != s.end());
        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.stat().collision_count == 0);

        rt.tdd_assert(s.insert(2,3) != s.end());
        rt.tdd_assert(s.size() == 2);
        rt.tdd_assert(s.stat().collision_count == 0);

        rt.tdd_assert(s.insert(3,4) != s.end());
        rt.tdd_assert(s.size() == 3);
        rt.tdd_assert(s.stat().collision_count == 0);

        rt.tdd_assert(s.insert(4,5) != s.end());
        rt.tdd_assert(s.size() == 4);
        rt.tdd_assert(s.stat().collision_count == 0);

        raw_tinyhashtbl_base_loop1(rt,s);

        rt.tdd_assert(s.find(4)!=s.end());
        rt.tdd_assert(s[4] != s.end());
        rt.tdd_assert(s.find(2) != s.end());
        rt.tdd_assert(s[2] != s.end());

        rt.tdd_assert(s.erase(2));
        rt.tdd_assert(s.size() == 3);
        rt.tdd_assert(s.stat().collision_count == 0);
        rt.tdd_assert(s.find(2) == s.end());
        rt.tdd_assert(s[2] == s.end());
    }

    //-----------------------------------------------------
    //哈希链表基础测试
    inline void raw_tinyhashlink_base_1(rx_tdd_t &rt)
    {
        typedef rx::tiny_hashlink_t<tmp_hashtbl_tiny_msize,int,int> cntr_t;
        cntr_t s;

        rt.tdd_assert(s.capacity() == tmp_hashtbl_tiny_msize);
        rt.tdd_assert(s.size() == 0);
        rt.tdd_assert(s.stat().collision_count == 0);
        rt.tdd_assert(s.begin()==s.end());

        rt.tdd_assert(s.insert(1,2) != s.end());
        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.stat().collision_count == 0);
        rt.tdd_assert(s.begin()()==1);
        rt.tdd_assert(s.rbegin()()==1);
        rt.tdd_assert(s.begin()!=s.end());

        rt.tdd_assert(s.insert(1,2) != s.end());
        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.stat().collision_count == 0);

        rt.tdd_assert(s.insert(2,3) != s.end());
        rt.tdd_assert(s.size() == 2);
        rt.tdd_assert(s.stat().collision_count == 0);
        rt.tdd_assert(s.begin()()==1);
        rt.tdd_assert(s.rbegin()()==2);
        rt.tdd_assert(s.begin()!=s.end());

        rt.tdd_assert(s.insert(3,4) != s.end());
        rt.tdd_assert(s.size() == 3);
        rt.tdd_assert(s.stat().collision_count == 0);
        rt.tdd_assert(s.begin()()==1);
        rt.tdd_assert(s.rbegin()()==3);
        rt.tdd_assert(s.begin()!=s.end());

        rt.tdd_assert(s.insert(4,5) != s.end());
        rt.tdd_assert(s.size() == 4);
        rt.tdd_assert(s.stat().collision_count == 0);
        rt.tdd_assert(s.begin()()==1);
        rt.tdd_assert(s.rbegin()()==4);
        rt.tdd_assert(s.begin()!=s.end());

        raw_tinyhashtbl_base_loop1(rt,s);

        rt.tdd_assert(s.find(4)!=s.end());
        rt.tdd_assert(s.find(2)!=s.end())
            ;
        rt.tdd_assert(s.erase(2));
        rt.tdd_assert(s.begin()()==1);
        rt.tdd_assert(s.rbegin()()==4);
        rt.tdd_assert(s.begin()!=s.end());

        rt.tdd_assert(s.size() == 3);
        rt.tdd_assert(s.stat().collision_count == 0);
        rt.tdd_assert(s.find(2)==s.end());

        rt.tdd_assert(s.erase(4));
        rt.tdd_assert(s.begin()()==1);
        rt.tdd_assert(s.rbegin()()==3);
        rt.tdd_assert(s.begin()!=s.end());

        rt.tdd_assert(s.size() == 2);
        rt.tdd_assert(s.stat().collision_count == 0);
        rt.tdd_assert(s.find(4)==s.end());

        rt.tdd_assert(s.erase(1));
        rt.tdd_assert(s.begin()()==3);
        rt.tdd_assert(s.rbegin()()==3);
        rt.tdd_assert(s.begin()!=s.end());

        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.stat().collision_count == 0);
        rt.tdd_assert(s.find(1)==s.end());

        rt.tdd_assert(s.erase(3));
        rt.tdd_assert(s.begin()==s.end());

        rt.tdd_assert(s.size() == 0);
        rt.tdd_assert(s.stat().collision_count == 0);
        rt.tdd_assert(s.find(3)==s.end());
    }
}
//---------------------------------------------------------
//验证测试整数key哈希表对应的哈希函数类型
class ut_htcmp_t0
{
public:
    //对节点比较函数进行覆盖,需要真正使用节点中的key字段与给定的k值进行比较
    template<class NVT, class KT>
    static bool equ(const NVT &n, const KT &k) { return n.key == k; }
};
class ut_htcmp_t1:public ut_htcmp_t0
{
public:
    static uint32_t hash(const uint32_t &k) { return (uint32_t)rx_hash_gold<0>(k); }
};
class ut_htcmp_t2:public ut_htcmp_t0
{
public:
    static uint32_t hash(const uint32_t &k) { return rx_hash_tomas(k); }
};
class ut_htcmp_t3:public ut_htcmp_t0
{
public:
    static uint32_t hash(const uint32_t &k) { return rx_hash_murmur3(k); }
};
class ut_htcmp_t4:public ut_htcmp_t0
{
public:
    static uint32_t hash(const uint32_t &k) { return rx_hash_mosquito(k); }
};
template<uint32_t nodes,uint32_t items,class cmpt>
void ut_hashtbl_ext_test_1(rx_tdd_t &rt)
{
    typedef rx::tiny_hashtbl_t<nodes,uint32_t,uint32_t,cmpt> htbl_t;
    htbl_t htbl;
    uint32_t ec=0;
    //tdd_tt(t,"ut_hashtbl_ext_test","1");
    for(uint32_t i=0;i<items;++i)
    {
        if (htbl.insert(i,i)==htbl.end())
            ++ec;
    }
    rt.tdd_assert(ec==0);
    //tdd_tt_hit(t,"ut_hashtbl_ext_test_1:nodes=%u,items=%u,colli=%u,ec=%u",nodes,items,htbl.stat().collision_count,ec);

    uint32_t loops = 7000;
    for (uint32_t l = 0; l < loops; ++l)
    {
        htbl.find(l+1);
    }
    //tdd_tt_hit(t, "ut_hashtbl_ext_test_1:nodes=%u,items=%u,loop=%u", nodes, items, loops);
}

//---------------------------------------------------------
//准备进行删除后的节点补位处理
class ut_htcmp_tra :public ut_htcmp_t0
{
public:
    static uint32_t hash(const uint32_t &k) { return k%7; }
};

inline void tinyhashtbl_removed_adj_1(rx_tdd_t &rt)
{
    const uint32_t nodes = 31;
    const uint32_t items = uint32_t(nodes*0.7);
    typedef rx::tiny_hashtbl_t<nodes, uint32_t, uint32_t, ut_htcmp_tra> htbl_t;
    htbl_t tbl;

    for (uint32_t i = 0; i<items; ++i)
        rt.tdd_assert(tbl.insert(i, i) != tbl.end());

    for (uint32_t i = 0; i < items; ++i)
    {
        htbl_t::iterator I = tbl[i];
        const htbl_t::node_t *node = tbl.at(I.pos());
        printf("key=%2d val=%2d pos=%2d flag=%2d step=%2d\n",I(),*I,I.pos(),node->flag,node->step);
    }
        


}
//---------------------------------------------------------

rx_tdd(hashtbl_tiny_base)
{
    tinyhashtbl_removed_adj_1(*this);

    rx_ut::raw_tinyhashset_base_1(*this);
    rx_ut::raw_tinyhashtbl_base_1(*this);
    rx_ut::raw_tinyhashlink_base_1(*this);

    ut_hashtbl_ext_test_1<10000,8000,ut_htcmp_t1>(*this);
    ut_hashtbl_ext_test_1<10000,8000,ut_htcmp_t2>(*this);
    ut_hashtbl_ext_test_1<10000,8000,ut_htcmp_t3>(*this);
    ut_hashtbl_ext_test_1<10000,8000,ut_htcmp_t4>(*this);
    ut_hashtbl_ext_test_1<10000,9000,ut_htcmp_t1>(*this);
    ut_hashtbl_ext_test_1<10000,9000,ut_htcmp_t2>(*this);
    ut_hashtbl_ext_test_1<10000,9000,ut_htcmp_t3>(*this);
    ut_hashtbl_ext_test_1<10000,9000,ut_htcmp_t4>(*this);
}


#endif
