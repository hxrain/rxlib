#ifndef _UT_HSM_CORE_H_
#define _UT_HSM_CORE_H_

#include "../rx_hsm_core.h"
#include "../rx_tdd.h"

namespace rx_ut
{
    const bool ut_show_hsm_debug = false;

    //HSM状态事件处理器
    class hsm_hdr_t
    {
    public:
        uint16_t on_event(rx::hsm_core_t &hsm, const rx::hsm_tree_i::state_node_t &state_node, uint16_t event_code, void *usrdat)
        {
            uint32_t data=*(uint32_t*)usrdat;
            void *usrobj=hsm.usrobj();
            tdd_print(ut_show_hsm_debug,"%p :: hit_state(%5d) : cur_state(%5d) : event(%5d) :: data(%u)\n",usrobj,state_node.code,hsm.curr_state()->code,event_code,data);
            return 1+data;
        }
        void on_entry(rx::hsm_core_t &hsm, const rx::hsm_tree_i::state_node_t &state_node, uint16_t event_code, uint16_t passed_on)
        {
            void *usrobj=hsm.usrobj();
            tdd_print(ut_show_hsm_debug, "%p :: hit_state(%5d) : cur_state(%5d) : Entry(%5d) :: passed(%u)\n",usrobj,state_node.code, hsm.curr_state()->code,event_code,passed_on);
        }
        void on_leave(rx::hsm_core_t &hsm, const rx::hsm_tree_i::state_node_t &state_node, uint16_t event_code, uint16_t passed_on)
        {
            void *usrobj=hsm.usrobj();
            tdd_print(ut_show_hsm_debug, "%p :: hit_state(%5d) : cur_state(%5d) : Leave(%5d) :: passed(%u)\n",usrobj,state_node.code, hsm.curr_state()->code,event_code,passed_on);
        }
    };

    //状态树类型
    typedef rx::hsm_tree_t<16,41> hsm_tree_tst_t;
    hsm_tree_tst_t      G_hsm_tree;                         //定义状态树对象
    hsm_hdr_t           G_hsm_handler;                      //定义状态事件处理对象

    //-----------------------------------------------------
    void test_hsm_core_hit_0(rx_tdd_t &rt, rx::hsm_t &hsm)
    {
        rx::hsm_tree_i::state_node_t *hit_state=NULL;

        rt.tdd_assert(hsm.init(1));
        int32_t usrdat=1;
        rt.tdd_assert(hsm.hit(0, &usrdat) == 1 + usrdat);
        usrdat = 2;
        rt.tdd_assert(hsm.hit(1, &usrdat) == 1 + usrdat);

        rt.tdd_assert(hsm.go(1));
        rt.tdd_assert(hsm.curr_state()->code==1);
        rt.tdd_assert(hsm.hit(0, &usrdat,&hit_state) == 1 + usrdat);
        rt.tdd_assert(hit_state->code == 0);
        ++usrdat;

        rt.tdd_assert(hsm.go(11));
        rt.tdd_assert(hsm.curr_state()->code == 11);
        rt.tdd_assert(hsm.hit(2, &usrdat) == 1 + usrdat);

        rt.tdd_assert(hsm.go(12));
        rt.tdd_assert(hsm.curr_state()->code == 12);
        rt.tdd_assert(hsm.hit(0, &usrdat) == 1 + usrdat);

        ++usrdat;

        rt.tdd_assert(hsm.go(21));
        rt.tdd_assert(hsm.curr_state()->code == 21);
        rt.tdd_assert(hsm.hit(0, &usrdat) == 1 + usrdat);

        rt.tdd_assert(hsm.go(3));
        rt.tdd_assert(hsm.curr_state()->code == 3);
        rt.tdd_assert(hsm.hit(2, &usrdat) == 1 + usrdat);

        rt.tdd_assert(hsm.go(12));
        rt.tdd_assert(hsm.curr_state()->code == 12);
        rt.tdd_assert(hsm.hit(0, &usrdat) == 1 + usrdat);

        rt.tdd_assert(hsm.go(32));
        rt.tdd_assert(hsm.curr_state()->code == 32);
        rt.tdd_assert(hsm.hit(1, &usrdat) == 1 + usrdat);

        rt.tdd_assert(hsm.go(1));
        rt.tdd_assert(hsm.curr_state()->code == 1);
        rt.tdd_assert(hsm.hit(0, &usrdat) == 1 + usrdat);

        rt.tdd_assert(hsm.go(11));
        rt.tdd_assert(hsm.curr_state()->code == 11);
        rt.tdd_assert(hsm.hit(2, &usrdat) == 1 + usrdat);

        rt.tdd_assert(hsm.go(2));
        rt.tdd_assert(hsm.curr_state()->code == 2);
        rt.tdd_assert(hsm.hit(2, &usrdat) == 1 + usrdat);

        rt.tdd_assert(hsm.go(3));
        rt.tdd_assert(hsm.curr_state()->code == 3);
        rt.tdd_assert(hsm.hit(0, &usrdat) == 1 + usrdat);

        rt.tdd_assert(hsm.go(12));
        rt.tdd_assert(hsm.curr_state()->code == 12);
        rt.tdd_assert(hsm.hit(1, &usrdat) == 1 + usrdat);

        rt.tdd_assert(hsm.go(32));
        rt.tdd_assert(hsm.curr_state()->code == 32);
        rt.tdd_assert(hsm.hit(0, &usrdat) == 1 + usrdat);

        rt.tdd_assert(hsm.go(22));
        rt.tdd_assert(hsm.curr_state()->code == 22);
        rt.tdd_assert(hsm.hit(2, &usrdat) == 1 + usrdat);

        rt.tdd_assert(hsm.go(3));
        rt.tdd_assert(hsm.curr_state()->code == 3);
        rt.tdd_assert(hsm.hit(0, &usrdat) == 1 + usrdat);

        tdd_print(ut_show_hsm_debug, "             *********************            \n\n");
    }

