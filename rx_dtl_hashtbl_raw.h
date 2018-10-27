#ifndef _RX_RAW_HASHTBL_TINY_H_
#define _RX_RAW_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_ct_util.h"

namespace rx
{
    //-----------------------------------------------------
    //记录原始哈希表的工作状态(外置存放便于分离存储)
    typedef struct raw_hashtbl_stat_t
    {
        uint32_t    max_nodes;                              //最大的节点容量
        uint32_t    using_count;                            //当前节点的数量
        uint32_t    collision_count;                        //节点哈希冲突总数
        uint32_t    collision_length;                       //节点哈希冲突总长度
        raw_hashtbl_stat_t():max_nodes(0),using_count(0), collision_count(0), collision_length(0){}
    }raw_hashtbl_stat_t;

    //-----------------------------------------------------
    //简单的定长槽位哈希表,使用开地址方式进行冲突处理,用于简单的哈希搜索管理器,自身不进行任何内存管理.
    //-----------------------------------------------------
    template<class NVT,class vkcmp>
    class raw_hashtbl_t
    {
    public:
        //-------------------------------------------------
        //哈希表节点基类,使用的时候,必须让真正的节点继承于此
        typedef struct node_t
        {
            uint32_t    state;                              //记录当前节点是否被使用,0未使用;1正常被使用;>1哈希冲突序数(比如3就是在冲突点后第三个位置)
            NVT         value;                              //哈希节点的value值
        } node_t;

    private:
        node_t              *m_nodes;                       //节点数组
        raw_hashtbl_stat_t  *m_stat;
    public:
        //-------------------------------------------------
        raw_hashtbl_t():m_nodes(NULL), m_stat(NULL){}
        //绑定最终可用的节点空间
        //-------------------------------------------------
        void bind(node_t* nodes,uint32_t max_slot_size, raw_hashtbl_stat_t* stat,bool clear=true)
        {
            m_nodes=nodes;
            if (m_nodes&&clear)
                memset(m_nodes,0,sizeof(node_t)*max_slot_size);
            m_stat = stat;
            m_stat->max_nodes = max_slot_size;
        }
        //-------------------------------------------------
        //通过节点索引直接访问节点
        node_t *node(uint32_t idx)const { return &m_nodes[idx]; }
        //-------------------------------------------------
        //添加节点
        //返回值:NULL失败;其他成功.
        template<class key_t>
        node_t *push(uint32_t hash_code,const key_t &value){uint32_t pos;return push(hash_code,value,pos);}
        template<class key_t>
        node_t *push(uint32_t hash_code,const key_t &value,uint32_t &pos,bool *is_dup=NULL)
        {
            for(uint32_t i=0; i<capacity(); ++i)
            {
                pos=(hash_code+i)%capacity();               //计算位置
                node_t &node = m_nodes[pos];                //得到节点
                if (!node.state)
                {
                    //该节点尚未使用,那么就直接使用
                    node.state=i+1;                         //记录当前节点冲突顺序
                    if (i)
                        m_stat->collision_count +=1;        //记录冲突总数
                    m_stat->collision_length += i;          //记录冲突总步长
                    ++m_stat->using_count;
                    return &node;
                }
                else if (vkcmp::equ(node.value,value))
                {
                    if (is_dup)
                        *is_dup=true;
                    return &node;                           //该节点已经被使用了,且value也是重复的,那么就直接给出吧
                }
            }

            pos=-1;
            return NULL;                                    //转了一圈没地方了!
        }

        //-------------------------------------------------
        //搜索节点
        //返回值:NULL未找到;其他成功
        template<class key_t>
        node_t *find(uint32_t hash_code,const key_t &value) const {uint32_t pos;return find(hash_code,value,pos);}
        template<class key_t>
        node_t *find(uint32_t hash_code,const key_t &value,uint32_t &pos) const
        {
            for(uint32_t i=0; i<capacity(); ++i)
            {
                uint32_t I=(hash_code+i)%capacity();        //计算位置
                node_t &node = m_nodes[I];                  //得到节点
                if (!node.state)
                    return NULL;                            //直接就碰到空档了,不用继续了

                if (vkcmp::equ(node.value,value))
                {
                    pos=I;
                    return &node;                           //顺延后找到了
                }
            }

            return NULL;                                    //转了一圈没找到!
        }
        //-------------------------------------------------
        //删除节点
        bool remove(node_t *node)
        {
            if (!node|| node->state==0)
                return false;
            node->state=0;                                  //删除动作仅仅是打标记
            --m_stat->using_count;                          //计数器递减
            return true;
        }
        //-------------------------------------------------
        //最大节点数量
        uint32_t capacity() const { return m_stat->max_nodes; }
        //已经使用的节点数量
        uint32_t size() const { return m_stat->using_count; }
        //只读获取内部状态
        const raw_hashtbl_stat_t& stat() const { return *m_stat; }
        //-------------------------------------------------
        //尝试找到pos节点后的下一个被使用的节点(跳过中间未被使用的部分)
        uint32_t next(uint32_t pos) const
        {
            while (++pos < capacity())
                if (m_nodes[pos].state)
                    return pos;
            return capacity();
        }
        //-------------------------------------------------
        //清理全部节点,回归初始状态.
        void clear()
        {
            for(uint32_t pos=next(-1);pos<capacity();pos=next(pos))
            {
                node_t &node=m_nodes[pos];
                ct::OD(&node.value);
                remove(&node);
            }
        }
    };

}
#endif
