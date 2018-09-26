#ifndef _RX_HSM_BASE_H_
#define _RX_HSM_BASE_H_

#include "rx_cc_macro.h"

    //״̬�㼶����������
    #ifndef HSM_LEVEL_MAXCOUNT
    #define HSM_LEVEL_MAXCOUNT 8
    #endif

    //���õ�״̬����������
    const uint16_t HSM_ROOT = (uint16_t)-1;
    #define hsm_name_root "HSM_ROOT"

    //���õ��¼�����������
    const uint16_t hsm_entry = (uint16_t)-2;
    #define hsm_name_entry "HSM_EVENT_ENTRY"
    const uint16_t hsm_exit = (uint16_t)-3;
    #define hsm_name_leave "HSM_EVENT_LEAVE"


namespace rx
{

    //---------------------------------------------------------------
    //����״̬��Ϣ�ṹ��.��״̬�Ͳ㼶���Ƿǳ���Ҫ���ֶ�,������ȷ�˸���״̬֮���·�������ϵ.
    typedef struct hsm_state_node_t
    {
        hsm_state_node_t   *parent_node;                    //��ǰ״̬�ĸ�״̬
        uint32_t            state_level;                    //��ǰ״̬�Ĳ㼶���
        const char         *state_name;                     //��ǰ״̬������,�������
        void               *usrptr;                         //״̬���ڵ��¼����չָ��,�����ⲿʹ��
    }hsm_state_node_t;

    //---------------------------------------------------------------
    //�����¼���Ϣ�ṹ��
    typedef struct hsm_state_event_t 
    {
        delegate_def_t  evt_cb;
    }hsm_state_event_t;

    //---------------------------------------------------------------
    //���״̬��ʹ�õ�״̬���ӿ�
    class hsm_tree_i
    {
        friend class hsm_core_t;
    protected:
        hsm_state_node_t    m_root_node;                    //��״̬�ڵ�
        virtual uint32_t on_state_event_key(uint16_t state_code, uint16_t event_code) 
        { 
            return ((uint32_t)state_code << 16) | event_code; 
        }
    public:
        //-----------------------------------------------------------
        hsm_tree_i() { memset(&m_root_node, 0, sizeof(m_root_node));}
        virtual ~hsm_tree_i(){}
        //����ָ����״ֵ̬���Ҷ�Ӧ��״̬�ڵ�
        virtual hsm_state_node_t* find_state(uint16_t state_code) = 0;
        //����ָ����״̬�¼�����,���Ҷ�Ӧ���¼��ڵ�
        virtual hsm_state_event_t* find_state_event(uint16_t state_code,uint16_t event_code) = 0;
    };

    //---------------------------------------------------------------
    //����״̬��������״̬��,��¼����״̬�Ĳ㼶��ϵ,����Ǩ��
    template<uint32_t MaxEventCount,uint32_t MaxStateCount,class tree_i=hsm_tree_i>
    class hsm_tree_core_t:public tree_i
    {
        friend class hsm_core_t;

        typedef lt_hash_table<hsm_state_event_t, MaxEventCount> hsm_event_table;
        typedef lt_hash_table<hsm_state_node_t, MaxStateCount> hsm_state_table;

        hsm_state_table        m_state_tbl;                         //״̬��Ϣ��״ֵ̬�Ĺ�ϣ��
        hsm_event_table        m_event_tbl;                         //״̬�¼��봦�����Ĺ�ϣ��

