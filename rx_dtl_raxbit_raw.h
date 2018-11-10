
#ifndef __RX_RAXBIT_RAW_H_
#define __RX_RAXBIT_RAW_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"
#include "rx_assert.h"
#include "rx_mem_alloc_cntr.h"

namespace rx
{
    //-----------------------------------------------------
    //bit分段radix树的key操作功能基类
    template<typename key_t>
    class raxbit_op_base
    {
    public:
        //叶子节点类型基类,仅含有key
        typedef struct leaf_base_t
        {
            key_t key;
        }leaf_base_t;

        //key的最大比特数
        enum { key_max_bits = sizeof(key_t) * 8 };
        //枝干节点的最大槽数,建议以cache line尺寸对齐
        enum { limb_slots_max_size = (64 / sizeof(void*)) };
        //枝干节点的槽位bit段数
        enum { limb_slots_bits = LOG2<limb_slots_max_size>::result };
        //枝干节点的槽位分支掩码
        enum { limb_slots_mask = (limb_slots_max_size - 1) };

        //顶层节点的槽位bit段数(下层节点槽位数量定长,用顶层槽位数来对齐key的剩余比特数)
        enum { top_slots_bits = ((key_max_bits % limb_slots_bits) + limb_slots_bits) };
        //顶层节点的最大槽位数量
        enum { top_slots_max_size = (1 << top_slots_bits) };
        //顶层节点的的槽位计算分支的位移数(相当于对顶层槽位bit段的掩码做与运算,或者说是对顶层槽位数量取模)
        enum { top_slots_shift = (key_max_bits - top_slots_bits) };
        //当前配置下的rax树的最大层数
        enum { tree_max_levels = (((key_max_bits - top_slots_bits) / limb_slots_bits) + 1) };
        //计算指定的key与节点是否相等
        template<typename KT>
        static bool equ(const KT& key, const leaf_base_t& leaf) { return key == leaf.key; }
    };

    //-----------------------------------------------------
    //整数类型key的操作方法
    template<typename key_t>
    class raxbit_op_int:public raxbit_op_base<key_t>
    {
        typedef raxbit_op_base<key_t> super_t;
    public:
        //计算枝干槽位索引
        static uint32_t limb_slot_idx(const key_t& key, uint32_t shift) { return (key >> shift) & super_t::limb_slot_mask; }
        //计算顶级槽位索引
        static uint32_t top_slot_idx(const key_t& key) { return key >> super_t::top_slots_shift; }
    };

    //-----------------------------------------------------
    //封装基础的bit分段基数树,代码膨胀只依赖key的类型
    template<class KT, class OP>
    class raw_raxbit_t
    {
    public:
        //-------------------------------------------------
        //叶子类型基类
        typedef typename OP::leaf_base_t leaf_t;

        //槽位分支类型(使用void*便于指向limb_t或leaf_t)
        typedef void* slot_t;

        //中间枝干节点类型基类
        typedef struct limb_t
        {
            slot_t slots[OP::limb_slots_max_size];          //每个枝干节点的槽位记录分支指针
        }limb_t;
    private:
        uint32_t        m_leaf_count;                       //叶子节点的数量
        uint32_t        m_limb_count;                       //枝干节点的数量
        mem_allotter_i &m_mem;                              //内存分配器接口
        slot_t          m_slots[OP::top_slots_max_size];    //顶层分支槽位数组

        //-------------------------------------------------
        //枝干指针操作
        bool    is_limb_ptr(void *ptr) { return ((size_t)(ptr)) & 1; }
        limb_t* get_limb_ptr(void *ptr) { return (limb_t*)(((size_t)(ptr)) - 1); }
        void*   set_limb_ptr(void *ptr) { return (void*)(((size_t)(ptr)) + 1); }

        //-------------------------------------------------
        //内存分配操作
        limb_t* limb_alloc() { return (limb_t*)m_mem.alloc(sizeof(limb_t)); }
        leaf_t* leaf_alloc(uint32_t ext_size) { return (leaf_t*)m_mem.alloc(sizeof(leaf_t)+ext_size); }
        void node_free(void* p) { m_mem.free(p); }

