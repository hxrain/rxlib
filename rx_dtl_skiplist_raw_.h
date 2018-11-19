#ifndef _RX_RAW_SKIPLIST_H_
#define _RX_RAW_SKIPLIST_H_

#include "rx_cc_macro.h"
#include "rx_mem_alloc.h"
#include "rx_hash_rand.h"
#include <time.h>

namespace rx
{
    //原始跳表的最大层级数量
    #define RX_RAW_SKIPLIST_MAX_LEVELS 32

    //跳表的多层后趋指针数组,封装在跳表节点尾块中,降低代码膨胀.
    //尾块指针向回偏移sizeof(key_t)字节即可访问真正的跳表节点
    typedef struct sk_block_t
    {
        sk_block_t *next[1];
    }sk_block_t;

    /*
    //原始跳表的节点操作类,进行节点内容的析构与节点key的比较
    class skiplist_op_t
    {
    public:
        //比较节点与给定key的大小
        //返回值:n<key为<0;n==key为0;n>key为>0
        static int (*cmp)(const void *n,const &key);

        //析构整个节点,返回节点头指针
        static void* (*des)(const void *n);
    };

    //原始跳表的节点类型(示例,只要对外接口语义正确即可)
    template<class key_t>
    struct tiny_skipset_node_t
    {
        //-------------------------------------------------
        //计算需要的扩展尺寸
        template<class KT>
        static int ext_size(const KT &k){return 0;}

        //-------------------------------------------------
        //进行定向构造并初始化
        template<class val_t>
        void OC(uint32_t level,val_t &val,uint32_t es){ct::OC(&key,val);}
        //-------------------------------------------------
        typedef key_t node_key_t;
        node_key_t      key;
        sk_tail_block   tail;               //跳表实现的关键:分层的后趋节点指针,必须放在节点的最后,用于弹性扩展访问
    };
    */

    //统一指定一下跳表使用的随机数发生器类型
    typedef rand_skl_t skiplist_rnd_t;

    //-----------------------------------------------------
    //进行跳表层级随机数生成器的封装
    template<class rnd_t, uint32_t MAX_LEVEL>
    class skiplist_rnd_level
    {
        rnd_t   m_rnd;
    public:
        static const uint32_t max_level() { return MAX_LEVEL; }
        skiplist_rnd_level(uint32_t seed)
        {
            if (seed<1)
                seed = (uint32_t)time(NULL);
            m_rnd.seed(seed);
        }
        //-------------------------------------------------
        //生成一个随机的层数:>=1;<=最大层数
        uint32_t make()
        {
            uint32_t rc = 1;
            while (rc<MAX_LEVEL && (m_rnd.get() & 0xFFFFFF)<(0xFFFFFF >> 2))   //随机概率连续小于25%则层高增加(相当于4叉树)
                ++rc;
            rx_assert(rc <= MAX_LEVEL);
            return rc;
        }
    };

