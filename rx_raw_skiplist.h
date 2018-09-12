#ifndef _RX_RAW_SKIPLIST_H_
#define _RX_RAW_SKIPLIST_H_

#include "rx_cc_macro.h"
#include "rx_mem_alloc.h"

namespace rx
{
    //�Ƿ���ԭʼ����ĵ��Դ�ӡ����
    #define RX_RAW_SKIPLIST_DEBUG_PRINT 0
    /*
    //ԭʼ����Ľڵ�����(ʾ��,ֻҪ����ӿ�������ȷ����)
    template<class key_t>
    struct tiny_skipset_node_t
    {
        //-------------------------------------------------
        //�ȽϽڵ������key�Ĵ�С
        //����ֵ:n<keyΪ<0;n==keyΪ0;n>keyΪ>0
        template<class KT>
        static int cmp(const tiny_skipset_node_t &n,const KT &key){return n.key-key;}
        //-------------------------------------------------
        //����key��Ҫ����չ�ߴ�
        template<class KT>
        static int ext_size(const KT &k){return 0;}

        //-------------------------------------------------
        //���ж����첢��ʼ��
        template<class val_t>
        void OC(uint32_t level,val_t &val,uint32_t es){ct::OC(&key,val);}
        //-------------------------------------------------
        typedef key_t node_key_t;
        key_t  key;
        struct tiny_skipset_node_t *next[1];                //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
    };
    */

    //---------------------------------------------------------
    //����ԭʼ��������������
    template<class node_t,uint32_t MAX_LEVEL>
    class raw_skiplist_t
    {
        uint32_t        m_count;                            //�ڲ��ڵ�����
        uint32_t        m_levels;                           //��ǰ�Ѿ�ʹ�õ���߲���
        node_t         *m_head;                             //ָ��ͷ���
        node_t         *m_tail;                             //ָ��β�ڵ�
        mem_allotter_i &m_mem;                              //�ڴ�������ӿ�
        //-----------------------------------------------------
        //���ݸ����Ĳ�������չ�ߴ�,��̬�����ڵ�
        node_t *m_make_node(uint32_t level, uint32_t ext_size)
        {
            node_t *node = (node_t *)m_mem.alloc(sizeof(node_t) + (level-1) * sizeof(node_t *)+ext_size);
            if (!node) return NULL;

            //�½ڵ����в�ĺ���,��ʼ��ΪNULL
            for (uint32_t lvl = 0; lvl < level; ++lvl)
                node->next[lvl] = NULL;

            return node;
        }
        //-----------------------------------------------------
        //ժ��ָ���Ľڵ�,��������ǰ���ڵ�ĺ���ָ��
        void m_pick(node_t *node, node_t **update)
        {
            for (uint32_t lvl = 0; lvl < m_levels; ++lvl)
            {//�ӵ�������ժ������
                if (update[lvl]->next[lvl] == node)         //����ǰ�ڵ��ǰ��ָ�������
                    update[lvl]->next[lvl] = node->next[lvl];
            }

            for (int32_t lvl = m_levels - 1; lvl >= 0; --lvl)
            {//�ٴӸ��������ж��Ƿ�Ӧ��������Ĳ���
                if (m_head->next[lvl] == NULL)
                    --m_levels;                             //���ͷ���ĵ�ǰ�����ΪNULL,��˵��������������Ӧ�ý�����
                else
                    break;
            }
        }

        //-----------------------------------------------------
        //����ָ��key��ÿ���Ӧ��ǰ��,���ǵ�update��
        //����ֵ:key��Ӧ����ײ��ǰ���ڵ�
        template<class key_t>
        node_t *m_find_prv(const key_t &key, node_t **update)
        {
            node_t  *node = m_head;                         //��ͷ�ڵ㿪ʼ������
            for (int32_t lvl = m_levels - 1; lvl >= 0; --lvl)
            {//����߲���㽵������,����skiplist���㷨�������
                while (node->next[lvl] &&                   //�����ǰ�ڵ��к���
                       node_t::cmp(*node->next[lvl],key)<0) //���ҵ�ǰ�ڵ�ĺ���С��key(˵��key��Ӧ�������)
                       node = node->next[lvl];              //��ǰ�ڵ����,׼����������
                update[lvl] = node;                         //��ǰ����ҽ�����,��¼��ǰ�ڵ�Ϊָ��keyλ�õ�ǰ��
            }
            return node;
        }
    public:
        //-----------------------------------------------------
        raw_skiplist_t(mem_allotter_i &ma):m_mem(ma)
        {
            m_levels = 1;                                   //����ĳ�ʼ����ֻ��1��(����Ϊ0�Ļ�����)
            m_head = m_make_node(MAX_LEVEL,0);              //ͷ���ֻ�����ڴ�,������
            rx_fail(m_head!=NULL);
            m_tail=NULL;
            m_count=0;
        }
        virtual ~raw_skiplist_t()
        {
            clear();
            m_mem.free(m_head);                             //�ͷ�ͷ����ڴ�,������
        }
        //-----------------------------------------------------
        //��ǰ�����еĽڵ�����
        uint32_t size() const {return m_count;}
        //��ǰ�����������
        uint32_t levels() const {return m_levels;}
        //��ȡ��ǰ�����ͷ��β�ڵ�
        node_t* head() const {return m_head->next[0];}
        node_t* tail() const {return m_tail;}
        //-----------------------------------------------------
        //����ָ����key��Ӧ�Ľڵ�(key�����ظ�,����᷵��֮ǰ���ڵĽڵ�ָ��);��������ڵ�Ĳ���level;������չ���ڴ�ߴ�ext_size;
        //���صĽڵ�ָ�������Ϊ��,��duplication�ظ�ָʾΪ��,����Խ��нڵ�Ĺ����ʼ��(key��val��������ʼ��)
        template<class key_t>
        node_t *insert_raw(const key_t &key,bool &duplication,uint32_t ext_size,const uint32_t level)
        {
            node_t *update[MAX_LEVEL];                      //������ʱ��¼��ǰ�ڵ������,��Ӧ�ĸ���ǰ���ڵ�
            node_t *prv=m_find_prv(key,update)->next[0];    //����ָ��key��Ӧ�ĸ���ǰ��,�����Եõ��Ѿ����ڵ�key�ڵ�

            if (prv&&node_t::cmp(*prv,key)==0)
            {
                duplication=true;                           //��֪key�Ѵ���
                return prv;                                 //ֱ�ӷ���
            }

            duplication=false;
            node_t *node = m_make_node(level,ext_size);     //��������ָ���������½ڵ�
            if (!node)
                return NULL;

            if (level > m_levels)
            {//����½ڵ�Ĳ�������ԭ�в���
                for (uint32_t lvl = m_levels; lvl < level; ++lvl)
                    update[lvl] = m_head;                   //������������ǰ��Ϊͷ���
                m_levels = level;                           //���������
            }

            for (uint32_t lvl = 0; lvl < level; ++lvl)
            {//��ʼ���ҽ��½ڵ�
                node->next[lvl] = update[lvl]->next[lvl];   //�½ڵ�ĺ���ָ��ǰ���ڵ�ĺ���
                update[lvl]->next[lvl] = node;              //ǰ���ڵ�ĺ���ָ���½ڵ�
            }

            if (m_count++==0||node->next[0]==NULL)          //��������ǿյ�,�������²���Ľڵ����β�ڵ�(û�к���)
                m_tail=node;                                //��β�ڵ�ָ���Ӧ��ָ�����½ڵ�

            return node;
        }