    public:
        hsm_tree_core_t(){}
        //-----------------------------------------------------------
        //����ָ����״ֵ̬���Ҷ�Ӧ��״̬�ڵ�
        virtual hsm_state_node_t* find_state(uint16_t state_code) { return (hsm_state_node_t*)m_state_tbl.Find(state_code); }
        //����ָ����״̬�¼�����,���Ҷ�Ӧ���¼��ڵ�
        virtual hsm_state_event_t* find_state_event(uint16_t state_code,uint16_t event_code) 
        { 
            return (hsm_state_event_t*)m_event_tbl.Find(on_state_event_key(state_code,event_code)); 
        }
        //-----------------------------------------------------------
        //����ȫ����״̬������,�������¹���.
        void clear()
        {
            m_state_tbl.Clear();
            m_event_tbl.Clear();
        }
        //-----------------------------------------------------------
        //����״̬��Ϣ,��¼��״̬�ĸ�״̬,�����㵱ǰ״̬�Ĳ㼶level.
        //����ֵ:<0����;0״̬�Ѿ�������;>0�ɹ�,Ϊ�Ѿ��洢��״̬����.
        int make(uint16_t state_code, const char *state_name, uint16_t parent_node = HSM_ROOT, void* usrptr = NULL)
        {
            //�ҵ�ָ���ĸ��ڵ�
            hsm_state_node_t *P;
            if (parent_node == HSM_ROOT)
                P = &m_root_node;
            else
            {//�Ҳ���ָ���ĸ��ڵ�,˵��״̬���ĳ�ʼ����˳�����.
                P = (hsm_state_node_t*)m_state_tbl.Find(parent_node);
                if (!P) return -1;
            }

            //���в㼶���
            uint32_t state_level = P->state_level + 1;
            if (state_level >= HSM_LEVEL_MAXCOUNT)
                return -2;                                  //��ǰ��״̬�㼶�Ѿ��������޶�ֵ.


            //��ȡ��ǰ�ڵ�
            hsm_state_node_t *S = (hsm_state_node_t*)m_state_tbl.Append(state_code);
            if (!S)
                return -3;                                  //״̬��ϣ������.

            if (S->parent_node)
                return 0;                                   //��ǰ״̬�Ѿ�������,�ظ�!

            //�����״̬����Ϣ
            S->state_name = state_name;
            S->parent_node = P;
            S->state_level = state_level;
            S->usrptr = usrptr;
            
            return m_state_tbl.size();
        }
        //-----------------------------------------------------------
        //����״̬��Ӧ���¼�,֮����Ҫ���Ҹ�״̬�¼����󶨻ص�����.
        //����ֵ:<0����;0״̬�¼��Ѿ�������;>0�ɹ�,Ϊ�Ѿ��洢��״̬�¼�����.
        int make(uint16_t state_code, uint16_t event_code)
        {
            hsm_state_event_t *E = (hsm_state_event_t*)m_event_tbl.Append(on_state_event_key(state_code, event_code));
            if (!E)
                return -1;                                  //״̬�¼�������.
            else if (E->evt_cb.IsValid())
                return 0;                                   //״̬�¼��ظ�(������key������ͻ��)
            return m_event_tbl.size();
        }
        //-----------------------------------------------------------
        //���Լ���ʼ���Ƿ���ȷ���
        //����ֵ:<0����;0����;>0Ϊ��ϣ���ͻ����(����:������ϣ������/����״̬���¼��Ĵ���/������ϣ����).
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
    //����״̬�����ж���������״̬��.������Ҫ,��������һ��״̬���Ķ��״̬��,�����Ƕ�����.
    //���������HSM��������,Ӧ�ÿ�����ʱ��,ʹ������hsm_machine����ʵ����.
    class hsm_core_t
    {
    protected:
        friend class hsm_tree_i;
        hsm_state_node_t   *m_curr_state;                   //ָ��ǰ״̬��Ϣ
        uint32_t            m_traning;                      //״̬��Ǩ�Ʊ��
        const char         *m_hsm_name;                     //״̬�������� 
        hsm_tree_i         *m_state_tree;                   //ָ��״̬��
        //-----------------------------------------------------------    
        //�ڲ�����,����������뿪״̬���¼�(����еĻ�).idxΪ������뿪��˳��,0�������ս��������뿪,��������м����.
        uint32_t m_walk_event(hsm_state_node_t *S, bool IsEntry,uint32_t idx)
        {
            if (!S) return false;
            uint32_t RC = -1;
            uint16_t ST = S->Key;
            uint16_t event_code = IsEntry ? hsm_entry : hsm_exit;

            hsm_state_event_t *E = (hsm_state_event_t*)m_state_tree->find_state_event(ST, event_code);
            if (!E || !E->evt_cb.IsValid())
                return RC;                                  //��ǰ״̬û�а󶨴��¼�����ص�,��ô�ͼ������ϲ���

            RC = E->evt_cb.Do(*this, ST, event_code, &idx); //�����ҵ��ܹ�������¼��Ļص���
            if (m_state_tree->m_Log)
                m_state_tree->m_Log->OnTrig(m_hsm_name, *S, *S, event_code, &idx, RC);

            return RC;
        }
    public:
        virtual ~hsm_core_t(){}
        //-----------------------------------------------------------
        //��ȡ��ǰ״̬�ڵ��״̬����Ͱ󶨵���չָ��
        void* curr_state(uint16_t* state_code=NULL) const
        {
            if (!m_state_tree || !m_curr_state)
                return NULL;
            if (state_code) *state_code = m_curr_state->Key;
            return m_curr_state->usrptr;
        }
        //-----------------------------------------------------------
        //�ж�״̬���ĵ�ǰ״̬�Ƿ��������״̬"����"(ֱ����ͬ�������״̬�ĸ�״̬��ͬ)
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
        //�жϵ�ǰ�Ƿ���״̬Ǩ�ƹ�����
        uint32_t traning()const{return m_traning;}        
        //-----------------------------------------------------------
        //״̬�������������¼�.��״̬���ĵ�ǰ״̬��״̬����,���Դ����������¼�.
        //����ֵ:���¼��Ƿ���ȷ������,<0����;0δ����ȷ����;Event�������.
        int hit(uint16_t event_code, void *Data = NULL)
        {
            if (!m_state_tree || !m_curr_state) return false;
            uint32_t RC = -1;

            //�ӵ�ǰ״̬,һֱ�������ϲ����
            hsm_state_node_t *S = m_curr_state;
            for (; S; S = S->parent_node)
            {
                uint16_t ST = S->Key;
                hsm_state_event_t *E = (hsm_state_event_t*)m_state_tree->find_state_event(ST,event_code);
                if (!E || !E->evt_cb.IsValid())
                    continue;                               //��ǰ״̬û�а󶨴��¼�����ص�,��ô�ͼ������ϲ���
                
                RC=E->evt_cb.Do(*this, ST, event_code, Data);//���������ڸ���״̬�����ҵ��ܹ�������¼��Ļص���
                break;                                      //ѭ������
            }

            if (m_state_tree->m_Log)                                        //���Ը����¼�������¼
                m_state_tree->m_Log->OnTrig(m_hsm_name, *m_curr_state, *S, event_code, Data, RC);

            return RC;
        }
        //-----------------------------------------------------------
        //��״̬���ĵ�ǰ״̬��,����Ǩ�Ƶ�Ŀ��״̬.
        int go(uint16_t DstState, bool CheckNesting = true)
        {
            if (!m_state_tree ||!m_curr_state) return false;
            hsm_state_node_t *S = (hsm_state_node_t*)m_state_tree->find_state(DstState);
            if (!S) return -1;                              //Ŀ��״̬������

            if (CheckNesting&&m_traning)
                return -2;                                  //��ǰ����entry/leave���ڲ��㼶Ǩ�ƹ�����

            ++m_traning;

            hsm_state_node_t *leave_path[HSM_LEVEL_MAXCOUNT];             //��¼Ǩ��·����,��Ҫ���˳���״̬�б�
            hsm_state_node_t *entry_path[HSM_LEVEL_MAXCOUNT];            //��¼Ǩ��·����,��Ҫ�������״̬�б�
            uint32_t leave_level_count = 0;                               //��Ҫ�˳���״̬����
            uint32_t entry_level_count = 0;                              //��Ҫ�����״̬����

            //׼������Դ״̬��Ŀ��״̬֮��Ĺ���·��
            hsm_state_node_t *SrcPathNode = m_curr_state;
            hsm_state_node_t *DstPathNode = S;

            //����Դ��Ŀ��ﵽ��ͬ�Ĳ㼶
            while (SrcPathNode->state_level != DstPathNode->state_level)
            {
                if (SrcPathNode->state_level > DstPathNode->state_level)
                {//Դ״̬�Ƚ���,��Ҫ�����˳�
                    leave_path[leave_level_count++] = SrcPathNode;
                    SrcPathNode = SrcPathNode->parent_node;
                }
                else
                {//Ŀ��״̬�Ƚ���,��Ҫ���ս���
                    entry_path[entry_level_count++] = DstPathNode;
                    DstPathNode = DstPathNode->parent_node;
                }
            }

            //���ҵ�Դ״̬��Ŀ��״̬��ͬ�ĸ�״̬
            while (SrcPathNode != DstPathNode)
            {
                leave_path[leave_level_count++] = SrcPathNode;
                SrcPathNode = SrcPathNode->parent_node;
                entry_path[entry_level_count++] = DstPathNode;
                DstPathNode = DstPathNode->parent_node;
            }

            //����,Դ״̬��Ŀ��״̬֮���·�����Ѿ���ȷ��
            for (uint32_t Idx = 0; Idx < leave_level_count; Idx++)
            {//��Դ״̬·��������˳�
                SrcPathNode = leave_path[Idx];
                m_walk_event(SrcPathNode, false,Idx);
            }

            for (uint32_t Idx = 0; Idx < entry_level_count; Idx++)
            {//��Ŀ��״̬·��������
                uint32_t i = entry_level_count - Idx - 1;
                DstPathNode = entry_path[i];
                m_walk_event(DstPathNode, true,i);
            }

            //���ڼ�¼���յ����״̬
            m_curr_state = S;
            --m_traning;
            return true;
        }

    };

    //---------------------------------------------------------------
    //�����Ĳ��״̬��������,ʵ���˳�ʼ����رշ���.
    class hsm_t :public hsm_core_t
    {
    public:
        //-----------------------------------------------------------
        //״̬���ĳ�ʼ�������������,���ڴ�״̬��,׼��������
        bool init(hsm_tree_i& state_tree, uint16_t InitState, const char* SMName = NULL)
        {
            Close();
            m_state_tree = &state_tree;
            m_hsm_name = SMName;

            //�Ƚ��г�ʼ�����,����������ʾ
            if (m_state_tree.Check())
                return false;

            //���Ҹ����ĳ�ʼ��״̬
            hsm_state_node_t *S = (hsm_state_node_t*)m_state_tree->find_state(InitState);
            if (!S)
            {
                m_state_tree->DoLogEx(m_hsm_name, true, "hsm_core_t::Open Error! Not Found state_code Node[%d]!", InitState);
                return false;
            }
            //��¼��ǰ״̬�����������¼�
            m_curr_state = S;
            m_walk_event(S, true, 0);
            return true;
        }
        //-----------------------------------------------------------
        //����ȫ������Ϣ,�ر�״̬��.���ټ���������.
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