    //-----------------------------------------------------
    //定义原始的跳表容器基类
    template<skiplist_op_t op>
    class raw_skiplist_t
    {
        uint32_t            m_count;                        //内部节点数量
        uint32_t            m_levels;                       //当前已经使用的最高层数
        sk_block_t         *m_head;                         //指向头结点
        sk_block_t         *m_tail;                         //指向尾节点
        mem_allotter_i     &m_mem;                          //内存分配器接口
        uint32_t            m_rsv_size;                     //节点首部保留尺寸(在sk_block_t之前用于记录节点内容)
        //-------------------------------------------------
        //根据给定的层数与扩展尺寸,动态创建节点
        sk_block_t *m_make_node(uint32_t level,uint32_t rsv_size, uint32_t ext_size)
        {
            //计算节点尾块尺寸与需要扩展的尺寸
            uint32_t size=rsv_size + (level-1)*sizeof(sk_block_t*) + ext_size;
            //分配节点内存,但从节点尾块开始使用
            sk_block_t *block = (sk_block_t *)((uint8_t*)m_mem.alloc(size)+rsv_size);
            if (!block) return NULL;

            //新节点所有层的后趋,初始都为NULL
            for (uint32_t lvl = 0; lvl < level; ++lvl)
                block->next[lvl] = NULL;

            return block;
        }
        //-------------------------------------------------
        //摘除指定的节点,并更新其前趋节点的后趋指向
        void m_pick(sk_block_t *block, sk_block_t **update)
        {
            for (uint32_t lvl = 0; lvl < m_levels; ++lvl)
            {//从低向高逐层摘除处理
                if (update[lvl]->next[lvl] == block)         //将当前节点的前趋指向其后趋
                    update[lvl]->next[lvl] = block->next[lvl];
            }

            for (int32_t lvl = m_levels - 1; lvl >= 0; --lvl)
            {//再从高向低逐层判断是否应降低整体的层数
                if (m_head->next[lvl] == NULL)
                    --m_levels;                             //如果头结点的当前层后趋为NULL,则说明跳表的整体层数应该降低了
                else
                    break;
            }
        }
        //-------------------------------------------------
        //查找指定key在每层对应的前趋,并记到update中
        //返回值:key对应的最底层的前趋节点
        template<class key_t>
        sk_block_t *m_prev(const key_t &key, sk_block_t **update)
        {
            sk_block_t  *block = m_head;                    //从头节点开始向后查找
            for (int32_t lvl = m_levels - 1; lvl >= 0; --lvl)
            {//从最高层逐层降级查找,就是skiplist的算法精髓核心
                while (block->next[lvl] &&                  //如果当前节点有后趋
                       op::cmp(block->next[lvl],key)<0)     //并且当前节点的后趋小于key(说明key还应该往后放)
                       block = block->next[lvl];            //则当前节点后移,准备继续查找
                update[lvl] = block;                        //当前层查找结束了,记录当前节点为指定key位置的前趋
            }
            return block;
        }
    public:
        //-------------------------------------------------
        raw_skiplist_t(mem_allotter_i &ma,uint32_t rsv_size,uint32_t head_levels=RX_RAW_SKIPLIST_MAX_LEVELS):m_mem(ma),m_rsv_size(rsv_size)
        {
            m_levels = 1;                                   //跳表的初始层数只有1层(索引为0的基础层)
            m_head = m_make_node(head_levels,0,0);          //头结点只分配内存,不构造
            rx_fail(m_head!=NULL);
            m_tail=NULL;
            m_count=0;
        }
        virtual ~raw_skiplist_t()
        {
            clear();
            m_mem.free(m_head);                             //释放头结点内存,不析构
        }
        //-------------------------------------------------
        //当前跳表中的节点数量
        uint32_t size() const {return m_count;}
        //当前跳表的最大层数
        uint32_t levels() const {return m_levels;}
        //获取当前跳表的头与尾节点
        sk_block_t* head() const {return m_head->next[0];}
        sk_block_t* tail() const {return m_tail;}
        //-------------------------------------------------
        //插入指定的key对应的节点(key不可重复,否则会返回之前存在的节点指针);需给定本节点的层数level;给定扩展的内存尺寸ext_size;
        //返回的节点指针如果不为空,且duplication重复指示为假,则可以进行节点的构造初始化(key与val的真正初始化)
        template<class key_t>
        void *insert_raw(const key_t &key,bool &duplication,uint32_t ext_size,const uint32_t level)
        {
            sk_block_t *update[RX_RAW_SKIPLIST_MAX_LEVELS]; //用于临时记录当前节点操作中,对应的各层前趋节点
            sk_block_t *prv=m_prev(key,update)->next[0];    //查找指定key对应的各层前趋,并尝试得到已经存在的key节点

            if (prv&&op::cmp((uint8_t*)prv-m_rsv_size,key)==0)
            {
                duplication=true;                           //告知key已存在
                return prv;                                 //直接返回
            }

            duplication=false;
            sk_block_t *block = m_make_node(level,m_rsv_size,ext_size);//创建含有指定层数的新节点
            if (!block)
                return NULL;

            if (level > m_levels)
            {//如果新节点的层数大于原有层数
                for (uint32_t lvl = m_levels; lvl < level; ++lvl)
                    update[lvl] = m_head;                   //则填充新增层的前趋为头结点
                m_levels = level;                           //更新最大层高
            }

            for (uint32_t lvl = 0; lvl < level; ++lvl)
            {//开始逐层挂接新节点
                block->next[lvl] = update[lvl]->next[lvl];  //新节点的后趋指向前趋节点的后趋
                update[lvl]->next[lvl] = block;             //前趋节点的后趋指向新节点
            }

            if (m_count++==0||block->next[0]==NULL)         //如果跳表是空的,或者最新插入的节点就是尾节点(没有后趋)
                m_tail=block;                               //则尾节点指针就应该指向最新节点

            return (uint8_t*)block - m_rsv_size;            //返回节点首指针
        }
        //-------------------------------------------------
        //直接删除给定key对应的一个节点(多个相同key节点存在时,先找到哪个删除哪个)
        template<class key_t>
        bool earse(const key_t &key)
        {
            sk_block_t *update[RX_RAW_SKIPLIST_MAX_LEVELS]; //用于临时记录当前节点操作时,对应的各层前趋节点
            sk_block_t *block = m_prev(key,update)->next[0];//查找指定key节点对应的各层前趋,并尝试得到对应的节点
            void *node=(uint8_t*)block-m_rsv_size;
            if (block && sk_block_t::cmp(node, key)==0)
            {//如果对应节点存在且key相同,则说明找到了此节点
                m_pick(block, update);                      //摘除指定的节点(进行节点各层前趋的调整)
                if (block==m_tail)
                    m_tail=update[0];                       //如果删除的节点就是尾节点,则尾节点指向其前趋
                m_mem.free(op::des(node));                  //节点析构,之后释放内存
                --m_count;                                  //跳表计数减少
                rx_assert_if(m_count==0,m_tail==NULL);
                rx_assert_if(m_count!=0,m_tail!=NULL);
                return true;
            }
            return false;
        }
        //-------------------------------------------------
        //查找指定key对应的节点
        template<class key_t>
        sk_block_t *find(const key_t &key) const
        {
            sk_block_t *block = m_head;                     //从首节点开始遍历
            for (int32_t lvl = m_levels - 1; lvl >= 0; --lvl)
            {//从高层向底层逐层查找
                while (block->next[lvl] != NULL)
                {//进行节点的比较
                    int cv=op::cmp((uint8_t*)block->next[lvl]-m_rsv_size,key);
                    if (cv<0)
                        block = block->next[lvl];           //节点小于key,需要继续向后遍历
                    else if (cv==0)
                        return block->next[lvl];            //key相同,找到了.
                    else
                        break;                              //节点大于key说明本层查不到,准备降层吧
                }
            }
            return NULL;                                    //全部层级遍历完成,确实没找到
        }
        //-------------------------------------------------
        //清空跳表,可以重新使用
        void clear()
        {
            sk_block_t *block = m_head->next[0];            //从头结点的后趋(首个节点)开始遍历
            while (block)
            {
                sk_block_t *next = block->next[0];          //记录当前节点的后趋指针
                m_mem.free(op::des((uint8_t*)block-m_rsv_size));//节点析构,并释放当前节点
                block = next;                               //当前节点指针指向其后趋
            }

            for (uint32_t lvl = 0; lvl < m_levels; ++lvl)
                m_head->next[lvl] = NULL;                   //头结点的后趋全部清空

            m_levels=1;
            m_count=0;
            m_tail=NULL;
        }
    };




}

#endif
