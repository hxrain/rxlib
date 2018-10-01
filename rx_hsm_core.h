#ifndef _RX_HSM_BASE_H_
#define _RX_HSM_BASE_H_

#include "rx_cc_macro.h"
#include "rx_hash_int.h"
#include "rx_dtl_hashtbl_raw.h"
#include "rx_ct_util.h"
#include "rx_ct_delegate.h"

namespace rx
{
    //层次状态机最大的层数
#ifndef HSM_MAX_LEVELs
    #define HSM_MAX_LEVELs    8
#endif
    //内置的状态类型:根状态
    const uint16_t HSM_ROOT_STATE  = (uint16_t)-1;
    //内置的事件类型:状态进入事件
    const uint16_t HSM_EVENT_ENTRY = (uint16_t)-2;
    //内置的事件类型:状态离开事件
    const uint16_t HSM_EVENT_LEAVE = (uint16_t)-3;

    //-----------------------------------------------------
    //定义状态信息结构体.父状态和层级号是非常重要的字段,他们明确了各个状态之间的路径到达关系.
    typedef struct hsm_state_node_t
    {
        hsm_state_node_t   *parent_state;                   //当前状态的父状态
        void               *usrptr;                         //状态树节点记录的扩展指针,方便外部使用
        uint16_t            state_level;                    //当前状态的层级深度
        uint16_t            code;                           //当前状态唯一标识(用于哈希表的key)
    }hsm_state_node_t;

    //-----------------------------------------------------
    class hsm_core_t;
    //业务事件回调委托类型定义
    typedef delegate4_rt<hsm_core_t&, hsm_state_node_t&, uint16_t, void*,    uint32_t>    hsm_evt_delegate_t;
    //状态进出事件回调委托类型定义
    typedef delegate4_rt<hsm_core_t&, hsm_state_node_t&, uint16_t, uint32_t, uint32_t>    hsm_sel_delegate_t;

    //-----------------------------------------------------
    //定义事件信息结构体
    typedef struct hsm_state_event_t
    {
        union{
        hsm_evt_delegate_t  evt_cb;                         //状态业务事件对应的回调函数
        hsm_sel_delegate_t  sel_cb;                         //状态进出事件对应的回调函数
        };
        uint32_t            code;                           //状态事件的唯一标识(用于哈希表的key)
    }hsm_state_event_t;

    //-----------------------------------------------------
    //拼装状态码与事件码并组成新key的函数
    typedef uint32_t (*hsm_st_ev_key_t)(uint16_t state_code, uint16_t event_code);
    inline uint32_t hsm_st_ev_key(uint16_t state_code, uint16_t event_code)
    {
        return ((uint32_t)state_code << 16) | event_code;
    }

    //-----------------------------------------------------
    //层次状态机使用的状态树接口
    class hsm_tree_i
    {
    protected:
        hsm_state_node_t    m_root_state;                   //根状态节点
        //-------------------------------------------------
    public:
        hsm_st_ev_key_t     state_event_key_fun;            //拼装状态事件key的函数
        rx_int_hash32_t     event_hashkey_fun;              //用于计算事件哈希索引的整数哈希函数
        rx_int_hash32_t     state_hashkey_fun;              //用于计算状态哈希索引的整数哈希函数
        //-------------------------------------------------
        hsm_tree_i()
        {
            event_hashkey_fun=rx_hash_skeeto_3s;
            state_hashkey_fun=rx_hash_skeeto_3s;
            state_event_key_fun=hsm_st_ev_key;

            m_root_state.usrptr=NULL;
            m_root_state.parent_state=NULL;
            m_root_state.state_level=0;
            m_root_state.code=HSM_ROOT_STATE;
        }
        virtual ~hsm_tree_i(){}
        //根据指定的状态值查找对应的状态节点
        virtual hsm_state_node_t*   find_state(uint16_t state_code) = 0;
        //根据指定的状态事件代码,查找对应的事件节点
        virtual hsm_evt_delegate_t* find_event(uint16_t state_code,uint16_t event_code) = 0;
        //查找指定状态上的进出事件
        virtual hsm_sel_delegate_t* find_over(uint16_t state_code, bool is_entry) = 0;
    };

