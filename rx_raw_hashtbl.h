#ifndef _RX_RAW_HASHTBL_TINY_H_
#define _RX_RAW_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"

namespace rx
{
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
        node_t         *m_nodes;                            //节点数组
        uint32_t        m_using;                            //当前节点的数量
        uint32_t        m_collision;                        //发生的value哈希冲突总数
        uint32_t        m_max_slot_size;
    public:
        //-------------------------------------------------
        raw_hashtbl_t():m_nodes(NULL) {}
        //绑定最终可用的节点空间
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
        //添加节点
        //返回值:NULL失败;其他成功.
        template<class key_t>
        node_t *push(uint32_t hash_code,const key_t &value)
        {
            for(uint32_t i=0; i<m_max_slot_size; ++i)
            {
                uint32_t pos=(hash_code+i)%m_max_slot_size; //计算位置
                node_t &node = m_nodes[pos];                //得到节点
                if (!node.state)
                {
                    //该节点尚未使用,那么就直接使用
                    node.state=i+1;                         //记录当前节点冲突顺序
                    m_collision+=i;                         //记录冲突总数
                    ++m_using;
                    return &node;
                }
                else if (vkcmp::equ(node.value,value))
                    return &node;                           //该节点已经被使用了,且value也是重复的,那么就直接给出吧
            }

            return NULL;                                    //转了一圈没地方了!
        }

        //-------------------------------------------------
        //搜索节点
        //返回值:NULL未找到;其他成功
        template<class key_t>
        node_t *find(uint32_t hash_code,const key_t &value) const
        {
            for(uint32_t i=0; i<m_max_slot_size; ++i)
            {
                uint32_t I=(hash_code+i)%m_max_slot_size;   //计算位置
                node_t &node = m_nodes[I];                  //得到节点
                if (!node.state)
                    return NULL;                            //直接就碰到空档了,不用继续了

                if (vkcmp::equ(node.value,value))
                    return &node;                           //顺延后找到了
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
            --m_using;                                      //计数器递减
            return true;
        }
        //-------------------------------------------------
        //最大节点数量
        uint32_t capacity() const { return m_max_slot_size; }
        //已经使用的节点数量
        uint32_t size() const { return m_using; }
        //插入位槽冲突总数
        uint32_t collision() const { return m_collision; }
        //-------------------------------------------------
        //尝试找到pos的下一个有效的位置(跳过中间未被使用的部分)
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