    //-----------------------------------------------------
    //使用hsm_tree_t的基础API进行状态树的生成
    void test_hsm_core_1(rx_tdd_t &rt)
    {
        G_hsm_tree.clear();
        rt.tdd_assert(G_hsm_tree.make_state(0)>0);
        rt.tdd_assert(G_hsm_tree.make_over(0, true)>0);
        G_hsm_tree.find_over(0, true)->bind(G_hsm_handler, &hsm_hdr_t::on_entry);
        rt.tdd_assert(G_hsm_tree.make_over(0, false)>0);
        G_hsm_tree.find_over(0, false)->bind(G_hsm_handler, &hsm_hdr_t::on_leave);
        rt.tdd_assert(G_hsm_tree.make_event(0, 0)>0);
        G_hsm_tree.find_event(0, 0)->bind(G_hsm_handler, &hsm_hdr_t::on_event);
        rt.tdd_assert(G_hsm_tree.make_event(0, 1)>0);
        G_hsm_tree.find_event(0, 1)->bind(G_hsm_handler, &hsm_hdr_t::on_event);
        rt.tdd_assert(G_hsm_tree.make_event(0, 2)>0);
        G_hsm_tree.find_event(0, 2)->bind(G_hsm_handler, &hsm_hdr_t::on_event);

        rt.tdd_assert(G_hsm_tree.make_state(1,0,NULL)>0);
        rt.tdd_assert(G_hsm_tree.make_over(1,true)>0);
        G_hsm_tree.find_over(1, true)->bind(G_hsm_handler,&hsm_hdr_t::on_entry);
        rt.tdd_assert(G_hsm_tree.make_over(1,false)>0);
        G_hsm_tree.find_over(1,false)->bind(G_hsm_handler,&hsm_hdr_t::on_leave);
        rt.tdd_assert(G_hsm_tree.make_event(1,1)>0);
        G_hsm_tree.find_event(1,1)->bind(G_hsm_handler,&hsm_hdr_t::on_event);
        rt.tdd_assert(G_hsm_tree.make_event(1,2)>0);
        G_hsm_tree.find_event(1,2)->bind(G_hsm_handler,&hsm_hdr_t::on_event);

        rt.tdd_assert(G_hsm_tree.make_state(11,1)>0);
        rt.tdd_assert(G_hsm_tree.make_over(11,true)>0);
        G_hsm_tree.find_over(11, true)->bind(G_hsm_handler,&hsm_hdr_t::on_entry);
        rt.tdd_assert(G_hsm_tree.make_over(11,false)>0);
        G_hsm_tree.find_over(11,false)->bind(G_hsm_handler,&hsm_hdr_t::on_leave);
        rt.tdd_assert(G_hsm_tree.make_event(11,1)>0);
        G_hsm_tree.find_event(11,1)->bind(G_hsm_handler,&hsm_hdr_t::on_event);
        rt.tdd_assert(G_hsm_tree.make_event(11,2)>0);
        G_hsm_tree.find_event(11,2)->bind(G_hsm_handler,&hsm_hdr_t::on_event);

        rt.tdd_assert(G_hsm_tree.make_state(12,1)>0);
        rt.tdd_assert(G_hsm_tree.make_over(12,true)>0);
        G_hsm_tree.find_over(12, true)->bind(G_hsm_handler,&hsm_hdr_t::on_entry);
        rt.tdd_assert(G_hsm_tree.make_over(12,false)>0);
        G_hsm_tree.find_over(12,false)->bind(G_hsm_handler,&hsm_hdr_t::on_leave);
        rt.tdd_assert(G_hsm_tree.make_event(12,1)>0);
        G_hsm_tree.find_event(12,1)->bind(G_hsm_handler,&hsm_hdr_t::on_event);
        rt.tdd_assert(G_hsm_tree.make_event(12,2)>0);
        G_hsm_tree.find_event(12,2)->bind(G_hsm_handler,&hsm_hdr_t::on_event);

        rt.tdd_assert(G_hsm_tree.make_state(2,0,NULL)>0);
        rt.tdd_assert(G_hsm_tree.make_over(2,true)>0);
        G_hsm_tree.find_over(2, true)->bind(G_hsm_handler,&hsm_hdr_t::on_entry);
        rt.tdd_assert(G_hsm_tree.make_over(2,false)>0);
        G_hsm_tree.find_over(2,false)->bind(G_hsm_handler,&hsm_hdr_t::on_leave);
        rt.tdd_assert(G_hsm_tree.make_event(2,1)>0);
        G_hsm_tree.find_event(2,1)->bind(G_hsm_handler,&hsm_hdr_t::on_event);

        rt.tdd_assert(G_hsm_tree.make_state(21,2)>0);
        rt.tdd_assert(G_hsm_tree.make_over(21,true)>0);
        G_hsm_tree.find_over(21, true)->bind(G_hsm_handler,&hsm_hdr_t::on_entry);
        rt.tdd_assert(G_hsm_tree.make_over(21,false)>0);
        G_hsm_tree.find_over(21,false)->bind(G_hsm_handler,&hsm_hdr_t::on_leave);
        rt.tdd_assert(G_hsm_tree.make_event(21,1)>0);
        G_hsm_tree.find_event(21,1)->bind(G_hsm_handler,&hsm_hdr_t::on_event);
        rt.tdd_assert(G_hsm_tree.make_event(21,2)>0);
        G_hsm_tree.find_event(21,2)->bind(G_hsm_handler,&hsm_hdr_t::on_event);

        rt.tdd_assert(G_hsm_tree.make_state(22,2)>0);
        rt.tdd_assert(G_hsm_tree.make_over(22,true)>0);
        G_hsm_tree.find_over(22, true)->bind(G_hsm_handler,&hsm_hdr_t::on_entry);
        rt.tdd_assert(G_hsm_tree.make_over(22,false)>0);
        G_hsm_tree.find_over(22,false)->bind(G_hsm_handler,&hsm_hdr_t::on_leave);
        rt.tdd_assert(G_hsm_tree.make_event(22,1)>0);
        G_hsm_tree.find_event(22,1)->bind(G_hsm_handler,&hsm_hdr_t::on_event);
        rt.tdd_assert(G_hsm_tree.make_event(22,2)>0);
        G_hsm_tree.find_event(22,2)->bind(G_hsm_handler,&hsm_hdr_t::on_event);

        rt.tdd_assert(G_hsm_tree.make_state(3, 0, NULL)>0);
        rt.tdd_assert(G_hsm_tree.make_over(3,true)>0);
        G_hsm_tree.find_over(3, true)->bind(G_hsm_handler, &hsm_hdr_t::on_entry);
        rt.tdd_assert(G_hsm_tree.make_over(3,false)>0);
        G_hsm_tree.find_over(3,false)->bind(G_hsm_handler,&hsm_hdr_t::on_leave);
        rt.tdd_assert(G_hsm_tree.make_event(3,1)>0);
        G_hsm_tree.find_event(3,1)->bind(G_hsm_handler,&hsm_hdr_t::on_event);

        rt.tdd_assert(G_hsm_tree.make_state(31,3)>0);
        rt.tdd_assert(G_hsm_tree.make_over(31,true)>0);
        G_hsm_tree.find_over(31, true)->bind(G_hsm_handler,&hsm_hdr_t::on_entry);
        rt.tdd_assert(G_hsm_tree.make_over(31,false)>0);
        G_hsm_tree.find_over(31,false)->bind(G_hsm_handler,&hsm_hdr_t::on_leave);
        rt.tdd_assert(G_hsm_tree.make_event(31,1)>0);
        G_hsm_tree.find_event(31,1)->bind(G_hsm_handler,&hsm_hdr_t::on_event);
        rt.tdd_assert(G_hsm_tree.make_event(31,2)>0);
        G_hsm_tree.find_event(31,2)->bind(G_hsm_handler,&hsm_hdr_t::on_event);

        rt.tdd_assert(G_hsm_tree.make_state(32,3)>0);
        rt.tdd_assert(G_hsm_tree.make_over(32,true)>0);
        G_hsm_tree.find_over(32, true)->bind(G_hsm_handler,&hsm_hdr_t::on_entry);
        rt.tdd_assert(G_hsm_tree.make_over(32,false)>0);
        G_hsm_tree.find_over(32,false)->bind(G_hsm_handler,&hsm_hdr_t::on_leave);
        rt.tdd_assert(G_hsm_tree.make_event(32,1)>0);
        G_hsm_tree.find_event(32,1)->bind(G_hsm_handler,&hsm_hdr_t::on_event);
        rt.tdd_assert(G_hsm_tree.make_event(32,2)>0);
        G_hsm_tree.find_event(32,2)->bind(G_hsm_handler,&hsm_hdr_t::on_event);

        rx::hsm_t hsm(G_hsm_tree);

        test_hsm_core_hit_0(rt,hsm);
    }

