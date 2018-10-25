#ifndef _RX_RAW_HASHTBL_TINY_H_
#define _RX_RAW_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_ct_util.h"

namespace rx
{
    //��¼ԭʼ��ϣ��Ĺ���״̬(���ô�ű��ڷ���洢)
    typedef struct raw_hashtbl_stat_t
    {
        uint32_t    using_count;                            //��ǰ�ڵ������
        uint32_t    collision;                              //�ڵ��ϣ��ͻ����
        raw_hashtbl_stat_t():using_count(0), collision(0){}
    }raw_hashtbl_stat_t;

    //-----------------------------------------------------
    //�򵥵Ķ�����λ��ϣ��,ʹ�ÿ���ַ��ʽ���г�ͻ����,���ڼ򵥵Ĺ�ϣ����������,���������κ��ڴ����.
    //-----------------------------------------------------
    template<class NVT,class vkcmp>
    class raw_hashtbl_t
    {
    public:
        //-------------------------------------------------
        //��ϣ��ڵ����,ʹ�õ�ʱ��,�����������Ľڵ�̳��ڴ�
        typedef struct node_t
        {
            uint32_t    state;                              //��¼��ǰ�ڵ��Ƿ�ʹ��,0δʹ��;1������ʹ��;>1��ϣ��ͻ����(����3�����ڳ�ͻ��������λ��)
            NVT         value;                              //��ϣ�ڵ��valueֵ
        } node_t;

    private:
        node_t              *m_nodes;                       //�ڵ�����
        raw_hashtbl_stat_t  *m_stat;
        uint32_t             m_max_node_count;              //�����ýڵ�����
    public:
        //-------------------------------------------------
        raw_hashtbl_t():m_nodes(NULL), m_stat(NULL){}
        //�����տ��õĽڵ�ռ�
        //-------------------------------------------------
        bool bind(node_t* nodes,uint32_t max_slot_size, raw_hashtbl_stat_t& stat)
        {
            if (m_nodes)
                return false;
            m_nodes=nodes;
            memset(m_nodes,0,sizeof(node_t)*max_slot_size);
            m_max_node_count=max_slot_size;
            m_stat = &stat;
            return true;
        }
        //-------------------------------------------------
        //ͨ���ڵ�����ֱ�ӷ��ʽڵ�
        node_t *node(uint32_t idx)const { return &m_nodes[idx]; }
        //-------------------------------------------------
        //��ӽڵ�
        //����ֵ:NULLʧ��;�����ɹ�.
        template<class key_t>
        node_t *push(uint32_t hash_code,const key_t &value){uint32_t pos;return push(hash_code,value,pos);}
        template<class key_t>
        node_t *push(uint32_t hash_code,const key_t &value,uint32_t &pos,bool *is_dup=NULL)
        {
            for(uint32_t i=0; i<m_max_node_count; ++i)
            {
                pos=(hash_code+i)%m_max_node_count;         //����λ��
                node_t &node = m_nodes[pos];                //�õ��ڵ�
                if (!node.state)
                {
                    //�ýڵ���δʹ��,��ô��ֱ��ʹ��
                    node.state=i+1;                         //��¼��ǰ�ڵ��ͻ˳��
                    m_stat->collision+=i;              //��¼��ͻ����
                    ++m_stat->using_count;
                    return &node;
                }
                else if (vkcmp::equ(node.value,value))
                {
                    if (is_dup)
                        *is_dup=true;
                    return &node;                           //�ýڵ��Ѿ���ʹ����,��valueҲ���ظ���,��ô��ֱ�Ӹ�����
                }
            }

            pos=-1;
            return NULL;                                    //ת��һȦû�ط���!
        }

        //-------------------------------------------------
        //�����ڵ�
        //����ֵ:NULLδ�ҵ�;�����ɹ�
        template<class key_t>
        node_t *find(uint32_t hash_code,const key_t &value) const {uint32_t pos;return find(hash_code,value,pos);}
        template<class key_t>
        node_t *find(uint32_t hash_code,const key_t &value,uint32_t &pos) const
        {
            for(uint32_t i=0; i<m_max_node_count; ++i)
            {
                uint32_t I=(hash_code+i)%m_max_node_count;  //����λ��
                node_t &node = m_nodes[I];                  //�õ��ڵ�
                if (!node.state)
                    return NULL;                            //ֱ�Ӿ������յ���,���ü�����

                if (vkcmp::equ(node.value,value))
                {
                    pos=I;
                    return &node;                           //˳�Ӻ��ҵ���
                }
            }

            return NULL;                                    //ת��һȦû�ҵ�!
        }
        //-------------------------------------------------
        //ɾ���ڵ�
        bool remove(node_t *node)
        {
            if (!node|| node->state==0)
                return false;
            node->state=0;                                  //ɾ�����������Ǵ���
            --m_stat->using_count;                     //�������ݼ�
            return true;
        }
        //-------------------------------------------------
        //���ڵ�����
        uint32_t capacity() const { return m_max_node_count; }
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_stat->using_count; }
        //����λ�۳�ͻ����
        uint32_t collision() const { return m_stat->collision; }
        //-------------------------------------------------
        //�����ҵ�pos�ڵ�����һ����ʹ�õĽڵ�(�����м�δ��ʹ�õĲ���)
        uint32_t next(uint32_t pos) const
        {
            while (++pos < m_max_node_count)
                if (m_nodes[pos].state)
                    return pos;
            return m_max_node_count;
        }
        //-------------------------------------------------
        //����ȫ���ڵ�,�ع��ʼ״̬.
        void clear()
        {
            for(uint32_t pos=next(-1);pos<m_max_node_count;pos=next(pos))
            {
                node_t &node=m_nodes[pos];
                ct::OD(&node.value);
                remove(&node);
            }
        }
    };

}
#endif
