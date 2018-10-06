#ifndef _RX_HSM_BASE_H_
#define _RX_HSM_BASE_H_

#include "rx_cc_macro.h"
#include "rx_hash_int.h"
#include "rx_dtl_hashtbl_raw.h"
#include "rx_ct_util.h"
#include "rx_ct_delegate.h"

/*
    //HSM״̬�¼�������
    class hsm_tst_handle_t
    {
    public:
        uint16_t on_event(hsm_core_t &hsm, const hsm_tree_i::state_node_t &state_node, uint16_t event_code, void *usrdat)
        {
            return event_code;
        }
        void on_entry(hsm_core_t &hsm, const hsm_tree_i::state_node_t &state_node, uint16_t event_code, uint16_t passed_on)
        {
        }
        void on_leave(hsm_core_t &hsm, const hsm_tree_i::state_node_t &state_node, uint16_t event_code, uint16_t passed_on)
        {
        }
    };
*/

namespace rx
{
    //���״̬�����Ĳ���
#ifndef HSM_MAX_LEVELs
    #define HSM_MAX_LEVELs    8
#endif
    class hsm_core_t;

    //-----------------------------------------------------
    //���״̬��ʹ�õ�״̬���ӿ�
    //-----------------------------------------------------
    class hsm_tree_i
    {
    public:
        //-----------------------------------------------------
        //����״̬��Ϣ�ṹ��.��״̬�Ͳ㼶���Ƿǳ���Ҫ���ֶ�,������ȷ�˸���״̬֮���·�������ϵ.
        typedef struct state_node_t
        {
            state_node_t       *parent_state;               //��ǰ״̬�ĸ�״̬
            void               *usrptr;                     //״̬���ڵ��¼����չָ��,�����ⲿʹ��
            uint16_t            state_level;                //��ǰ״̬�Ĳ㼶���
            uint16_t            code;                       //��ǰ״̬Ψһ��ʶ(���ڹ�ϣ����key)
        }state_node_t;

        //-----------------------------------------------------
        //ҵ���¼��ص�ί�����Ͷ���: uint16_t on_event(hsm_core_t &hsm, const hsm_tree_i::state_node_t &state_node, uint16_t event_code, void *usrdat)
        typedef delegate4_rt<hsm_core_t&, const state_node_t&, uint16_t, void*, uint16_t> evnt_delegate_t;

        //״̬�����¼��ص�ί�����Ͷ���: void on_entry(hsm_core_t &hsm, const hsm_tree_i::state_node_t &state_node, uint16_t event_code, uint16_t passed_on)
        typedef delegate4_rt<hsm_core_t&, const state_node_t&, uint16_t, uint16_t, void> over_delegate_t;

        //-----------------------------------------------------
        //�����¼���Ϣ�ṹ��
        typedef struct hsm_state_event_t
        {
            union {
                evnt_delegate_t  evnt_cb;                   //״̬ҵ���¼���Ӧ�Ļص�����
                over_delegate_t  over_cb;                   //״̬�����¼���Ӧ�Ļص�����
            };
            uint32_t             code;                      //״̬�¼���Ψһ��ʶ(���ڹ�ϣ����key)
        }hsm_state_event_t;