    //-----------------------------------------------------
    //定义状态机依赖的状态树,记录所有状态的层级关系,一颗状态树可以被多个HSM状态机共用.
    template<uint32_t max_state_count,uint32_t max_event_count>
    class hsm_tree_t:public hsm_tree_i
    {
    protected:
        //简单哈希表使用的节点比较器
        class cmp_t
        {
        public:
            //对节点比较函数进行覆盖,需要真正使用节点中的key字段与给定的k值进行比较
            template<class NVT, class KT>
            static bool equ(const NVT &n, const KT &k) { return n.code == k; }
        };

        typedef raw_hashtbl_t<hsm_state_node_t, cmp_t> hsm_state_table_t;
        typedef raw_hashtbl_t<hsm_state_event_t, cmp_t> hsm_event_table_t;

        typename hsm_state_table_t::node_t m_states[max_state_count];
        typename hsm_event_table_t::node_t m_events[max_event_count];

        hsm_state_table_t        m_state_tbl;               //状态信息与状态值的哈希表
        hsm_event_table_t        m_event_tbl;               //状态事件与处理函数的哈希表

    public:
        //-------------------------------------------------
        hsm_tree_t()
        {
            m_state_tbl.bind(m_states,max_state_count);
            m_event_tbl.bind(m_events,max_event_count);
        }
        //-------------------------------------------------
        //根据指定的状态值查找对应的状态节点
        hsm_state_node_t* find_state(uint16_t state_code)
        {
            if (state_code==HSM_ROOT_STATE)
                return &(hsm_tree_i::m_root_state);
            uint32_t pos;
            typename hsm_state_table_t::node_t *node=m_state_tbl.find(hsm_tree_i::state_hashkey_fun(state_code),state_code,pos);
            return node?&node->value:NULL;
        }
        //-------------------------------------------------
        //根据指定的状态事件代码,查找对应的事件节点
        hsm_evt_delegate_t* find_event(uint16_t state_code,uint16_t event_code)
        {
            uint32_t svkey=hsm_tree_i::state_event_key_fun(state_code,event_code);
            typename hsm_event_table_t::node_t *node=m_event_tbl.find(hsm_tree_i::event_hashkey_fun(svkey),svkey);
            return node?&node->value.evt_cb:NULL;
        }
        //-------------------------------------------------
        //查找指定状态上的进出事件
        hsm_sel_delegate_t* find_over(uint16_t state_code, bool is_entry)
        {
            uint16_t event_code = is_entry ? HSM_EVENT_ENTRY : HSM_EVENT_LEAVE;
            uint32_t svkey = hsm_tree_i::state_event_key_fun(state_code, event_code);
            typename hsm_event_table_t::node_t *node = m_event_tbl.find(hsm_tree_i::event_hashkey_fun(svkey), svkey);
            return node ? &node->value.sel_cb : NULL;
        }
        //-------------------------------------------------
        //创建状态信息,记录该状态的父状态,并计算当前状态的层级level.
        //返回值:<0错误;0状态已经存在了;>0成功,为已经存储的状态数量.
        int make_state(uint16_t state_code, uint16_t parent_state = HSM_ROOT_STATE, void* usrptr = NULL)
        {
            if (state_code==parent_state||state_code==HSM_ROOT_STATE)
                return -4;

            //找到指定的父节点
            hsm_state_node_t *parent;
            if (parent_state == HSM_ROOT_STATE)
                parent = &(hsm_tree_i::m_root_state);
            else
            {//找不到指定的父节点,说明状态树的初始构造顺序错误.
                parent = find_state(parent_state);
                if (!parent)
                    return -1;
            }

            //进行层级检查
            uint32_t state_level = parent->state_level + 1;
            if (state_level >= HSM_MAX_LEVELs)
                return -2;                                  //当前的状态层级已经超过了限定值.


            //创建新节点
            uint32_t hashcode=hsm_tree_i::state_hashkey_fun(state_code);
            typename hsm_state_table_t::node_t *tbl_node = m_state_tbl.push(hashcode,state_code);
            if (!tbl_node)
                return -3;                                  //状态哈希表满了.
            hsm_state_node_t &state_node = tbl_node->value;

            if (state_node.parent_state)
                return 0;                                   //当前状态已经存在了,重复!

            //填充新状态的信息
            state_node.parent_state = parent;
            state_node.state_level = state_level;
            state_node.usrptr = usrptr;
            state_node.code=state_code;

            return m_state_tbl.size();
        }
        //-------------------------------------------------
        //创建状态对应的事件,之后需要查找该状态事件并绑定回调函数.
        //返回值:<0错误;0状态事件已经存在了;>0成功,为已经存储的状态事件数量.
        int make_event(uint16_t state_code, uint16_t event_code)
        {
            uint32_t evt_key=hsm_tree_i::state_event_key_fun(state_code, event_code);
            uint32_t hashcode=hsm_tree_i::state_hashkey_fun(evt_key);
            uint32_t pos;
            bool is_dup=false;
            typename hsm_event_table_t::node_t *tbl_node = m_event_tbl.push(hashcode,evt_key,pos,&is_dup);
            if (!tbl_node)
                return -1;                                  //状态事件表满了.
            if (is_dup)
                return 0;                                   //状态事件重复(或者是key函数冲突了)
            hsm_state_event_t &state_event = tbl_node->value;
            state_event.evt_cb.reset();
            state_event.code=evt_key;
            return m_event_tbl.size();
        }
        //-------------------------------------------------
        //创建状态对应的进入离开事件
        int make_over(uint16_t state_code, bool is_entry)
        {
            uint16_t ec = is_entry ? HSM_EVENT_ENTRY : HSM_EVENT_LEAVE;
            return make_event(state_code,ec);
        }
        //-------------------------------------------------
        //可以检查初始化是否正确完成
        //返回值:<0错误;0正常;>0为哈希表冲突总数(建议:调整哈希表容量/调整状态或事件的代码/调整哈希函数).
        uint32_t check(uint32_t *st_coll=NULL,uint32_t *ev_coll = NULL)
        {
            if (!m_state_tbl.size())
                return -1;
            if (!m_event_tbl.size())
                return -2;

            if (st_coll)
                *st_coll = m_state_tbl.collision();
            if (ev_coll)
                *ev_coll = m_event_tbl.collision();

            return m_state_tbl.collision()+m_event_tbl.collision();
        }
        //-------------------------------------------------
        //清理全部的状态树数据,可以重新构建.
        void clear()
        {
            m_state_tbl.clear();
            m_event_tbl.clear();
        }
    };

