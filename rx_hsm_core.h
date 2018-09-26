#ifndef _RX_HSM_BASE_H_
#define _RX_HSM_BASE_H_

#include "rx_cc_macro.h"

    //状态层级的数量上限
    #ifndef HSM_LEVEL_MAXCOUNT
    #define HSM_LEVEL_MAXCOUNT 8
    #endif

    //内置的状态类型与名称
    const uint16_t HSM_ROOT = (uint16_t)-1;
    #define hsm_name_root "HSM_ROOT"

    //内置的事件类型与名称
    const uint16_t hsm_entry = (uint16_t)-2;
    #define hsm_name_entry "HSM_EVENT_ENTRY"
    const uint16_t hsm_exit = (uint16_t)-3;
    #define hsm_name_leave "HSM_EVENT_LEAVE"


namespace rx
{

    //---------------------------------------------------------------
    //定义状态信息结构体.父状态和层级号是非常重要的字段,他们明确了各个状态之间的路径到达关系.
    typedef struct hsm_state_node_t
    {
        hsm_state_node_t   *parent_node;                    //当前状态的父状态
        uint32_t            state_level;                    //当前状态的层级深度
        const char         *state_name;                     //当前状态的名字,方便调试
        void               *usrptr;                         //状态树节点记录的扩展指针,方便外部使用
    }hsm_state_node_t;

    //---------------------------------------------------------------
    //定义事件信息结构体
    typedef struct hsm_state_event_t 
    {
        delegate_def_t  evt_cb;
    }hsm_state_event_t;

    //---------------------------------------------------------------
    //层次状态机使用的状态树接口
    class hsm_tree_i
    {
        friend class hsm_core_t;
    protected:
        hsm_state_node_t    m_root_node;                    //根状态节点
        virtual uint32_t on_state_event_key(uint16_t state_code, uint16_t event_code) 
        { 
            return ((uint32_t)state_code << 16) | event_code; 
        }
    public:
        //-----------------------------------------------------------
        hsm_tree_i() { memset(&m_root_node, 0, sizeof(m_root_node));}
        virtual ~hsm_tree_i(){}
        //根据指定的状态值查找对应的状态节点
        virtual hsm_state_node_t* find_state(uint16_t state_code) = 0;
        //根据指定的状态事件代码,查找对应的事件节点
        virtual hsm_state_event_t* find_state_event(uint16_t state_code,uint16_t event_code) = 0;
    };

    //---------------------------------------------------------------
    //定义状态机依赖的状态树,记录所有状态的层级关系,便于迁移
    template<uint32_t MaxEventCount,uint32_t MaxStateCount,class tree_i=hsm_tree_i>
    class hsm_tree_core_t:public tree_i
    {
        friend class hsm_core_t;

        typedef lt_hash_table<hsm_state_event_t, MaxEventCount> hsm_event_table;
        typedef lt_hash_table<hsm_state_node_t, MaxStateCount> hsm_state_table;

        hsm_state_table        m_state_tbl;                         //状态信息与状态值的哈希表
        hsm_event_table        m_event_tbl;                         //状态事件与处理函数的哈希表

    public:
        hsm_tree_core_t(){}
        //-----------------------------------------------------------
        //根据指定的状态值查找对应的状态节点
        virtual hsm_state_node_t* find_state(uint16_t state_code) { return (hsm_state_node_t*)m_state_tbl.Find(state_code); }
        //根据指定的状态事件代码,查找对应的事件节点
        virtual hsm_state_event_t* find_state_event(uint16_t state_code,uint16_t event_code) 
        { 
            return (hsm_state_event_t*)m_event_tbl.Find(on_state_event_key(state_code,event_code)); 
        }
        //-----------------------------------------------------------
        //清理全部的状态树数据,可以重新构建.
        void clear()
        {
            m_state_tbl.Clear();
            m_event_tbl.Clear();
        }
        //-----------------------------------------------------------
        //创建状态信息,记录该状态的父状态,并计算当前状态的层级level.
        //返回值:<0错误;0状态已经存在了;>0成功,为已经存储的状态数量.
        int make(uint16_t state_code, const char *state_name, uint16_t parent_node = HSM_ROOT, void* usrptr = NULL)
        {
            //找到指定的父节点
            hsm_state_node_t *P;
            if (parent_node == HSM_ROOT)
                P = &m_root_node;
            else
            {//找不到指定的父节点,说明状态树的初始构造顺序错误.
                P = (hsm_state_node_t*)m_state_tbl.Find(parent_node);
                if (!P) return -1;
            }

            //进行层级检查
            uint32_t state_level = P->state_level + 1;
            if (state_level >= HSM_LEVEL_MAXCOUNT)
                return -2;                                  //当前的状态层级已经超过了限定值.


            //获取当前节点
            hsm_state_node_t *S = (hsm_state_node_t*)m_state_tbl.Append(state_code);
            if (!S)
                return -3;                                  //状态哈希表满了.

            if (S->parent_node)
                return 0;                                   //当前状态已经存在了,重复!

            //填充新状态的信息
            S->state_name = state_name;
            S->parent_node = P;
            S->state_level = state_level;
            S->usrptr = usrptr;
            
            return m_state_tbl.size();
        }
        //-----------------------------------------------------------
        //创建状态对应的事件,之后需要查找该状态事件并绑定回调函数.
        //返回值:<0错误;0状态事件已经存在了;>0成功,为已经存储的状态事件数量.
        int make(uint16_t state_code, uint16_t event_code)
        {
            hsm_state_event_t *E = (hsm_state_event_t*)m_event_tbl.Append(on_state_event_key(state_code, event_code));
            if (!E)
                return -1;                                  //状态事件表满了.
            else if (E->evt_cb.IsValid())
                return 0;                                   //状态事件重复(或者是key函数冲突了)
            return m_event_tbl.size();
        }
        //-----------------------------------------------------------
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
    };

