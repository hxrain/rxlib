#ifndef _RX_RAW_SKIPLIST_H_
#define _RX_RAW_SKIPLIST_H_

#include "rx_cc_macro.h"
#include "rx_mem_alloc.h"

namespace rx
{
    //是否开启原始跳表的调试打印功能
    #define RX_RAW_SKIPLIST_DEBUG_PRINT 0
    /*
    //原始跳表的节点类型(示例,只要对外接口语义正确即可)
    template<class key_t>
    struct tiny_skipset_node_t
    {
        //-------------------------------------------------
        //比较节点与给定key的大小
        //返回值:n<key为<0;n==key为0;n>key为>0
        template<class KT>
        static int cmp(const tiny_skipset_node_t &n,const KT &key){return n.key-key;}
        //-------------------------------------------------
        //计算key需要的扩展尺寸
        template<class KT>
        static int ext_size(const KT &k){return 0;}

        //-------------------------------------------------
        //进行定向构造并初始化
        template<class val_t>
        void OC(uint32_t level,val_t &val,uint32_t es){ct::OC(&key,val);}
        //-------------------------------------------------
        typedef key_t node_key_t;
        key_t  key;
        struct tiny_skipset_node_t *next[1];                //跳表实现的关键:分层的后趋节点指针,必须放在节点的最后,用于弹性扩展访问
    };
    */

    //---------------------------------------------------------
    //定义原始的跳表容器基类
    template<class node_t,uint32_t MAX_LEVEL>
    class raw_skiplist_t
    {
        uint32_t        m_count;                            //内部节点数量
        uint32_t        m_levels;                           //当前已经使用的最高层数
        node_t         *m_head;                             //指向头结点
        node_t         *m_tail;                             //指向尾节点
        mem_allotter_i &m_mem;                              //内存分配器接口
        //-----------------------------------------------------
        //根据给定的层数与扩展尺寸,动态创建节点
        node_t *m_make_node(uint32_t level, uint32_t ext_size)
        {
            node_t *node = (node_t *)m_mem.alloc(sizeof(node_t) + (level-1) * sizeof(node_t *)+ext_size);
            if (!node) return NULL;

            //新节点所有层的后趋,初始都为NULL
            for (uint32_t lvl = 0; lvl < level; ++lvl)
                node->next[lvl] = NULL;

            return node;
        }
        //-----------------------------------------------------
        //摘除指定的节点,并更新其前趋节点的后趋指向
        void m_pick(node_t *node, node_t **update)
        {
            for (uint32_t lvl = 0; lvl < m_levels; ++lvl)
            {//从低向高逐层摘除处理
                if (update[lvl]->next[lvl] == node)         //将当前节点的前趋指向其后趋
                    update[lvl]->next[lvl] = node->next[lvl];
            }

            for (int32_t lvl = m_levels - 1; lvl >= 0; --lvl)
            {//再从高向低逐层判断是否应降低整体的层数
                if (m_head->next[lvl] == NULL)
                    --m_levels;                             //如果头结点的当前层后趋为NULL,则说明跳表的整体层数应该降低了
                else
                    break;
            }
        }