        //-----------------------------------------------------
        //ƴװ״̬�����¼��벢�����key�ĺ���
        typedef uint32_t(*hsm_st_ev_key_t)(uint16_t state_code, uint16_t event_code);
        inline static uint32_t hsm_st_ev_key(uint16_t state_code, uint16_t event_code)
        {
            return ((uint32_t)state_code << 16) | event_code;
        }
        //���õ��¼�����(�ⲿ��Ҫ�ظ�):״̬�����¼�
        static const uint16_t HSM_EVENT_ENTRY = (uint16_t)-1;
        //���õ��¼�����(�ⲿ��Ҫ�ظ�):״̬�뿪�¼�
        static const uint16_t HSM_EVENT_LEAVE = (uint16_t)-2;
    public:
        hsm_st_ev_key_t     state_event_key_fun;            //ƴװ״̬�¼�key�ĺ���
        rx_int_hash32_t     event_hashkey_fun;              //���ڼ����¼���ϣ������������ϣ����
        rx_int_hash32_t     state_hashkey_fun;              //���ڼ���״̬��ϣ������������ϣ����
        //-------------------------------------------------
        hsm_tree_i()
        {
            event_hashkey_fun=rx_hash_skeeto_3s;
            state_hashkey_fun=rx_hash_skeeto_3s;
            state_event_key_fun=hsm_st_ev_key;
            reset();
        }
        virtual ~hsm_tree_i(){}
        //-------------------------------------------------
        //����ָ����״ֵ̬���Ҷ�Ӧ��״̬�ڵ�
        virtual state_node_t*   find_state(uint16_t state_code) = 0;
        //����ָ����״̬�¼�����,���Ҷ�Ӧ���¼��ڵ�
        virtual evnt_delegate_t* find_event(uint16_t state_code,uint16_t event_code) = 0;
        //����ָ��״̬�ϵĽ����¼�
        virtual over_delegate_t* find_over(uint16_t state_code, bool is_entry) = 0;
    protected:
        //-----------------------------------------------------
        //���õ�״̬����:��״̬
        static const uint16_t HSM_ROOT_STATE = (uint16_t)-1;
        //��״̬�ڵ�
        state_node_t        m_root_state;
        //-----------------------------------------------------
        void reset()
        {
            m_root_state.usrptr = NULL;
            m_root_state.parent_state = NULL;
            m_root_state.state_level = 0;
            m_root_state.code = HSM_ROOT_STATE;
        }
    };

    //-----------------------------------------------------
    //����״̬��������״̬��,��¼����״̬�Ĳ㼶��ϵ(һ��״̬�����Ա����HSM״̬������).
    //-----------------------------------------------------
    template<uint32_t max_state_count,uint32_t max_event_count>
    class hsm_tree_t:public hsm_tree_i
    {
    protected:
        //�򵥹�ϣ��ʹ�õĽڵ�Ƚ���
        class cmp_t
        {
        public:
            //�Խڵ�ȽϺ������и���,��Ҫ����ʹ�ýڵ��е�key�ֶ��������kֵ���бȽ�
            template<class NVT, class KT>
            static bool equ(const NVT &n, const KT &k) { return n.code == k; }
        };

        typedef raw_hashtbl_t<state_node_t, cmp_t> hsm_state_table_t;
        typedef raw_hashtbl_t<hsm_state_event_t, cmp_t> hsm_event_table_t;

        typename hsm_state_table_t::node_t m_states[max_state_count];
        typename hsm_event_table_t::node_t m_events[max_event_count];