    //---------------------------------------------------------------
    //基于状态树进行独立工作的状态机.根据需要,可以生成一个状态树的多个状态机,互相是独立的.
    //这里仅开放HSM的主功能,应用开发的时候,使用子类hsm_machine进行实例化.
    class hsm_core_t
    {
    protected:
        friend class hsm_tree_i;
        hsm_state_node_t   *m_curr_state;                   //指向当前状态信息
        uint32_t            m_traning;                      //状态机迁移标记
        const char         *m_hsm_name;                     //状态机的名字 
        hsm_tree_i         *m_state_tree;                   //指向状态树
        //-----------------------------------------------------------    
        //内部调用,触发进入或离开状态的事件(如果有的话).idx为进入或离开的顺序,0代表最终进入或最初离开,非零代表中间过程.
        uint32_t m_walk_event(hsm_state_node_t *S, bool IsEntry,uint32_t idx)
        {
            if (!S) return false;
            uint32_t RC = -1;
            uint16_t ST = S->Key;
            uint16_t event_code = IsEntry ? hsm_entry : hsm_exit;

            hsm_state_event_t *E = (hsm_state_event_t*)m_state_tree->find_state_event(ST, event_code);
            if (!E || !E->evt_cb.IsValid())
                return RC;                                  //当前状态没有绑定此事件处理回调,那么就继续向上查找

            RC = E->evt_cb.Do(*this, ST, event_code, &idx); //现在找到能够处理此事件的回调了
            if (m_state_tree->m_Log)
                m_state_tree->m_Log->OnTrig(m_hsm_name, *S, *S, event_code, &idx, RC);

            return RC;
        }
    public:
        virtual ~hsm_core_t(){}
        //-----------------------------------------------------------
        //获取当前状态节点的状态代码和绑定的扩展指针
        void* curr_state(uint16_t* state_code=NULL) const
        {
            if (!m_state_tree || !m_curr_state)
                return NULL;
            if (state_code) *state_code = m_curr_state->Key;
            return m_curr_state->usrptr;
        }
        //-----------------------------------------------------------
        //判断状态机的当前状态是否与给定的状态"相容"(直接相同或与给定状态的父状态相同)
        bool is_curr_state(uint16_t state_code) const
        {
            if (!m_state_tree) return false;
            for (hsm_state_node_t *S = m_curr_state; S; S = S->parent_node)
            {
                if (S->Key == state_code)
                    return true;
            }
            return false;
        }
        //-----------------------------------------------------------
        //判断当前是否处于状态迁移过程中
        uint32_t traning()const{return m_traning;}        
        //-----------------------------------------------------------
        //状态机触发给定的事件.在状态机的当前状态或父状态链上,尝试触发给定的事件.
        //返回值:此事件是否被正确处理了,<0错误;0未被正确处理;Event处理完成.
        int hit(uint16_t event_code, void *Data = NULL)
        {
            if (!m_state_tree || !m_curr_state) return false;
            uint32_t RC = -1;

            //从当前状态,一直尝试向上层查找
            hsm_state_node_t *S = m_curr_state;
            for (; S; S = S->parent_node)
            {
                uint16_t ST = S->Key;
                hsm_state_event_t *E = (hsm_state_event_t*)m_state_tree->find_state_event(ST,event_code);
                if (!E || !E->evt_cb.IsValid())
                    continue;                               //当前状态没有绑定此事件处理回调,那么就继续向上查找
                
                RC=E->evt_cb.Do(*this, ST, event_code, Data);//现在终于在父子状态链上找到能够处理此事件的回调了
                break;                                      //循环结束
            }

            if (m_state_tree->m_Log)                                        //尝试给出事件触发记录
                m_state_tree->m_Log->OnTrig(m_hsm_name, *m_curr_state, *S, event_code, Data, RC);

            return RC;
        }
        //-----------------------------------------------------------
        //在状态机的当前状态上,尝试迁移到目标状态.
        int go(uint16_t DstState, bool CheckNesting = true)
        {
            if (!m_state_tree ||!m_curr_state) return false;
            hsm_state_node_t *S = (hsm_state_node_t*)m_state_tree->find_state(DstState);
            if (!S) return -1;                              //目标状态不存在

            if (CheckNesting&&m_traning)
                return -2;                                  //当前处于entry/leave的内部层级迁移过程中

            ++m_traning;

            hsm_state_node_t *leave_path[HSM_LEVEL_MAXCOUNT];             //记录迁移路径中,需要被退出的状态列表
            hsm_state_node_t *entry_path[HSM_LEVEL_MAXCOUNT];            //记录迁移路径中,需要被进入的状态列表
            uint32_t leave_level_count = 0;                               //需要退出的状态数量
            uint32_t entry_level_count = 0;                              //需要进入的状态数量

            //准备查找源状态和目标状态之间的公共路径
            hsm_state_node_t *SrcPathNode = m_curr_state;
            hsm_state_node_t *DstPathNode = S;

            //先让源和目标达到相同的层级
            while (SrcPathNode->state_level != DstPathNode->state_level)
            {
                if (SrcPathNode->state_level > DstPathNode->state_level)
                {//源状态比较深,需要向上退出
                    leave_path[leave_level_count++] = SrcPathNode;
                    SrcPathNode = SrcPathNode->parent_node;
                }
                else
                {//目标状态比较深,需要最终进入
                    entry_path[entry_level_count++] = DstPathNode;
                    DstPathNode = DstPathNode->parent_node;
                }
            }

            //再找到源状态和目标状态相同的父状态
            while (SrcPathNode != DstPathNode)
            {
                leave_path[leave_level_count++] = SrcPathNode;
                SrcPathNode = SrcPathNode->parent_node;
                entry_path[entry_level_count++] = DstPathNode;
                DstPathNode = DstPathNode->parent_node;
            }

            //现在,源状态和目标状态之间的路径都已经明确了
            for (uint32_t Idx = 0; Idx < leave_level_count; Idx++)
            {//从源状态路径上逐层退出
                SrcPathNode = leave_path[Idx];
                m_walk_event(SrcPathNode, false,Idx);
            }

            for (uint32_t Idx = 0; Idx < entry_level_count; Idx++)
            {//对目标状态路径逐层进入
                uint32_t i = entry_level_count - Idx - 1;
                DstPathNode = entry_path[i];
                m_walk_event(DstPathNode, true,i);
            }

            //现在记录最终到达的状态
            m_curr_state = S;
            --m_traning;
            return true;
        }

    };