        //-----------------------------------------------------
        //ֱ��ɾ������key��Ӧ��һ���ڵ�(�����ͬkey�ڵ����ʱ,���ҵ��ĸ�ɾ���ĸ�)
        template<class key_t>
        bool earse(const key_t &key)
        {
            node_t *update[MAX_LEVEL];                      //������ʱ��¼��ǰ�ڵ����ʱ,��Ӧ�ĸ���ǰ���ڵ�
            node_t *node = m_find_prv(key,update)->next[0]; //����ָ��key�ڵ��Ӧ�ĸ���ǰ��,�����Եõ���Ӧ�Ľڵ�
            if (node && node_t::cmp(*node, key)==0)
            {//�����Ӧ�ڵ������key��ͬ,��˵���ҵ��˴˽ڵ�
                m_pick(node, update);                       //ժ��ָ���Ľڵ�(���нڵ����ǰ���ĵ���)
                if (node==m_tail)
                    m_tail=update[0];                       //���ɾ���Ľڵ����β�ڵ�,��β�ڵ�ָ����ǰ��
                ct::OD(node);                               //�ڵ�����
                m_mem.free(node);                           //�ڵ��ڴ��ͷ�
                --m_count;                                  //�����������
                rx_assert_if(m_count==0,m_tail==NULL);
                rx_assert_if(m_count!=0,m_tail!=NULL);
                return true;
            }
            return false;
        }
        //-----------------------------------------------------
        //����ָ��key��Ӧ�Ľڵ�
        template<class key_t>
        node_t *find(const key_t &key) const
        {
            node_t *node = m_head;                          //���׽ڵ㿪ʼ����
            for (int32_t lvl = m_levels - 1; lvl >= 0; --lvl)
            {//�Ӹ߲���ײ�������
                while (node->next[lvl] != NULL)
                {//���нڵ�ıȽ�
                    int cv=node_t::cmp(*node->next[lvl],key);//���нڵ���key�ıȽ�
                    if (cv<0)
                        node = node->next[lvl];             //�ڵ�С��key,��Ҫ����������
                    else if (cv==0)
                        return node->next[lvl];             //key��ͬ,�ҵ���.
                    else
                        break;                              //�ڵ����key˵������鲻��,׼�������
                }
            }
            return NULL;                                    //ȫ���㼶�������,ȷʵû�ҵ�
        }

        //-----------------------------------------------------
        //�������,��������ʹ��
        void clear()
        {
            node_t *node = m_head->next[0];                 //��ͷ���ĺ���(�׸��ڵ�)��ʼ����
            while (node)
            {
                node_t *next = node->next[0];               //��¼��ǰ�ڵ�ĺ���ָ��
                ct::OD(node);                               //�ڵ�����
                m_mem.free(node);                           //�ͷŵ�ǰ�ڵ�
                node = next;                                //��ǰ�ڵ�ָ��ָ�������
            }

            for (uint32_t lvl = 0; lvl < m_levels; ++lvl)
                m_head->next[lvl] = NULL;                   //ͷ���ĺ���ȫ�����

            m_levels=1;
            m_count=0;
            m_tail=NULL;
        }

#if RX_RAW_SKIPLIST_DEBUG_PRINT
        void print()
        {
            for (uint32_t lvl = 0; lvl < m_levels; lvl++)
            {
                node_t *node = m_head->next[lvl];
                printf("Level[%d]:", lvl);

                while (node)
                {
                    printf("%3d->", node->key);
                    node = node->next[lvl];
                }
                printf("\n\n");
            }
        }
#endif

    };




}

#endif