        hsm_state_table_t        m_state_tbl;               //״̬��Ϣ��״ֵ̬�Ĺ�ϣ��
        hsm_event_table_t        m_event_tbl;               //״̬�¼��봦�������Ĺ�ϣ��
    public:
        //-------------------------------------------------
        hsm_tree_t()
        {
            m_state_tbl.bind(m_states,max_state_count);
            m_event_tbl.bind(m_events,max_event_count);
        }
        //-------------------------------------------------
        //����ָ����״ֵ̬���Ҷ�Ӧ��״̬�ڵ�
        state_node_t* find_state(uint16_t state_code)
        {
            uint32_t pos;
            typename hsm_state_table_t::node_t *node=m_state_tbl.find(hsm_tree_i::state_hashkey_fun(state_code),state_code,pos);
            return node?&node->value:NULL;
        }
        //-------------------------------------------------
        //����ָ����״̬�¼�����,���Ҷ�Ӧ���¼��ڵ�
        evnt_delegate_t* find_event(uint16_t state_code,uint16_t event_code)
        {
            uint32_t svkey=hsm_tree_i::state_event_key_fun(state_code,event_code);
            typename hsm_event_table_t::node_t *node=m_event_tbl.find(hsm_tree_i::event_hashkey_fun(svkey),svkey);
            return node?&node->value.evnt_cb:NULL;
        }
        //-------------------------------------------------
        //����ָ��״̬�ϵĽ����¼�
        over_delegate_t* find_over(uint16_t state_code, bool is_entry)
        {
            uint16_t event_code = is_entry ? HSM_EVENT_ENTRY : HSM_EVENT_LEAVE;
            uint32_t svkey = hsm_tree_i::state_event_key_fun(state_code, event_code);
            typename hsm_event_table_t::node_t *node = m_event_tbl.find(hsm_tree_i::event_hashkey_fun(svkey), svkey);
            return node ? &node->value.over_cb : NULL;
        }
        //-------------------------------------------------
        //����״̬��Ϣ,��¼��״̬�ĸ�״̬,�����㵱ǰ״̬�Ĳ㼶level.
        //����ֵ:<0����;0״̬�Ѿ�������;>0�ɹ�,Ϊ�Ѿ��洢��״̬����.
        int make_state(uint16_t state_code, uint16_t parent_state , void* usrptr = NULL)
        {
            if (state_code==parent_state||state_code==HSM_ROOT_STATE)
                return -4;

            //�ҵ�ָ���ĸ��ڵ�
            state_node_t *parent;
            if (parent_state == HSM_ROOT_STATE)
                parent = &(hsm_tree_i::m_root_state);
            else
            {//�Ҳ���ָ���ĸ��ڵ�,˵��״̬���ĳ�ʼ����˳�����.
                parent = find_state(parent_state);
                if (!parent)
                    return -1;
            }

            //���в㼶���
            uint32_t state_level = parent->state_level + 1;
            if (state_level >= HSM_MAX_LEVELs)
                return -2;                                  //��ǰ��״̬�㼶�Ѿ��������޶�ֵ.


            //�����½ڵ�
            uint32_t hashcode=hsm_tree_i::state_hashkey_fun(state_code);
            typename hsm_state_table_t::node_t *tbl_node = m_state_tbl.push(hashcode,state_code);
            if (!tbl_node)
                return -3;                                  //״̬��ϣ������.
            state_node_t &state_node = tbl_node->value;

            if (state_node.parent_state)
                return 0;                                   //��ǰ״̬�Ѿ�������,�ظ�!

            //�����״̬����Ϣ
            state_node.parent_state = parent;
            state_node.state_level = state_level;
            state_node.usrptr = usrptr;
            state_node.code=state_code;

            return m_state_tbl.size();
        }
        //-------------------------------------------------
        //����һ��״̬,Ĭ���丸״̬Ϊ��.
        int make_state(uint16_t state_code, void* usrptr = NULL) { return make_state(state_code, HSM_ROOT_STATE,usrptr); }
        //-------------------------------------------------
        //����״̬��Ӧ���¼�,֮����Ҫ���Ҹ�״̬�¼����󶨻ص�����.
        //����ֵ:<0����;0״̬�¼��Ѿ�������;>0�ɹ�,Ϊ�Ѿ��洢��״̬�¼�����.
        int make_event(uint16_t state_code, uint16_t event_code)
        {
            uint32_t evt_key=hsm_tree_i::state_event_key_fun(state_code, event_code);
            uint32_t hashcode=hsm_tree_i::state_hashkey_fun(evt_key);
            uint32_t pos;
            bool is_dup=false;
            typename hsm_event_table_t::node_t *tbl_node = m_event_tbl.push(hashcode,evt_key,pos,&is_dup);
            if (!tbl_node)
                return -1;                                  //״̬�¼�������.
            if (is_dup)
                return 0;                                   //״̬�¼��ظ�(������key������ͻ��)
            hsm_state_event_t &state_event = tbl_node->value;
            state_event.evnt_cb.reset();
            state_event.code=evt_key;
            return m_event_tbl.size();
        }
        //-------------------------------------------------
        //����״̬��Ӧ�Ľ����뿪�¼�
        //����ֵ:<0����;0״̬�¼��Ѿ�������;>0�ɹ�,Ϊ�Ѿ��洢��״̬�¼�����.
        int make_over(uint16_t state_code, bool is_entry)
        {
            uint16_t ec = is_entry ? HSM_EVENT_ENTRY : HSM_EVENT_LEAVE;
            return make_event(state_code,ec);
        }
        //-------------------------------------------------
        //���Լ���ʼ���Ƿ���ȷ���
        //����ֵ:<0����;0����;>0Ϊ��ϣ����ͻ����(����:������ϣ������/����״̬���¼��Ĵ���/������ϣ����).
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
        //����ȫ����״̬������,�������¹���.
        void clear()
        {
            m_state_tbl.clear();
            m_event_tbl.clear();
            reset();
            memset(m_states,0, sizeof(m_states));
            memset(m_events,0, sizeof(m_events));
        }
    };

