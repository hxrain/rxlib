#ifndef _UT_HSM_CORE_H_
#define _UT_HSM_CORE_H_

#include "../rx_hsm_core.h"
#include "../rx_tdd.h"

namespace rx_ut
{
    //HSM状态事件处理器
    class hsm_tst_handle_t
    {
    public:
        uint32_t on_se(rx::hsm_core_t &hsm, rx::hsm_state_node_t &state_node, uint16_t event_code, void *usrdat)
        {
            uint32_t data=*(uint32_t*)usrdat;
            void *usrobj=hsm.usrobj();
            printf("%p :: s(%d)e(%d) :: data(%u)",usrobj,state_node.code,event_code,data);
            return 1+data;
        }
        uint32_t on_se_entry(rx::hsm_core_t &hsm, rx::hsm_state_node_t &state_node, uint16_t event_code, void *p)
        {
            uint32_t passed_on=*(uint32_t*)p;
            void *usrobj=hsm.usrobj();
            printf("%p :: s(%d)e(%d) :: entry(%u)",usrobj,state_node.code,event_code,passed_on);
            return 1;
        }
        uint32_t on_se_leave(rx::hsm_core_t &hsm, rx::hsm_state_node_t &state_node, uint16_t event_code, void *p)
        {
            uint32_t passed_on=*(uint32_t*)p;
            void *usrobj=hsm.usrobj();
            printf("%p :: s(%d)e(%d) :: leave(%u)",usrobj,state_node.code,event_code,passed_on);
            return 1;
        }
    };