    //-----------------------------------------------------
    //基于状态树进行独立工作的状态机.根据需要,可以生成一个状态树的多个状态机,互相是独立的.
    //这里仅开放HSM的主功能,应用时使用子类hsm_t进行实例化.
    class hsm_core_t
    {
    protected:
        friend class hsm_tree_i;
        hsm_state_node_t   *m_curr_state;                   //指向当前状态信息
        hsm_tree_i         &m_state_tree;                   //指向状态树
        uint32_t            m_traning;                      //状态机迁移标记
        void               *m_usrobj;
        //-------------------------------------------------
        //内部调用,触发进入或离开状态的事件(如果有的话).idx为进入或离开的顺序,0代表最终进入或最初离开,非零代表中间过程.
        //返回值:是否触发了事件回调函数
        void m_walk_event(hsm_state_node_t *state_node, bool is_entry,uint32_t idx)
        {
            uint16_t state_code = state_node->code;
            uint16_t event_code = is_entry ? HSM_EVENT_ENTRY : HSM_EVENT_LEAVE;

            hsm_sel_delegate_t *E = m_state_tree.find_over(state_code, is_entry);
            if (!E || !E->is_valid())
                return;                                     //当前状态没有绑定此事件处理回调,那么就继续向上查找

            (*E)(*this, *state_node, event_code, idx);      //现在找到能够处理此事件的回调了
        }
        virtual ~hsm_core_t(){}
    public:
        hsm_core_t(hsm_tree_i& state_tree):m_curr_state(NULL),m_state_tree(state_tree),m_traning(0),m_usrobj(NULL){}
        //-------------------------------------------------
        //获取绑定的用户关联对象
        void *usrobj(){return m_usrobj;}
        //-------------------------------------------------
        //获取当前状态节点的状态代码和绑定的扩展指针
        hsm_state_node_t* curr_state() const { return m_curr_state; }
        //-------------------------------------------------
        //判断状态机的当前状态是否与给定的状态"相容"(直接相同或与给定状态的父状态相同)
        bool is_compatible(uint16_t state_code) const
        {
            for (hsm_state_node_t *state_node = m_curr_state; state_node; state_node = state_node->parent_state)
            {
                if (state_node->code == state_code)
                    return true;
            }
            return false;
        }
        //-------------------------------------------------
        //判断当前是否处于状态迁移过程中
        uint32_t going()const{return m_traning;}
        //-------------------------------------------------
        //状态机触发给定的事件.在状态机的当前状态或父状态链上,尝试触发给定的事件.
        //返回值:0未被正确处理;其他为事件处理回调返回值.
        int hit(uint16_t event_code, void *Data = NULL)
        {
            //从当前状态,一直尝试向上层查找
            for (hsm_state_node_t *state_node = m_curr_state; state_node; state_node = state_node->parent_state)
            {
                uint16_t state_code = state_node->code;
                hsm_evt_delegate_t *E = m_state_tree.find_event(state_code,event_code);
                if (!E || !E->is_valid())
                    continue;                               //当前状态没有绑定此事件处理回调,那么就继续向上查找

                return (*E)(*this, *state_node, event_code, Data);//现在终于在父子状态链上找到能够处理此事件的回调了
            }
            return 0;
        }
        //-------------------------------------------------
        //在状态机的当前状态上,尝试迁移到目标状态.
        bool go(uint16_t state_code, bool check_nesting = true)
        {
            if (!m_curr_state)
                return false;                               //状态机未正确初始化.

            if (check_nesting&&m_traning)
                return false;                               //当前处于entry/leave的内部层级迁移过程中

            hsm_state_node_t *state_node = (hsm_state_node_t*)m_state_tree.find_state(state_code);
            if (!state_node)
                return false;                               //目标不存在

            ++m_traning;

            hsm_state_node_t *leave_path[HSM_MAX_LEVELs];   //记录迁移路径中,需要被退出的状态列表
            hsm_state_node_t *entry_path[HSM_MAX_LEVELs];   //记录迁移路径中,需要被进入的状态列表
            uint32_t leave_level_count = 0;                 //需要退出的状态数量
            uint32_t entry_level_count = 0;                 //需要进入的状态数量

            //准备查找源状态和目标状态之间的公共路径
            hsm_state_node_t *src_state_node = m_curr_state;
            hsm_state_node_t *dst_state_node = state_node;

            //先让源和目标向上达到相同的层级
            while (src_state_node->state_level != dst_state_node->state_level)
            {
                if (src_state_node->state_level > dst_state_node->state_level)
                {//源状态层级比较深,需要向上退出
                    leave_path[leave_level_count++] = src_state_node;
                    src_state_node = src_state_node->parent_state;
                }
                else
                {//目标状态层级比较深,需要最终进入
                    entry_path[entry_level_count++] = dst_state_node;
                    dst_state_node = dst_state_node->parent_state;
                }
            }

            //再继续向上,找到源和目标共同的父状态
            while (src_state_node != dst_state_node)
            {
                leave_path[leave_level_count++] = src_state_node;
                src_state_node = src_state_node->parent_state;
                entry_path[entry_level_count++] = dst_state_node;
                dst_state_node = dst_state_node->parent_state;
            }

            //现在,源状态和目标状态之间的路径都已经明确
            for (uint32_t idx = 0; idx < leave_level_count; idx++)
                m_walk_event(leave_path[idx], false,idx);   //从源状态路径上逐层退出

            for (uint32_t idx = 0; idx < entry_level_count; idx++)
            {
                uint32_t i = entry_level_count - idx - 1;
                m_walk_event(entry_path[i], true,i);        //对目标状态路径逐层进入
            }

            //现在记录最终到达的状态
            m_curr_state = state_node;
            --m_traning;
            return true;
        }
    };

    //-----------------------------------------------------
    //真正的层次状态机功能类,实现了初始化与关闭方法.
    class hsm_t :public hsm_core_t
    {
    public:
        hsm_t(hsm_tree_i& state_tree):hsm_core_t(state_tree){}
        //-------------------------------------------------
        //状态树的初始化工作都完成了,现在打开状态机,准备工作了
        bool init(uint16_t init_state_code=HSM_ROOT_STATE,void *usrobj=NULL)
        {
            //查找给定的初始化状态
            hsm_core_t::m_curr_state = hsm_core_t::m_state_tree.find_state(init_state_code);
            if (!hsm_core_t::m_curr_state)
                return false;

            hsm_core_t::m_usrobj=usrobj;

            //给出进入事件
            hsm_core_t::m_walk_event(hsm_core_t::m_curr_state, true, 0);//给出状态进入事件,0代表最终进入而不是路过
            return true;
        }
        //-------------------------------------------------
        //清理全部的信息,关闭状态机.不再继续操作了.
        void uninit()
        {
            hsm_core_t::m_curr_state = NULL;
            hsm_core_t::m_traning = 0;
        }
    };
}


#endif
