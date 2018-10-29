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
            uint16_t    flag;                               //�ڵ���:0δʹ��;1ʹ����;2��ɾ��.
            uint16_t    step;                               //��¼��ͻ�Ĳ���0~n
            NVT         value;                              //��ϣ�ڵ��valueֵ
        } node_t;

    private:
        node_t              *m_nodes;                       //�ڵ�����
        raw_hashtbl_stat_t  *m_stat;                        //��ϣ��״̬��¼
        //-------------------------------------------------
        //�жϸ����Ľڵ��Ƿ�Ϊ�սڵ�
        static bool node_is_empty(const node_t &node) {return node.flag==0;}
        //�жϸ����Ľڵ��Ƿ�δ��ʹ��
        static bool node_is_unused(const node_t &node) {return node.flag==0||node.flag==2;}
        //�жϸ����Ľڵ��Ƿ�ɾ��
        static bool node_is_deleted(const node_t &node) {return node.flag==2;}
        //�жϸ����Ľڵ��Ƿ�ʹ��
        static bool node_is_using(const node_t &node) {return node.flag==1;}
        //���ýڵ㱻ʹ��
        static void node_used(node_t &node,uint16_t step){node.flag=1;node.step=step;}
        //���ýڵ㱻ɾ��
        static void node_delete(node_t &node){node.flag=2;}
    public:
        //-------------------------------------------------
        raw_hashtbl_t():m_nodes(NULL), m_stat(NULL){}
        //�����տ��õĽڵ�ռ�
        //-------------------------------------------------
        void bind(node_t* nodes, raw_hashtbl_stat_t* stat,bool clear=true)
        {
            m_nodes=nodes;
            if (m_nodes&&clear)
                memset(m_nodes,0,sizeof(node_t)*stat->max_nodes);
            m_stat = stat;
        }
        bool is_valid() {return m_nodes&&m_stat;}
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
                if (node_is_unused(node))
                {
                    node_used(node,i);                      //�ýڵ�δ��ʹ����,��ô��ֱ��ʹ��(˳��ɼ�¼��ǰ�ڵ��ͻ����)

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
                //����������δ��ʹ�õĽڵ�.
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
                if (node_is_empty(node))
                    return NULL;                            //ֱ�Ӿ������յ���,���ü�����

                if (vkcmp::equ(node.value,value))
                {
                    if (node_is_deleted(node))
                        return NULL;                        //����ҵ�����֮ǰ�Ѿ���ɾ����ֵ,ֱ�ӷ���.

                    pos=I;
                    return &node;                           //�ҵ���
                }

                //���˳����һ����
            }

            return NULL;                                    //ת��һȦû�ҵ�!
        }
        //-------------------------------------------------
        //ɾ���ڵ�
        bool remove(node_t *node,uint32_t pos)
        {
            if (!node || node_is_unused(*node))
                return false;
            node_delete(*node);                             //ɾ�����������Ǵ���
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
                if (node_is_using(m_nodes[pos]))
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
