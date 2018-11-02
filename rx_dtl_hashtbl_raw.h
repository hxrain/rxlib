#ifndef _RX_RAW_HASHTBL_TINY_H_
#define _RX_RAW_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_ct_util.h"

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

        //�ڵ�״̬��ǵ����Ͷ���
        enum
        {
            node_flag_empty     =0,
            node_flag_using     =1,
            node_flag_removed   =2,
        };
    private:
        node_t              *m_nodes;                       //�ڵ�����
        raw_hashtbl_stat_t  *m_stat;                        //��ϣ��״̬��¼
        //-------------------------------------------------
        //�жϸ����Ľڵ��Ƿ�Ϊ�սڵ�
        static bool node_is_empty(const node_t &node) { return node.flag == node_flag_empty; }
        //�жϸ����Ľڵ��Ƿ�δ��ʹ��
        static bool node_is_unused(const node_t &node) { return node.flag == node_flag_empty || node.flag == node_flag_removed; }
        //�жϸ����Ľڵ��Ƿ�ɾ��
        static bool node_is_deleted(const node_t &node) { return node.flag == node_flag_removed; }
        //�жϸ����Ľڵ��Ƿ�ʹ��
        static bool node_is_using(const node_t &node) { return node.flag == node_flag_using; }
        //���ýڵ㱻ʹ��
        static void node_set_used(node_t &node, uint16_t step) { node.flag = node_flag_using; node.step = step; }
        //���ýڵ㱻ɾ��
        static void node_set_delete(node_t &node) { node.flag = node_flag_removed; }
        //���ýڵ㱻���
        static void node_set_empty(node_t &node) { node.flag = node_flag_empty; }
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
                    node_set_used(node,i);                  //�ýڵ�δ��ʹ����,��ô��ֱ��ʹ��(˳��ɼ�¼��ǰ�ڵ��ͻ����)

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
            node_set_delete(*node);                         //ɾ�����������Ǵ���
            --m_stat->using_count;                          //�������ݼ�
            return true;
        }
        //-------------------------------------------------
        //��remove֮���Խ��д�begin_pos�ĺ��濪ʼ����Ӧ���ƶ���begin_pos���Ľڵ�λ��
        //����ֵ:-1û��;����Ϊ���ƶ��ڵ�λ��
        uint32_t correct_find(uint32_t begin_pos,uint32_t abort_pos)
        {
            //��ҪУ���������:posռ���˺���ĳ���ڵ��λ��;
            for(uint32_t i=1; i<capacity(); ++i)
            {
                //�õ���ǰ����λ��
                uint32_t curr_pos = (begin_pos + i) % capacity();
                //��ǰλ��λ��,����
                if (node_is_empty(m_nodes[curr_pos])||curr_pos==abort_pos)
                    return -1;
                //�õ���ǰ�ڵ��Ԥ��λ��
                uint32_t right_pos = curr_pos - m_nodes[curr_pos].step;

                if (((curr_pos > begin_pos) && (right_pos <= begin_pos || right_pos > curr_pos)) ||
                    ((curr_pos < begin_pos) && (right_pos <= begin_pos && right_pos > curr_pos)))
                    return curr_pos;
            }
            //ȫ��ѭ�����,��������
            return -1;
        }
        //-------------------------------------------------
        //У��ָ���ڵ�,����״̬��Ϊ����
        void correct_empty(uint32_t begin_pos) { node_set_empty(m_nodes[begin_pos]); }
        //-------------------------------------------------
        //У��ָ���ڵ��ĸ���ڵ�,����remove֮��Ŀն��޸�,����Ƶ����ɾ��ն����ٽ��Ͳ�ѯ����
        //�˷������нڵ����ݵ�ֱ�Ӹ�ֵ����,begin_posΪ�ոձ�ɾ���Ľڵ�����
        //����ֵ:У�������е������Ľڵ�����
        uint32_t correct_following(uint32_t begin_pos)
        {
            rx_assert_ret(m_nodes[begin_pos].flag==node_flag_removed);  //Ҫ���ʼ�ڵ�����Ǳ�ɾ���Ľڵ�

            uint32_t rc = 0;
            uint32_t abort_pos=begin_pos;
            while(1)
            {
                //�ӿ�ʼ�������Ҵ�У���Ľڵ�λ��
                uint32_t next_pos = correct_find(begin_pos,abort_pos);
                if (next_pos == (uint32_t)-1)               //�Ҳ����������
                {
                    correct_empty(begin_pos);               //��ʼ����Ա��ÿ���
                    break;
                }
                    
                ++rc;
                //���нڵ����ݵĸ���
                node_t &dst_node = m_nodes[begin_pos];
                node_t &src_node = m_nodes[next_pos];
                
                dst_node.value = src_node.value;
                dst_node.flag = src_node.flag;
                rx_assert(dst_node.flag==node_flag_using);

                //���нڵ��ͻ������У��
                int dp = next_pos - begin_pos;
                dst_node.step = src_node.step - dp;

                begin_pos = next_pos;                       //�������ÿ�ʼ��,׼����������
            }
            return rc;
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
        //����ֵ:��һ���ڵ��λ��;��������������ͬʱ�������
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