        //-----------------------------------------------------
        //查找指定key在每层对应的前趋,并记到update中
        //返回值:key对应的最底层的前趋节点
        template<class key_t>
        node_t *m_find_prv(const key_t &key, node_t **update)
        {
            node_t  *node = m_head;                         //从头节点开始向后查找
            for (int32_t lvl = m_levels - 1; lvl >= 0; --lvl)
            {//从最高层逐层降级查找,就是skiplist的算法精髓核心
                while (node->next[lvl] &&                   //如果当前节点有后趋
                       node_t::cmp(*node->next[lvl],key)<0) //并且当前节点的后趋小于key(说明key还应该往后放)
                       node = node->next[lvl];              //则当前节点后移,准备继续查找
                update[lvl] = node;                         //当前层查找结束了,记录当前节点为指定key位置的前趋
            }
            return node;
        }
    public:
        //-----------------------------------------------------
        raw_skiplist_t(mem_allotter_i &ma):m_mem(ma)
        {
            m_levels = 1;                                   //跳表的初始层数只有1层(索引为0的基础层)
            m_head = m_make_node(MAX_LEVEL,0);              //头结点只分配内存,不构造
            rx_fail(m_head!=NULL);
            m_tail=NULL;
            m_count=0;
        }
        virtual ~raw_skiplist_t()
        {
            clear();
            m_mem.free(m_head);                             //释放头结点内存,不析构
        }
        //-----------------------------------------------------
        //当前跳表中的节点数量
        uint32_t size() const {return m_count;}
        //当前跳表的最大层数
        uint32_t levels() const {return m_levels;}
        //获取当前跳表的头与尾节点
        node_t* head() const {return m_head->next[0];}
        node_t* tail() const {return m_tail;}
        //-----------------------------------------------------
        //插入指定的key对应的节点(key不可重复,否则会返回之前存在的节点指针);需给定本节点的层数level;给定扩展的内存尺寸ext_size;
        //返回的节点指针如果不为空,且duplication重复指示为假,则可以进行节点的构造初始化(key与val的真正初始化)
        template<class key_t>
        node_t *insert_raw(const key_t &key,bool &duplication,uint32_t ext_size,const uint32_t level)
        {
            node_t *update[MAX_LEVEL];                      //用于临时记录当前节点操作中,对应的各层前趋节点
            node_t *prv=m_find_prv(key,update)->next[0];    //查找指定key对应的各层前趋,并尝试得到已经存在的key节点

            if (prv&&node_t::cmp(*prv,key)==0)
            {
                duplication=true;                           //告知key已存在
                return prv;                                 //直接返回
            }

            duplication=false;
            node_t *node = m_make_node(level,ext_size);     //创建含有指定层数的新节点
            if (!node)
                return NULL;

            if (level > m_levels)
            {//如果新节点的层数大于原有层数
                for (uint32_t lvl = m_levels; lvl < level; ++lvl)
                    update[lvl] = m_head;                   //则填充新增层的前趋为头结点
                m_levels = level;                           //更新最大层高
            }

            for (uint32_t lvl = 0; lvl < level; ++lvl)
            {//开始逐层挂接新节点
                node->next[lvl] = update[lvl]->next[lvl];   //新节点的后趋指向前趋节点的后趋
                update[lvl]->next[lvl] = node;              //前趋节点的后趋指向新节点
            }

            if (m_count++==0||node->next[0]==NULL)          //如果跳表是空的,或者最新插入的节点就是尾节点(没有后趋)
                m_tail=node;                                //则尾节点指针就应该指向最新节点

            return node;
        }

        //-----------------------------------------------------
        //直接删除给定key对应的一个节点(多个相同key节点存在时,先找到哪个删除哪个)
        template<class key_t>
        bool earse(const key_t &key)
        {
            node_t *update[MAX_LEVEL];                      //用于临时记录当前节点操作时,对应的各层前趋节点
            node_t *node = m_find_prv(key,update)->next[0]; //查找指定key节点对应的各层前趋,并尝试得到对应的节点
            if (node && node_t::cmp(*node, key)==0)
            {//如果对应节点存在且key相同,则说明找到了此节点
                m_pick(node, update);                       //摘除指定的节点(进行节点各层前趋的调整)
                if (node==m_tail)
                    m_tail=update[0];                       //如果删除的节点就是尾节点,则尾节点指向其前趋
                ct::OD(node);                               //节点析构
                m_mem.free(node);                           //节点内存释放
                --m_count;                                  //跳表计数减少
                rx_assert_if(m_count==0,m_tail==NULL);
                rx_assert_if(m_count!=0,m_tail!=NULL);
                return true;
            }
            return false;
        }
        //-----------------------------------------------------
        //查找指定key对应的节点
        template<class key_t>
        node_t *find(const key_t &key) const
        {
            node_t *node = m_head;                          //从首节点开始遍历
            for (int32_t lvl = m_levels - 1; lvl >= 0; --lvl)
            {//从高层向底层逐层查找
                while (node->next[lvl] != NULL)
                {//进行节点的比较
                    int cv=node_t::cmp(*node->next[lvl],key);//进行节点与key的比较
                    if (cv<0)
                        node = node->next[lvl];             //节点小于key,需要继续向后遍历
                    else if (cv==0)
                        return node->next[lvl];             //key相同,找到了.
                    else
                        break;                              //节点大于key说明本层查不到,准备降层吧
                }
            }
            return NULL;                                    //全部层级遍历完成,确实没找到
        }

        //-----------------------------------------------------
        //清空跳表,可以重新使用
        void clear()
        {
            node_t *node = m_head->next[0];                 //从头结点的后趋(首个节点)开始遍历
            while (node)
            {
                node_t *next = node->next[0];               //记录当前节点的后趋指针
                ct::OD(node);                               //节点析构
                m_mem.free(node);                           //释放当前节点
                node = next;                                //当前节点指针指向其后趋
            }

            for (uint32_t lvl = 0; lvl < m_levels; ++lvl)
                m_head->next[lvl] = NULL;                   //头结点的后趋全部清空

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