        //-------------------------------------------------
        //遍历rax路径,找到或创建key对应的叶子节点的槽位:shift剩余的槽位索引计算位移量;slot_ptr指向枝干槽位指针;is_dup告知key是否重复.
        //返回值:NULL内存不足;其他为key对应的槽位指针
        slot_t* m_insert(int32_t shift, slot_t *slot_ptr, bool &is_dup, const KT& key)
        {
            leaf_t* leaf;
            //先尝试查找适当的分支指向
            while (1)
            {
                //取出当前的槽位指向
                slot_t ptr = *slot_ptr;

                //当前槽位指向如果为空,可直接用于记录新的待插入节点
                if (!ptr)
                    return slot_ptr;

                if (!is_limb_ptr(ptr))
                {//当前的槽位指向已经是叶子节点了,查找结束
                    leaf = (leaf_t*)ptr;
                    break;
                }

                //如果当前槽位指向的是中间的枝干,继续向下深入
                uint32_t slot_idx = OP::limb_slot_idx(key,shift);
                //获取向下行走的槽位指针
                slot_ptr = &get_limb_ptr(ptr)->slots[slot_idx];
                //调整槽位索引位移量
                shift -= OP::limb_slot_bits;
                rx_assert(shift >= 0 && shift%OP::limb_slot_bits == 0);
            }

            if (OP::equ(key,*leaf))
            {//如果当前叶子节点的key与待插入节点的key相同,说明遇到重复key节点了
                is_dup = true;
                return slot_ptr;
            }

            //否则需要进行中间叶子节点的层级下移,将叶子节点放入正确的位置
            while (1)
            {
                //分配一个新的枝干
                limb_t *limb = limb_alloc();
                if (!limb) return NULL;

                ++m_limb_count;
                for (uint32_t i = 0; i < OP::limb_slot_size; ++i)
                    limb->slots[i] = NULL;

                //原来的槽位指向需要校正为新的枝干
                *slot_ptr = set_limb_ptr(limb);

                //计算中间叶子节点key在当前枝干的槽位索引
                uint32_t leaf_slot_idx = OP::limb_slot_idx(leaf->key,shift);
                //计算目标key在当前枝干的槽位索引
                uint32_t dst_slot_idx = OP::limb_slot_idx(key , shift);

                //根据原有叶子key与目标key的槽位冲突情况,判断处理方式
                if (leaf_slot_idx != dst_slot_idx)
                {//槽位不冲突,查找结束
                 //原有的叶子节点放到当前枝干的正确槽位上
                    limb->slots[leaf_slot_idx] = leaf;
                    //目标key对应的节点放在当前枝干的正确槽位上
                    //limb->slots[dst_slot_idx] = insert;
                    //返回目标槽位指针
                    return &limb->slots[dst_slot_idx];
                }
                else
                {//槽位冲突,当前叶子节点的位置需要继续下移
                    slot_ptr = &limb->slots[leaf_slot_idx];
                    shift -= OP::limb_slot_bits;
                    rx_assert(shift >= 0 && shift%OP::limb_slot_bits == 0);
                }
            }
            return NULL;
        }

    public:
        //-------------------------------------------------
        raw_raxbit_t():m_mem(rx_global_mem_allotter()), m_leaf_count(0), m_limb_count(0){}
        raw_raxbit_t(mem_allotter_i &m):m_mem(m), m_leaf_count(0), m_limb_count(0) {}
        //-------------------------------------------------
        //叶子节点的数量
        uint32_t size() const { return m_leaf_count; }
        //-------------------------------------------------
        //根据指定的key找到或插入叶子节点:目标key;data_size告知额外需要的数据尺寸
        //返回值:NULL内存不足;其他为叶子节点指针
        leaf_t* insert(const KT &key, bool &is_dup,uint32_t data_size=0)
        {
            is_dup=false;
            slot_t *slot = m_insert(OP::top_slots_shift, &m_slots[OP::top_slot_idx(key)], is_dup, key);
            if (is_dup)
                return (leaf_t*)*slot;                      //重复key直接返回
            if (slot == NULL)
                return NULL;                                //内存不足

            leaf_t *leaf = leaf_alloc(data_size);           //新的key,需要创建叶子节点
            if (leaf == NULL)
                return NULL;                                //内存不足

            //leaf->key = key;                              //原始容器,key的赋值动作也不执行,需要外面处理
            *slot = leaf;                                   //让目标槽位指向正确的叶子节点
            ++m_leaf_count;
            return leaf;
        }
    };



}



#endif