    //-----------------------------------------------------
    //����״̬�����ж���������״̬��.������Ҫ,��������һ��״̬���Ķ��״̬��,�����Ƕ�����.
    //���������HSM��������,Ӧ��ʱʹ������hsm_t����ʵ����.
    //-----------------------------------------------------
    class hsm_core_t
    {
    protected:
        friend class hsm_tree_i;
        hsm_tree_i::state_node_t   *m_curr_state;           //ָ��ǰ״̬��Ϣ
        hsm_tree_i                 &m_state_tree;           //ָ��״̬��
        uint32_t                    m_traning;              //״̬��Ǩ�Ʊ��
        void                       *m_usrobj;               //�ⲿ�����û�����ָ��
        //-------------------------------------------------
        //�ڲ�����,����������뿪״̬���¼�(����еĻ�).idxΪ������뿪��˳��,0�������ս��������뿪,��������м����.
        //����ֵ:�Ƿ񴥷����¼��ص�����
        void m_walk_event(hsm_tree_i::state_node_t *state_node, bool is_entry,uint16_t idx)
        {
            uint16_t state_code = state_node->code;
            uint16_t event_code = is_entry ? hsm_tree_i::HSM_EVENT_ENTRY : hsm_tree_i::HSM_EVENT_LEAVE;

            hsm_tree_i::over_delegate_t *E = m_state_tree.find_over(state_code, is_entry);
            if (!E || !E->is_valid())
                return;                                     //��ǰ״̬û�а󶨴��¼������ص�,��ô�ͼ������ϲ���

            (*E)(*this, *state_node, event_code, idx);      //�����ҵ��ܹ��������¼��Ļص���
        }
        virtual ~hsm_core_t(){}
    public:
        hsm_core_t(hsm_tree_i& state_tree):m_curr_state(NULL),m_state_tree(state_tree),m_traning(0),m_usrobj(NULL){}
        //-------------------------------------------------
        //��ȡ�󶨵��û���������
        void *usrobj(){return m_usrobj;}
        //-------------------------------------------------
        //��ȡ��ǰ״̬�ڵ��״̬����Ͱ󶨵���չָ��
        hsm_tree_i::state_node_t* curr_state() const { return m_curr_state; }
        //-------------------------------------------------
        //�ж�״̬���ĵ�ǰ״̬�Ƿ��������״̬"����"(ֱ����ͬ�������״̬�ĸ�״̬��ͬ)
        bool is_compatible(uint16_t state_code) const
        {
            for (hsm_tree_i::state_node_t *state_node = m_curr_state; state_node; state_node = state_node->parent_state)
            {
                if (state_node->code == state_code)
                    return true;
            }
            return false;
        }
        //-------------------------------------------------
        //�жϵ�ǰ�Ƿ���״̬Ǩ�ƹ�����
        uint32_t going()const{return m_traning;}
        //-------------------------------------------------
        //״̬�������������¼�.��״̬���ĵ�ǰ״̬��״̬����,���Դ����������¼�.
        //����ֵ:0δ����ȷ����;����Ϊ�¼������ص�����ֵ.
        int hit(uint16_t event_code, void *Data = NULL, hsm_tree_i::state_node_t* *hit_state=NULL)
        {
            //�ӵ�ǰ״̬,һֱ�������ϲ����
            for (hsm_tree_i::state_node_t *state_node = m_curr_state; state_node; state_node = state_node->parent_state)
            {
                uint16_t state_code = state_node->code;
                hsm_tree_i::evnt_delegate_t *E = m_state_tree.find_event(state_code,event_code);
                if (!E || !E->is_valid())
                    continue;                               //��ǰ״̬û�а󶨴��¼������ص�,��ô�ͼ������ϲ���

                if (hit_state) *hit_state = state_node;     //�����Ҫ,���¼���������¼���״̬�ڵ�
                return (*E)(*this, *state_node, event_code, Data);//���������ڸ���״̬�����ҵ��ܹ��������¼��Ļص���
            }
            return 0;
        }
        //-------------------------------------------------
        //��״̬���ĵ�ǰ״̬��,����Ǩ�Ƶ�Ŀ��״̬.
        bool go(uint16_t state_code, bool check_nesting = true)
        {
            if (!m_curr_state)
                return false;                               //״̬��δ��ȷ��ʼ��.

            if (check_nesting&&m_traning)
                return false;                               //��ǰ����entry/leave���ڲ��㼶Ǩ�ƹ�����

            hsm_tree_i::state_node_t *state_node = (hsm_tree_i::state_node_t*)m_state_tree.find_state(state_code);
            if (!state_node)
                return false;                               //Ŀ�겻����

            ++m_traning;

            hsm_tree_i::state_node_t *leave_path[HSM_MAX_LEVELs];   //��¼Ǩ��·����,��Ҫ���˳���״̬�б�
            hsm_tree_i::state_node_t *entry_path[HSM_MAX_LEVELs];   //��¼Ǩ��·����,��Ҫ�������״̬�б�
            uint32_t leave_level_count = 0;                 //��Ҫ�˳���״̬����
            uint32_t entry_level_count = 0;                 //��Ҫ�����״̬����

            //׼������Դ״̬��Ŀ��״̬֮��Ĺ���·��
            hsm_tree_i::state_node_t *src_state_node = m_curr_state;
            hsm_tree_i::state_node_t *dst_state_node = state_node;

            //����Դ��Ŀ�����ϴﵽ��ͬ�Ĳ㼶
            while (src_state_node->state_level != dst_state_node->state_level)
            {
                if (src_state_node->state_level > dst_state_node->state_level)
                {//Դ״̬�㼶�Ƚ���,��Ҫ�����˳�
                    leave_path[leave_level_count++] = src_state_node;
                    src_state_node = src_state_node->parent_state;
                }
                else
                {//Ŀ��״̬�㼶�Ƚ���,��Ҫ���ս���
                    entry_path[entry_level_count++] = dst_state_node;
                    dst_state_node = dst_state_node->parent_state;
                }
            }

            //�ټ�������,�ҵ�Դ��Ŀ�깲ͬ�ĸ�״̬
            while (src_state_node != dst_state_node)
            {
                leave_path[leave_level_count++] = src_state_node;
                src_state_node = src_state_node->parent_state;
                entry_path[entry_level_count++] = dst_state_node;
                dst_state_node = dst_state_node->parent_state;
            }

            //����,Դ״̬��Ŀ��״̬֮���·�����Ѿ���ȷ
            for (uint16_t idx = 0; idx < leave_level_count; idx++)
                m_walk_event(leave_path[idx], false,idx);   //��Դ״̬·��������˳�

            for (uint16_t idx = 0; idx < entry_level_count; idx++)
            {
                uint32_t i = entry_level_count - idx - 1;
                m_walk_event(entry_path[i], true,i);        //��Ŀ��״̬·��������
            }

            //���ڼ�¼���յ����״̬
            m_curr_state = state_node;
            --m_traning;
            return true;
        }
    };

