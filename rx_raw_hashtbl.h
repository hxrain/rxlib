#ifndef _RX_RAW_HASHTBL_TINY_H_
#define _RX_RAW_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"

namespace rx
{
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
        node_t         *m_nodes;                            //�ڵ�����
        uint32_t        m_using;                            //��ǰ�ڵ������
        uint32_t        m_collision;                        //������value��ϣ��ͻ����
        uint32_t        m_max_slot_size;
    public:
        //-------------------------------------------------
        raw_hashtbl_t():m_nodes(NULL) {}
        //�����տ��õĽڵ�ռ�
        //-------------------------------------------------
        bool bind(node_t* nodes,uint32_t max_slot_size)
        {
            if (m_nodes)
                return false;
            m_nodes=nodes;
            memset(m_nodes,0,sizeof(node_t)*max_slot_size);
            m_max_slot_size=max_slot_size;
            m_using=0;
            m_collision=0;
            return true;
        }
        //-------------------------------------------------
        //��ӽڵ�
        //����ֵ:NULLʧ��;�����ɹ�.
        template<class key_t>
        node_t *push(uint32_t hash_code,const key_t &value)
        {
            for(uint32_t i=0; i<m_max_slot_size; ++i)
            {
                uint32_t pos=(hash_code+i)%m_max_slot_size; //����λ��
                node_t &node = m_nodes[pos];                //�õ��ڵ�
                if (!node.state)
                {
                    //�ýڵ���δʹ��,��ô��ֱ��ʹ��
                    node.state=i+1;                         //��¼��ǰ�ڵ��ͻ˳��
                    m_collision+=i;                         //��¼��ͻ����
                    ++m_using;
                    return &node;
                }
                else if (vkcmp::equ(node.value,value))
                    return &node;                           //�ýڵ��Ѿ���ʹ����,��valueҲ���ظ���,��ô��ֱ�Ӹ�����
            }

            return NULL;                                    //ת��һȦû�ط���!
        }

        //-------------------------------------------------
        //�����ڵ�
        //����ֵ:NULLδ�ҵ�;�����ɹ�
        template<class key_t>
        node_t *find(uint32_t hash_code,const key_t &value) const
        {
            for(uint32_t i=0; i<m_max_slot_size; ++i)
            {
                uint32_t I=(hash_code+i)%m_max_slot_size;   //����λ��
                node_t &node = m_nodes[I];                  //�õ��ڵ�
                if (!node.state)
                    return NULL;                            //ֱ�Ӿ������յ���,���ü�����

                if (vkcmp::equ(node.value,value))
                    return &node;                           //˳�Ӻ��ҵ���
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
            --m_using;                                      //�������ݼ�
            return true;
        }
        //-------------------------------------------------
        //���ڵ�����
        uint32_t capacity() const { return m_max_slot_size; }
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_using; }
        //����λ�۳�ͻ����
        uint32_t collision() const { return m_collision; }
        //-------------------------------------------------
        //�����ҵ�pos����һ����Ч��λ��(�����м�δ��ʹ�õĲ���)
        uint32_t next(uint32_t pos) const
        {
            while (++pos < m_max_slot_size)
                if (m_nodes[pos].state)
                    return pos;
            return m_max_slot_size;
        }
    };

}
#endif