    //-----------------------------------------------------
    //使用hsm_tree_maker进行状态树的生成.
    void test_hsm_core_2(rx_tdd_t &rt)
    {
        G_hsm_tree.clear();
        rx::hsm_tree_maker<hsm_tree_tst_t> tm(G_hsm_tree);
        tm.state(0)
            .entry(G_hsm_handler, &hsm_hdr_t::on_entry)
            .leave(G_hsm_handler, &hsm_hdr_t::on_leave)
            .event(0, G_hsm_handler, &hsm_hdr_t::on_event)
            .event(1, G_hsm_handler, &hsm_hdr_t::on_event)
            .event(2, G_hsm_handler, &hsm_hdr_t::on_event)
            .state(1,0,NULL)
                .entry(G_hsm_handler, &hsm_hdr_t::on_entry)
                .leave(G_hsm_handler, &hsm_hdr_t::on_leave)
                .event(1, G_hsm_handler, &hsm_hdr_t::on_event)
                .event(2, G_hsm_handler, &hsm_hdr_t::on_event)
                .state(11, 1, NULL)
                    .entry(G_hsm_handler, &hsm_hdr_t::on_entry)
                    .leave(G_hsm_handler, &hsm_hdr_t::on_leave)
                    .event(1, G_hsm_handler, &hsm_hdr_t::on_event)
                    .event(2, G_hsm_handler, &hsm_hdr_t::on_event)
                .state(12, 1, NULL)
                    .entry(G_hsm_handler, &hsm_hdr_t::on_entry)
                    .leave(G_hsm_handler, &hsm_hdr_t::on_leave)
                    .event(1, G_hsm_handler, &hsm_hdr_t::on_event)
                    .event(2, G_hsm_handler, &hsm_hdr_t::on_event)
            .state(2,0,NULL)
                .entry(G_hsm_handler, &hsm_hdr_t::on_entry)
                .leave(G_hsm_handler, &hsm_hdr_t::on_leave)
                .event(1, G_hsm_handler, &hsm_hdr_t::on_event)
                .state(21, 2, NULL)
                    .entry(G_hsm_handler, &hsm_hdr_t::on_entry)
                    .leave(G_hsm_handler, &hsm_hdr_t::on_leave)
                    .event(1, G_hsm_handler, &hsm_hdr_t::on_event)
                    .event(2, G_hsm_handler, &hsm_hdr_t::on_event)
                .state(22, 2, NULL)
                    .entry(G_hsm_handler, &hsm_hdr_t::on_entry)
                    .leave(G_hsm_handler, &hsm_hdr_t::on_leave)
                    .event(1, G_hsm_handler, &hsm_hdr_t::on_event)
                    .event(2, G_hsm_handler, &hsm_hdr_t::on_event)
            .state(3,0,NULL)
                .entry(G_hsm_handler, &hsm_hdr_t::on_entry)
                .leave(G_hsm_handler, &hsm_hdr_t::on_leave)
                .event(1, G_hsm_handler, &hsm_hdr_t::on_event)
                .state(31, 3, NULL)
                    .entry(G_hsm_handler, &hsm_hdr_t::on_entry)
                    .leave(G_hsm_handler, &hsm_hdr_t::on_leave)
                    .event(1, G_hsm_handler, &hsm_hdr_t::on_event)
                    .event(2, G_hsm_handler, &hsm_hdr_t::on_event)
                .state(32, 3, NULL)
                    .entry(G_hsm_handler, &hsm_hdr_t::on_entry)
                    .leave(G_hsm_handler, &hsm_hdr_t::on_leave)
                    .event(1, G_hsm_handler, &hsm_hdr_t::on_event)
                    .event(2, G_hsm_handler, &hsm_hdr_t::on_event);

        rx::hsm_t hsm(G_hsm_tree);

        test_hsm_core_hit_0(rt, hsm);
    }
    //-----------------------------------------------------
    //使用hsm_maker进行状态树的生成.
    void test_hsm_core_3(rx_tdd_t &rt)
    {
        G_hsm_tree.clear();
        rx::hsm_maker<hsm_tree_tst_t, hsm_hdr_t> htm(G_hsm_tree, G_hsm_handler);
        htm.state(0)
            .entry(&hsm_hdr_t::on_entry)
            .leave(&hsm_hdr_t::on_leave)
            .event(0, &hsm_hdr_t::on_event)
            .event(1, &hsm_hdr_t::on_event)
            .event(2, &hsm_hdr_t::on_event)
            .state(1,0,NULL)
                .entry(&hsm_hdr_t::on_entry)
                .leave(&hsm_hdr_t::on_leave)
                .event(1, &hsm_hdr_t::on_event)
                .event(2, &hsm_hdr_t::on_event)
                .state(11, 1)
                    .entry(&hsm_hdr_t::on_entry)
                    .leave(&hsm_hdr_t::on_leave)
                    .event(1, &hsm_hdr_t::on_event)
                    .event(2, &hsm_hdr_t::on_event)
                .state(12)
                    .entry(&hsm_hdr_t::on_entry)
                    .leave(&hsm_hdr_t::on_leave)
                    .event(1, &hsm_hdr_t::on_event)
                    .event(2, &hsm_hdr_t::on_event)
            .state(2,0,NULL)
                .entry(&hsm_hdr_t::on_entry)
                .leave(&hsm_hdr_t::on_leave)
                .event(1, &hsm_hdr_t::on_event)
                .state(21, 2)
                    .entry(&hsm_hdr_t::on_entry)
                    .leave(&hsm_hdr_t::on_leave)
                    .event(1, &hsm_hdr_t::on_event)
                    .event(2, &hsm_hdr_t::on_event)
                .state(22)
                    .entry(&hsm_hdr_t::on_entry)
                    .leave(&hsm_hdr_t::on_leave)
                    .event(1, &hsm_hdr_t::on_event)
                    .event(2, &hsm_hdr_t::on_event)
            .state(3,0,NULL)
                .entry(&hsm_hdr_t::on_entry)
                .leave(&hsm_hdr_t::on_leave)
                .event(1, &hsm_hdr_t::on_event)
                .state(31, 3)
                    .entry(&hsm_hdr_t::on_entry)
                    .leave(&hsm_hdr_t::on_leave)
                    .event(1, &hsm_hdr_t::on_event)
                    .event(2, &hsm_hdr_t::on_event)
                .state(32)
                    .entry(&hsm_hdr_t::on_entry)
                    .leave(&hsm_hdr_t::on_leave)
                    .event(1, &hsm_hdr_t::on_event)
                    .event(2, &hsm_hdr_t::on_event);

        rx::hsm_t hsm(G_hsm_tree);

        test_hsm_core_hit_0(rt, hsm);
    }
}

rx_tdd(test_hsm_core_base_1)
{
    //使用各种方式生成相同的状态树并进行相同的测试路径的测试.
    rx_ut::test_hsm_core_1(*this);
    rx_ut::test_hsm_core_2(*this);
    rx_ut::test_hsm_core_3(*this);
}





#endif // _UT_MEM_POOL_H_
