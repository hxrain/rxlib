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
        uint32_t    max_nodes;                              //最大的节点容量,必须给定初始值
        uint32_t    using_count;                            //当前节点的数量
        uint32_t    collision_count;                        //节点哈希冲突总数
        uint32_t    collision_length;                       //节点哈希冲突总长度
        raw_hashtbl_stat_t():using_count(0), collision_count(0), collision_length(0){}
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
            uint16_t    flag;                               //节点标记:0未使用;1使用中;2被删除.
            uint16_t    step;                               //记录冲突的步数0~n
            NVT         value;                              //哈希节点的value值
        } node_t;

        //节点状态标记的类型定义
        enum
        {
            node_flag_empty     =0,
            node_flag_using     =1,
            node_flag_removed   =2,
        };
    private:
        node_t              *m_nodes;                       //节点数组
        raw_hashtbl_stat_t  *m_stat;                        //哈希表状态记录
        //-------------------------------------------------
        //判断给定的节点是否为空节点
        static bool node_is_empty(const node_t &node) { return node.flag == node_flag_empty; }
        //判断给定的节点是否未被使用
        static bool node_is_unused(const node_t &node) { return node.flag == node_flag_empty || node.flag == node_flag_removed; }
        //判断给定的节点是否被删除
        static bool node_is_deleted(const node_t &node) { return node.flag == node_flag_removed; }
        //判断给定的节点是否被使用
        static bool node_is_using(const node_t &node) { return node.flag == node_flag_using; }
        //设置节点被使用
        static void node_set_used(node_t &node, uint16_t step) { node.flag = node_flag_using; node.step = step; }
        //设置节点被删除
        static void node_set_delete(node_t &node) { node.flag = node_flag_removed; }
        //设置节点被清空
        static void node_set_empty(node_t &node) { node.flag = node_flag_empty; }
    public:
        //-------------------------------------------------
        raw_hashtbl_t():m_nodes(NULL), m_stat(NULL){}
        //绑定最终可用的节点空间
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
                if (node_is_unused(node))
                {
                    node_set_used(node,i);                  //该节点未被使用中,那么就直接使用(顺便可记录当前节点冲突步长)

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
                //继续向后查找未被使用的节点.
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
                if (node_is_empty(node))
                    return NULL;                            //直接就碰到空档了,不用继续了

                if (vkcmp::equ(node.value,value))
                {
                    if (node_is_deleted(node))
                        return NULL;                        //如果找到的是之前已经被删除的值,直接放弃.

                    pos=I;
                    return &node;                           //找到了
                }

                //向后顺序逐一查找
            }

            return NULL;                                    //转了一圈没找到!
        }
        //-------------------------------------------------
        //删除节点
        bool remove(node_t *node,uint32_t pos)
        {
            if (!node || node_is_unused(*node))
                return false;
            node_set_delete(*node);                         //删除动作仅仅是打标记
            --m_stat->using_count;                          //计数器递减
            return true;
        }
        //-------------------------------------------------
        //在remove之后尝试进行从begin_pos的后面开始查找应该移动到begin_pos处的节点位置
        //返回值:-1没有;其他为待移动节点位置
        uint32_t correct_find(uint32_t begin_pos,uint32_t abort_pos)
        {
            //需要校正的情况有:pos占用了后面某个节点的位置;
            for(uint32_t i=1; i<capacity(); ++i)
            {
                //得到当前遍历位置
                uint32_t curr_pos = (begin_pos + i) % capacity();
                //当前位置位空,结束
                if (node_is_empty(m_nodes[curr_pos])||curr_pos==abort_pos)
                    return -1;
                //得到当前节点的预期位置
                uint32_t right_pos = curr_pos - m_nodes[curr_pos].step;

                if (((curr_pos > begin_pos) && (right_pos <= begin_pos || right_pos > curr_pos)) ||
                    ((curr_pos < begin_pos) && (right_pos <= begin_pos && right_pos > curr_pos)))
                    return curr_pos;
            }
            //全表循环完成,遍历结束
            return -1;
        }
        //-------------------------------------------------
        //校正指定节点,将其状态改为空闲
        void correct_empty(uint32_t begin_pos) { node_set_empty(m_nodes[begin_pos]); }
        //-------------------------------------------------
        //校正指定节点后的跟随节点,用于remove之后的空洞修复,避免频繁增删后空洞过少降低查询性能
        //此方法进行节点数据的直接赋值拷贝,begin_pos为刚刚被删除的节点索引
        //返回值:校正过程中调整过的节点数量
        uint32_t correct_following(uint32_t begin_pos)
        {
            rx_assert_ret(m_nodes[begin_pos].flag==node_flag_removed);  //要求初始节点必须是被删除的节点

            uint32_t rc = 0;
            uint32_t abort_pos=begin_pos;
            while(1)
            {
                //从开始点向后查找待校正的节点位置
                uint32_t next_pos = correct_find(begin_pos,abort_pos);
                if (next_pos == (uint32_t)-1)               //找不到了则结束
                {
                    correct_empty(begin_pos);               //开始点可以被置空了
                    break;
                }
                    
                ++rc;
                //进行节点内容的复制
                node_t &dst_node = m_nodes[begin_pos];
                node_t &src_node = m_nodes[next_pos];
                
                dst_node.value = src_node.value;
                dst_node.flag = src_node.flag;
                rx_assert(dst_node.flag==node_flag_using);

                //进行节点冲突步长的校正
                int dp = next_pos - begin_pos;
                dst_node.step = src_node.step - dp;

                begin_pos = next_pos;                       //重新设置开始点,准备继续遍历
            }
            return rc;
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
        //返回值:下一个节点的位置;与容器的容量相同时代表结束
        uint32_t next(uint32_t pos) const
        {
            while (++pos < capacity())
                if (node_is_using(m_nodes[pos]))
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
                remove(&node,pos);
            }
        }
    };

}
#endif
