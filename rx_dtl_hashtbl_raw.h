#ifndef _RX_RAW_HASHTBL_TINY_H_
#define _RX_RAW_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_ct_util.h"
#include "rx_bits_op.h"

namespace rx
{
    //-----------------------------------------------------
    //��¼ԭʼ��ϣ��Ĺ���״̬(���ô�ű��ڷ���洢)
    typedef struct raw_hashtbl_stat_t
    {
        uint32_t    max_nodes;                              //���Ľڵ�����,���������ʼֵ
        uint32_t    using_count;                            //��ǰ�ڵ������
        uint32_t    collision_count;                        //�ڵ��ϣ��ͻ����
        uint32_t    collision_length;                       //�ڵ��ϣ��ͻ�ܳ���
        raw_hashtbl_stat_t():using_count(0), collision_count(0), collision_length(0){}
    }raw_hashtbl_stat_t;

    #define raw_hashtbl_using_node_state 1

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
#if raw_hashtbl_using_node_state
            uint32_t    state;                              //��¼��ǰ�ڵ��Ƿ�ʹ��,0δʹ��;1������ʹ��;>1��ϣ��ͻ����(����3�����ڳ�ͻ��������λ��)
#endif
            NVT         value;                              //��ϣ�ڵ��valueֵ
        } node_t;

    private:
        node_t              *m_nodes;                       //�ڵ�����
        raw_hashtbl_stat_t  *m_stat;                        //��ϣ��״̬��¼
        uint8_t             *m_bits;                        //���ڼ�¼�ڵ��Ƿ�ʹ�õı�������
        uint32_t             m_bytesize;
        //-------------------------------------------------
        //�жϸ����Ľڵ��Ƿ�ʹ��
        bool m_node_tst(const node_t &node,uint32_t idx)const
        {
        #if raw_hashtbl_using_node_state
            return node.state!=0;
        #else
            if (m_bits) return rx_bits_tst(idx,m_bits,m_bytesize);
            else return !equ_zero(&node.value,sizeof(node.value));
        #endif
        }
        //���û�����ָ���ڵ��ʹ�ñ��
        void m_node_set(node_t &node,uint32_t idx,uint32_t is_using)
        {
        #if raw_hashtbl_using_node_state
            node.state=is_using;
        #else
            if (m_bits) 
            {
                if (is_using) 
                    rx_bits_set(idx,m_bits,m_bytesize);
                else
                    rx_bits_clr(idx,m_bits,m_bytesize);
            }
            else 
            {
                if (!is_using)
                    memset(&node.value,0,sizeof(node.value));
            }
        #endif
        }
    public:
        //-------------------------------------------------
        raw_hashtbl_t():m_nodes(NULL), m_stat(NULL), m_bits(NULL){}
        //�����տ��õĽڵ�ռ�
        //-------------------------------------------------
        void bind(node_t* nodes, raw_hashtbl_stat_t* stat,bool clear=true,uint8_t *bytes=NULL,uint32_t bytesize=0)
        {
            m_nodes=nodes;
            if (m_nodes&&clear)
                memset(m_nodes,0,sizeof(node_t)*stat->max_nodes);
            m_stat = stat;
            m_bits = bytes;
            m_bytesize=bytesize;
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
            for(uint32_t i=0; i<capacity(); ++i)
            {
                pos=(hash_code+i)%capacity();               //����λ��
                node_t &node = m_nodes[pos];                //�õ��ڵ�
                if (!m_node_tst(node,pos))
                {
                    m_node_set(node,pos,i+1);               //�ýڵ���δʹ��,��ô��ֱ��ʹ��(˳��ɼ�¼��ǰ�ڵ��ͻ����)

                    if (i)
                        m_stat->collision_count +=1;        //��¼��ͻ����
                    m_stat->collision_length += i;          //��¼��ͻ�ܲ���
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
            for(uint32_t i=0; i<capacity(); ++i)
            {
                uint32_t I=(hash_code+i)%capacity();        //����λ��
                node_t &node = m_nodes[I];                  //�õ��ڵ�
                if (!m_node_tst(node,I))
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
        bool remove(node_t *node,uint32_t pos)
        {
            if (!node|| !m_node_tst(*node,pos))
                return false;
            m_node_set(*node,pos,false);                    //ɾ�����������Ǵ���
            --m_stat->using_count;                          //�������ݼ�
            return true;
        }
        //-------------------------------------------------
        //���ڵ�����
        uint32_t capacity() const { return m_stat->max_nodes; }
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_stat->using_count; }
        //ֻ����ȡ�ڲ�״̬
        const raw_hashtbl_stat_t& stat() const { return *m_stat; }
        //-------------------------------------------------
        //�����ҵ�pos�ڵ�����һ����ʹ�õĽڵ�(�����м�δ��ʹ�õĲ���)
        uint32_t next(uint32_t pos) const
        {
            while (++pos < capacity())
                if (m_node_tst(m_nodes[pos],pos))
                    return pos;
            return capacity();
        }
        //-------------------------------------------------
        //����ȫ���ڵ�,�ع��ʼ״̬.
        void clear()
        {
            for(uint32_t pos=next(-1);pos<capacity();pos=next(pos))
            {
                node_t &node=m_nodes[pos];
                ct::OD(&node.value);
                remove(&node,pos);
            }
        }
    };

}
#endif