    //---------------------------------------------------------------
    //真正的层次状态机功能类,实现了初始化与关闭方法.
    class hsm_t :public hsm_core_t
    {
    public:
        //-----------------------------------------------------------
        //状态树的初始化工作都完成了,现在打开状态机,准备工作了
        bool init(hsm_tree_i& state_tree, uint16_t InitState, const char* SMName = NULL)
        {
            Close();
            m_state_tree = &state_tree;
            m_hsm_name = SMName;

            //先进行初始化检查,给出错误提示
            if (m_state_tree.Check())
                return false;

            //查找给定的初始化状态
            hsm_state_node_t *S = (hsm_state_node_t*)m_state_tree->find_state(InitState);
            if (!S)
            {
                m_state_tree->DoLogEx(m_hsm_name, true, "hsm_core_t::Open Error! Not Found state_code Node[%d]!", InitState);
                return false;
            }
            //记录当前状态并给出进入事件
            m_curr_state = S;
            m_walk_event(S, true, 0);
            return true;
        }
        //-----------------------------------------------------------
        //清理全部的信息,关闭状态机.不再继续操作了.
        void uninit()
        {
            m_curr_state = NULL;
            m_traning = false;
            m_hsm_name = NULL;
            m_state_tree = NULL;
        }
    };

}


#endif
