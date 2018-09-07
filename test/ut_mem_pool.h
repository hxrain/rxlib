#ifndef _UT_MEM_POOL_H_
#define _UT_MEM_POOL_H_

#include "../rx_mem_pool_fx1.h"
#include "../rx_tdd.h"

typedef struct tmp_raw_stack_node_t
{
    struct tmp_raw_stack_node_t* volatile next;	            //节点的后趋
    uint32_t tmp_value;
}tmp_raw_stack_node_t;

//---------------------------------------------------------
inline void test_raw_list_1(rx_tdd_t &rt)
{
    tmp_raw_stack_node_t *node;
    tmp_raw_stack_node_t tmp[3];
    rx::raw_queue_t<tmp_raw_stack_node_t> list;
    for(int i=0;i<3;++i)
        tmp[i].tmp_value=i;

    rt.tdd_assert(list.size()==0);
    rt.tdd_assert(list.head()==NULL);
    rt.tdd_assert(list.tail()==NULL);

    list.push_back(tmp[0]);
    rt.tdd_assert(list.size()==1);
    rt.tdd_assert(list.head()==list.tail());
    rt.tdd_assert(list.tail()->next==NULL);
    rt.tdd_assert(list.tail()->tmp_value==0);

    list.push_back(tmp[1]);
    rt.tdd_assert(list.size()==2);
    rt.tdd_assert(list.head()!=list.tail());
    rt.tdd_assert(list.head()->next==list.tail());
    rt.tdd_assert(list.tail()->next==NULL);
    rt.tdd_assert(list.tail()->tmp_value==1);

    list.push_front(tmp[2]);
    rt.tdd_assert(list.size()==3);
    rt.tdd_assert(list.head()!=list.tail());
    rt.tdd_assert(list.head()->next->next==list.tail());
    rt.tdd_assert(list.tail()->next==NULL);
    rt.tdd_assert(list.head()->tmp_value==2);

    node=list.pop_front();
    rt.tdd_assert(node->tmp_value==2);
    rt.tdd_assert(list.size()==2);
    rt.tdd_assert(list.head()->next==list.tail());
    rt.tdd_assert(list.tail()->next==NULL);
    rt.tdd_assert(list.head()->tmp_value==0);

    node=list.pop_front();
    rt.tdd_assert(node->tmp_value==0);
    rt.tdd_assert(list.size()==1);
    rt.tdd_assert(list.head()==list.tail());
    rt.tdd_assert(list.tail()->next==NULL);
    rt.tdd_assert(list.head()->tmp_value==1);

    node=list.pop_front();
    rt.tdd_assert(node->tmp_value==1);
    rt.tdd_assert(list.size()==0);
    rt.tdd_assert(list.head()==list.tail());
    rt.tdd_assert(list.tail()==NULL);
    rt.tdd_assert(list.head()==NULL);

}
//---------------------------------------------------------
inline void test_raw_stack_1(rx_tdd_t &rt)
{
    tmp_raw_stack_node_t *node;
    tmp_raw_stack_node_t tmp[3];
    rx::raw_stack_t<tmp_raw_stack_node_t> stack;
    for(int i=0;i<3;++i)
        tmp[i].tmp_value=i;

    rt.tdd_assert(stack.size()==0);
    rt.tdd_assert(stack.head()==NULL);

    stack.push_front(tmp[0]);
    rt.tdd_assert(stack.size()==1);
    rt.tdd_assert(stack.head()!=NULL);
    rt.tdd_assert(stack.head()->next==NULL);
    rt.tdd_assert(stack.head()->tmp_value==0);

    stack.push_front(tmp[1]);
    rt.tdd_assert(stack.size()==2);
    rt.tdd_assert(stack.head()->next==&tmp[0]);
    rt.tdd_assert(stack.head()->tmp_value==1);

    node=stack.pop_front();
    rt.tdd_assert(node->tmp_value==1);
    rt.tdd_assert(stack.size()==1);
    rt.tdd_assert(stack.head()->next==NULL);

    node=stack.pop_front();
    rt.tdd_assert(node->tmp_value==0);
    rt.tdd_assert(stack.size()==0);
    rt.tdd_assert(stack.head()==NULL);

}
//---------------------------------------------------------
template<class T>
void test_mem_pool_base(rx_tdd_t &rt)
{
    uint32_t bsize;
    T mempool(4);

    uint8_t *p1=(uint8_t *)mempool.do_alloc(bsize);
    rt.assert(p1!=NULL);

    uint8_t *p2=(uint8_t *)mempool.do_alloc(bsize);
    rt.assert(p2!=NULL);

    mempool.do_free(p1);

    mempool.do_free(p2);

    mempool.do_uninit();
}

rx_tdd(test_raw_list_stack)
{
    test_raw_list_1(*this);
    test_raw_stack_1(*this);
}

rx_tdd(test_mem_pool_base)
{
    test_mem_pool_base<rx::mempool_fixed_t<> >(*this);
}





#endif // _UT_MEM_POOL_H_