    //-----------------------------------------------------
    //�����Ĳ��״̬��������,ʵ���˳�ʼ����رշ���.
    //-----------------------------------------------------
    class hsm_t :public hsm_core_t
    {
    public:
        hsm_t(hsm_tree_i& state_tree):hsm_core_t(state_tree){}
        //-------------------------------------------------
        //״̬���ĳ�ʼ�������������,���ڴ�״̬��,׼��������
        bool init(uint16_t init_state_code,void *usrobj=NULL)
        {
            //���Ҹ����ĳ�ʼ��״̬
            hsm_core_t::m_curr_state = hsm_core_t::m_state_tree.find_state(init_state_code);
            if (!hsm_core_t::m_curr_state)
                return false;

            hsm_core_t::m_usrobj=usrobj;

            //���������¼�
            hsm_core_t::m_walk_event(hsm_core_t::m_curr_state, true, 0);//����״̬�����¼�,0�������ս��������·��
            return true;
        }
        //-------------------------------------------------
        //����ȫ������Ϣ,�ر�״̬��.���ټ���������.
        void uninit()
        {
            hsm_core_t::m_curr_state = NULL;
            hsm_core_t::m_traning = 0;
        }
    };

    //-----------------------------------------------------
    //��װ״̬��������,�����ʼ������������״̬��
    //-----------------------------------------------------
    template<class tree_t>
    class hsm_tree_maker
    {
        tree_t      &m_tree;
        uint16_t    m_last_state_code;
    public:
        hsm_tree_maker(tree_t& t) :m_tree(t){}
        //-------------------------------------------------
        //�������ĸ�״̬,������״̬�ڵ�
        hsm_tree_maker& state(uint16_t state_code, void* usrptr = NULL)
        {
            int rc = m_tree.make_state(state_code, usrptr);
            rx_fail(rc > 0);
            m_last_state_code = state_code;
            return *this;
        }
        //��ָ���ĸ�״̬��������״̬,����¼���ĸ�״̬
        hsm_tree_maker& state(uint16_t state_code,uint16_t parent, void* usrptr = NULL)
        {
            int rc = m_tree.make_state(state_code, parent, usrptr);
            rx_fail(rc > 0);
            m_last_state_code = state_code;
            return *this;
        }
        //-------------------------------------------------
        //��״̬�ڵ�󶨽����¼�
        template<class T>
        hsm_tree_maker& entry(T& owner, void (T::*entry_func)(hsm_core_t &hsm, const hsm_tree_i::state_node_t &state_node, uint16_t event_code, uint16_t passed_on))
        {
            int rc = m_tree.make_over(m_last_state_code, true);
            rx_fail(rc > 0);
            hsm_tree_i::over_delegate_t *dg= m_tree.find_over(m_last_state_code, true);
            rx_fail(dg!=NULL);
            dg->bind(owner, entry_func);
            return *this;
        }
        hsm_tree_maker& entry(hsm_tree_i::over_delegate_t::cb_func_t cf, void* obj = NULL)
        {
            int rc = m_tree.make_over(m_last_state_code, true);
            rx_fail(rc > 0);
            hsm_tree_i::over_delegate_t *dg = m_tree.find_over(m_last_state_code, true);
            rx_fail(dg != NULL);
            dg->bind(cf, obj);
            return *this;
        }
        //-------------------------------------------------
        //��״̬�ڵ���뿪�¼�
        template<class T>
        hsm_tree_maker& leave(T& owner, void (T::*leave_func)(hsm_core_t &hsm, const hsm_tree_i::state_node_t &state_node, uint16_t event_code, uint16_t passed_on))
        {
            int rc = m_tree.make_over(m_last_state_code, false);
            rx_fail(rc > 0);
            hsm_tree_i::over_delegate_t *dg = m_tree.find_over(m_last_state_code, false);
            rx_fail(dg != NULL);
            dg->bind(owner, leave_func);
            return *this;
        }
        hsm_tree_maker& leave(hsm_tree_i::over_delegate_t::cb_func_t cf, void* obj = NULL)
        {
            int rc = m_tree.make_over(m_last_state_code, false);
            rx_fail(rc > 0);
            hsm_tree_i::over_delegate_t *dg = m_tree.find_over(m_last_state_code, false);
            rx_fail(dg != NULL);
            dg->bind(cf, obj);
            return *this;
        }
        //-------------------------------------------------
        //��״̬�ڵ�󶨹����¼�
        template<class T>
        hsm_tree_maker& event(uint16_t event_code,T& owner, uint16_t (T::*event_func)(hsm_core_t &hsm, const hsm_tree_i::state_node_t &state_node, uint16_t event_code, void *usrdat))
        {
            int rc = m_tree.make_event(m_last_state_code, event_code);
            rx_fail(rc > 0);
            hsm_tree_i::evnt_delegate_t *dg = m_tree.find_event(m_last_state_code, event_code);
            rx_fail(dg != NULL);
            dg->bind(owner, event_func);
            return *this;
        }
        hsm_tree_maker& event(uint16_t event_code, hsm_tree_i::evnt_delegate_t::cb_func_t cf, void* obj = NULL)
        {
            int rc = m_tree.make_event(m_last_state_code, event_code);
            rx_fail(rc > 0);
            hsm_tree_i::evnt_delegate_t *dg = m_tree.find_event(m_last_state_code, event_code);
            rx_fail(dg != NULL);
            dg->bind(cf, obj);
            return *this;
        }
    };