    //状态树类型
    typedef rx::hsm_tree_t<16,41> hsm_tree_tst_t;
    hsm_tree_tst_t      G_hsm_tree;                         //定义状态树对象
    hsm_tst_handle_t    G_hsm_handler;                      //定义状态事件处理对象
    //-----------------------------------------------------
    void test_hsm_core_1(rx_tdd_t &rt)
    {
        rt.tdd_assert(G_hsm_tree.make_event(rx::HSM_ROOT_STATE,0)>0);
        G_hsm_tree.find_event(rx::HSM_ROOT_STATE,0)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);
        rt.tdd_assert(G_hsm_tree.make_event(rx::HSM_ROOT_STATE,1)>0);
        G_hsm_tree.find_event(rx::HSM_ROOT_STATE,1)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);

        rt.tdd_assert(G_hsm_tree.make_state(1)>0);
        rt.tdd_assert(G_hsm_tree.make_event(1,rx::HSM_EVENT_ENTRY)>0);
        G_hsm_tree.find_event(1,rx::HSM_EVENT_ENTRY)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_entry);
        rt.tdd_assert(G_hsm_tree.make_event(1,rx::HSM_EVENT_LEAVE)>0);
        G_hsm_tree.find_event(1,rx::HSM_EVENT_LEAVE)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_leave);
        rt.tdd_assert(G_hsm_tree.make_event(1,1)>0);
        G_hsm_tree.find_event(1,1)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);
        rt.tdd_assert(G_hsm_tree.make_event(1,2)>0);
        G_hsm_tree.find_event(1,2)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);

        rt.tdd_assert(G_hsm_tree.make_state(11,1)>0);
        rt.tdd_assert(G_hsm_tree.make_event(11,rx::HSM_EVENT_ENTRY)>0);
        G_hsm_tree.find_event(11,rx::HSM_EVENT_ENTRY)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_entry);
        rt.tdd_assert(G_hsm_tree.make_event(11,rx::HSM_EVENT_LEAVE)>0);
        G_hsm_tree.find_event(11,rx::HSM_EVENT_LEAVE)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_leave);
        rt.tdd_assert(G_hsm_tree.make_event(11,1)>0);
        G_hsm_tree.find_event(11,1)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);
        rt.tdd_assert(G_hsm_tree.make_event(11,2)>0);
        G_hsm_tree.find_event(11,2)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);

        rt.tdd_assert(G_hsm_tree.make_state(12,1)>0);
        rt.tdd_assert(G_hsm_tree.make_event(12,rx::HSM_EVENT_ENTRY)>0);
        G_hsm_tree.find_event(12,rx::HSM_EVENT_ENTRY)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_entry);
        rt.tdd_assert(G_hsm_tree.make_event(12,rx::HSM_EVENT_LEAVE)>0);
        G_hsm_tree.find_event(12,rx::HSM_EVENT_LEAVE)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_leave);
        rt.tdd_assert(G_hsm_tree.make_event(12,1)>0);
        G_hsm_tree.find_event(12,1)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);
        rt.tdd_assert(G_hsm_tree.make_event(12,2)>0);
        G_hsm_tree.find_event(12,2)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);

        rt.tdd_assert(G_hsm_tree.make_state(2)>0);
        rt.tdd_assert(G_hsm_tree.make_event(2,rx::HSM_EVENT_ENTRY)>0);
        G_hsm_tree.find_event(2,rx::HSM_EVENT_ENTRY)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_entry);
        rt.tdd_assert(G_hsm_tree.make_event(2,rx::HSM_EVENT_LEAVE)>0);
        G_hsm_tree.find_event(2,rx::HSM_EVENT_LEAVE)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_leave);
        rt.tdd_assert(G_hsm_tree.make_event(2,1)>0);
        G_hsm_tree.find_event(2,1)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);

        rt.tdd_assert(G_hsm_tree.make_state(21,2)>0);
        rt.tdd_assert(G_hsm_tree.make_event(21,rx::HSM_EVENT_ENTRY)>0);
        G_hsm_tree.find_event(21,rx::HSM_EVENT_ENTRY)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_entry);
        rt.tdd_assert(G_hsm_tree.make_event(21,rx::HSM_EVENT_LEAVE)>0);
        G_hsm_tree.find_event(21,rx::HSM_EVENT_LEAVE)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_leave);
        rt.tdd_assert(G_hsm_tree.make_event(21,1)>0);
        G_hsm_tree.find_event(21,1)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);
        rt.tdd_assert(G_hsm_tree.make_event(21,2)>0);
        G_hsm_tree.find_event(21,2)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);

        rt.tdd_assert(G_hsm_tree.make_state(22,2)>0);
        rt.tdd_assert(G_hsm_tree.make_event(22,rx::HSM_EVENT_ENTRY)>0);
        G_hsm_tree.find_event(22,rx::HSM_EVENT_ENTRY)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_entry);
        rt.tdd_assert(G_hsm_tree.make_event(22,rx::HSM_EVENT_LEAVE)>0);
        G_hsm_tree.find_event(22,rx::HSM_EVENT_LEAVE)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_leave);
        rt.tdd_assert(G_hsm_tree.make_event(22,1)>0);
        G_hsm_tree.find_event(22,1)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);
        rt.tdd_assert(G_hsm_tree.make_event(22,2)>0);
        G_hsm_tree.find_event(22,2)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);

        rt.tdd_assert(G_hsm_tree.make_state(3)>0);
        rt.tdd_assert(G_hsm_tree.make_event(3,rx::HSM_EVENT_ENTRY)>0);
        G_hsm_tree.find_event(3, rx::HSM_EVENT_ENTRY)->bind(G_hsm_handler, &hsm_tst_handle_t::on_se_entry);
        rt.tdd_assert(G_hsm_tree.make_event(3,rx::HSM_EVENT_LEAVE)>0);
        G_hsm_tree.find_event(3,rx::HSM_EVENT_LEAVE)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_leave);
        rt.tdd_assert(G_hsm_tree.make_event(3,1)>0);
        G_hsm_tree.find_event(3,1)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);

        rt.tdd_assert(G_hsm_tree.make_state(31,3)>0);
        rt.tdd_assert(G_hsm_tree.make_event(31,rx::HSM_EVENT_ENTRY)>0);
        G_hsm_tree.find_event(31,rx::HSM_EVENT_ENTRY)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_entry);
        rt.tdd_assert(G_hsm_tree.make_event(31,rx::HSM_EVENT_LEAVE)>0);
        G_hsm_tree.find_event(31,rx::HSM_EVENT_LEAVE)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_leave);
        rt.tdd_assert(G_hsm_tree.make_event(31,1)>0);
        G_hsm_tree.find_event(31,1)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);
        rt.tdd_assert(G_hsm_tree.make_event(31,2)>0);
        G_hsm_tree.find_event(31,2)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);

        rt.tdd_assert(G_hsm_tree.make_state(32,3)>0);
        rt.tdd_assert(G_hsm_tree.make_event(32,rx::HSM_EVENT_ENTRY)>0);
        G_hsm_tree.find_event(32,rx::HSM_EVENT_ENTRY)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_entry);
        rt.tdd_assert(G_hsm_tree.make_event(32,rx::HSM_EVENT_LEAVE)>0);
        G_hsm_tree.find_event(32,rx::HSM_EVENT_LEAVE)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se_leave);
        rt.tdd_assert(G_hsm_tree.make_event(32,1)>0);
        G_hsm_tree.find_event(32,1)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);
        rt.tdd_assert(G_hsm_tree.make_event(32,2)>0);
        G_hsm_tree.find_event(32,2)->bind(G_hsm_handler,&hsm_tst_handle_t::on_se);

        rx::hsm_t hsm(G_hsm_tree);

        rt.tdd_assert(hsm.init());
        uint32_t usrdat=1;
        rt.tdd_assert(hsm.hit(0, &usrdat) == 1 + 1);
        usrdat = 2;
        rt.tdd_assert(hsm.hit(0, &usrdat) == 1 + 2);
    }
}

rx_tdd(test_hsm_core_base_1)
{
    rx_ut::test_hsm_core_1(*this);
}





#endif // _UT_MEM_POOL_H_
