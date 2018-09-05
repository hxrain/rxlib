#ifndef _RX_DTL_SKIPLIST_H_
#define _RX_DTL_SKIPLIST_H_

#include "rx_cc_macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace rx
{
    #define key_t int8_t
    #define val_t uint32_t
    typedef struct sl_node_t
    {
        key_t key;
        val_t val;
        struct sl_node_t *next[1];
    } sl_node_t;

    #define MAX_LEVEL 4

    //---------------------------------------------------------
    class raw_skiplist_t
    {
        uint32_t        m_levels;                           //当前已经使用的最高层数
        sl_node_t       *m_head;                            //指向头结点

        //-----------------------------------------------------
        //根据给定的层数动态创建节点,并记录key与val
        static sl_node_t *create_node(uint32_t level, const key_t &key, const val_t& val)
        {
            sl_node_t *node = (sl_node_t *)malloc(sizeof(sl_node_t) + (level-1) * sizeof(sl_node_t *));
            node->val = val;
            node->key = key;

            //新节点所有层的后趋,初始都为NULL
            for (uint32_t lvl = 0; lvl < level; lvl++)
                node->next[lvl] = NULL;

            return node;
        }
        //-----------------------------------------------------
        //释放节点占用的空间
        static void free_node(sl_node_t *node)
        {
            free(node);
        }

        //-----------------------------------------------------
        //生成一个随机的层数:>=1;<=最大层数
        static uint32_t random_level()
        {
            uint32_t level = 1;
            while (rand() %2)
                ++level;
            return (level < MAX_LEVEL) ? level : MAX_LEVEL;
        }

        //-----------------------------------------------------
        //摘除指定的节点,并更新其前驱节点的后趋指向
        void pick_node(sl_node_t *node, sl_node_t **update)
        {
            for (uint32_t lvl = 0; lvl < m_levels; lvl++)
            {//从低向高逐层判断
                if (update[lvl]->next[lvl] == node)         //将当前节点的前驱指向其后趋
                    update[lvl]->next[lvl] = node->next[lvl];
            }

            for (uint32_t lvl = m_levels - 1; lvl >= 0; )
            {//再从高向低逐层判断
                if (m_head->next[lvl] == NULL)
                    m_levels--;                             //如果头结点的当前层后趋为NULL,则说明跳表的整体层数应该降低了
                else
                    break;
                if (lvl-- == 0)
                    break;
            }
        }

        //-----------------------------------------------------
        //查找指定key在每层对应的前驱,并记到update中
        //返回值:key对应的最底层的前驱节点
        sl_node_t *m_find_node(const key_t &key, sl_node_t **update)
        {
            sl_node_t  *node = m_head;                      //从头节点开始向后查找
            for (uint32_t lvl = m_levels - 1; lvl >= 0;)
            {//从最高层逐层降级查找,就是skiplist的算法精髓核心
                while (node->next[lvl] &&                   //如果当前节点有后趋
                       node->next[lvl]->key < key)          //并且当前节点的后趋小于key(说明key还应该往后放)
                    node = node->next[lvl];                 //则当前节点后移,准备继续查找
                update[lvl] = node;                         //当前层查找结束了,记录当前节点为指定key位置的前驱
                if ( lvl-- == 0)                            //降层,并判断是否应该结束多层循环
                    break;
            }
            return node;
        }
    public:
        //-----------------------------------------------------
        raw_skiplist_t()
        {
            m_levels = 1;                                   //跳表的初始层数只有1层
            m_head = create_node(MAX_LEVEL, 0, 0);
        }

        //-----------------------------------------------------
        void uninit()
        {
            sl_node_t *node = m_head->next[0];              //从头结点的后趋开始遍历
            while (node)
            {
                sl_node_t *next = node->next[0];            //记录当前节点的后趋指针
                free_node(node);                            //释放当前节点
                node = next;                                //当前节点指针指向其后趋
            }

            free_node(m_head);                              //释放头结点
        }

        //-----------------------------------------------------
        //插入指定的key与val
        bool insert(const key_t &key, const val_t &val,bool duplication=false)
        {
            sl_node_t *update[MAX_LEVEL];
            sl_node_t *try_node=m_find_node(key,update)->next[0];    //查找指定key对应的各层前驱

            if (try_node&&try_node->key==key&&!duplication)
                return false;                               //key已存在且不允许重复出现,则返回

            uint32_t level = random_level();                //生成一个随机的层数
            sl_node_t *node = create_node(level, key, val); //创建含有这些层数的新节点
            if (!node)
                return false;

            if (level > m_levels)
            {//如果新节点的层数大于原有层数,则填充新层的前驱为头结点
                for (uint32_t lvl = m_levels; lvl < level; lvl++)
                    update[lvl] = m_head;
                m_levels = level;                           //更新最大层高
            }

            for (uint32_t lvl = 0; lvl < level; lvl++)
            {//开始逐层挂接新节点
                node->next[lvl] = update[lvl]->next[lvl];   //新节点的后趋指向前驱节点的后趋
                update[lvl]->next[lvl] = node;              //前驱节点的后趋指向新节点
            }
            return true;
        }

        //-----------------------------------------------------
        bool remove(const key_t &key)
        {
            sl_node_t *update[MAX_LEVEL];
            sl_node_t *node = m_find_node(key,update)->next[0]; //查找指定key的各层前驱,并尝试得到对应的节点
            if (node && node->key == key)
            {//如果对应节点存在且key相同,则说明找到了此节点
                pick_node(node, update);                    //摘除指定的节点(进行节点各层前驱的调整)
                free_node(node);                            //释放节点内存
                return true;
            }
            return false;
        }

        //-----------------------------------------------------
        //查找指定key对应的节点
        sl_node_t *find(const key_t &key)
        {
            sl_node_t *node = m_head;                       //从跳表头开始遍历
            for (uint32_t lvl = m_levels - 1; lvl >= 0;)
            {//从高层向底层逐层查找
                while (node->next[lvl] != NULL)
                {//进行节点后趋的判断
                    if (node->next[lvl]->key < key)
                        node = node->next[lvl];             //节点后趋小于key,需要继续向后遍历
                    else if (node->next[lvl]->key == key)
                        return node->next[lvl];             //key相同,找到了.
                    else
                        break;
                }
                if (lvl-- == 0)
                    break;
            }
            return NULL;
        }
        //-----------------------------------------------------
        void print()
        {
            for (uint32_t lvl = 0; lvl < MAX_LEVEL; lvl++)
            {
                sl_node_t *node = m_head->next[lvl];
                printf("Level[%d]:", lvl);

                while (node)
                {
                    printf("%d(%d) -> ", node->key,node->val);
                    node = node->next[lvl];
                }
                printf("\n");
            }
        }

    };




}

#endif