    //-----------------------------------------------------
    //�﷨��,״̬����չ������,���԰󶨴�����ʵ�ֶ����򻯵��ò���
    template<class tree_t,class handler_t>
    class hsm_maker
    {
        typedef hsm_tree_maker<tree_t> maker_t;
        maker_t         m_maker;
        handler_t      &m_handler;
        uint16_t        m_last_parent_code;
    public:
        //-------------------------------------------------
        hsm_maker(tree_t& t, handler_t &h):m_maker(t),m_handler(h), m_last_parent_code(-1){}
        //-------------------------------------------------
        //�������ĸ�״̬,������״̬�ڵ�
        hsm_maker& state(uint16_t state_code, void* usrptr = NULL)
        {
            if (m_last_parent_code == -1)
                m_maker.state(state_code, usrptr);
            else
                m_maker.state(state_code, m_last_parent_code, usrptr);
            return *this;
        }
        //��ָ���ĸ�״̬��������״̬,����¼���ĸ�״̬
        hsm_maker& state(uint16_t state_code, uint16_t parent, void* usrptr = NULL)
        {
            m_last_parent_code = parent;
            m_maker.state(state_code, parent, usrptr);
            return *this;
        }
        //-------------------------------------------------
        //��״̬�ڵ�󶨽����¼�
        hsm_maker& entry(void (handler_t::*entry_func)(hsm_core_t &hsm, const hsm_tree_i::state_node_t &state_node, uint16_t event_code, uint16_t passed_on))
        {
            m_maker.entry(m_handler, entry_func);
            return *this;
        }
        //-------------------------------------------------
        //��״̬�ڵ���뿪�¼�
        hsm_maker& leave(void (handler_t::*leave_func)(hsm_core_t &hsm, const hsm_tree_i::state_node_t &state_node, uint16_t event_code, uint16_t passed_on))
        {
            m_maker.leave(m_handler, leave_func);
            return *this;
        }
        //-------------------------------------------------
        //��״̬�ڵ�󶨹����¼�
        hsm_maker& event(uint16_t event_code, uint16_t(handler_t::*event_func)(hsm_core_t &hsm, const hsm_tree_i::state_node_t &state_node, uint16_t event_code, void *usrdat))
        {
            m_maker.event(event_code, m_handler,event_func);
            return *this;
        }
    };
}


#endif