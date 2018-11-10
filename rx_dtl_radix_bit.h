
#ifndef __RX_BIT_RAX_H_
#define __RX_BIT_RAX_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"
#include "rx_assert.h"
#include "rx_mem_alloc.h"

namespace rx
{

    //初始规格
    //key的类型
    #define rax_key_t uint32_t
    //中间层枝干节点的最大槽数,建议以cache line尺寸对齐
    #define limb_slots_max_size (64 / sizeof(void*))

    //衍生数据:
    //key的最大比特数
    #define key_max_bits (sizeof(rax_key_t)*8)
    //中间层枝干节点的槽位bit段数
    #define limb_slots_bits LOG2<limb_slots_max_size>::result
    //枝干节点的槽位分支掩码
    #define limb_slots_mask (limb_slots_max_size - 1)
    //顶层节点的槽位bit段数(下层节点槽位数量定长,用顶层槽位数来对齐key的剩余比特数)
    #define top_slots_bits ((key_max_bits % limb_slots_bits) + limb_slots_bits)
    //顶层节点的最大槽位数量
    #define top_slots_max_size (1 << top_slots_bits)
    //key在顶层槽位计算分支的位移数(相当于对顶层槽位bit段的掩码做与运算,或者说是对顶层槽位数量取模)
    #define top_slots_shift (key_max_bits - top_slots_bits)
    //当前配置下的rax树的最大层数
    #define tree_max_levels (((key_max_bits - top_slots_bits) / limb_slots_bits)+1)

    //最终的叶子节点基类
    typedef struct leaf_t
    {
        rax_key_t   key;
    }leaf_t;

    //分支指针类型
    typedef void* slot_t;

    //中间枝干节点类型基类
    typedef struct limb_t
    {
        slot_t slots[1];                        //每个枝干节点的槽位记录分支指针,实际尺寸是limb_slots_max_size
    }limb_t;

    //基于key的bit段计算分支的radix树
    typedef struct radix_bit_t {
        uint32_t leaf_count;                    //叶子节点的数量
        uint32_t limb_count;                    //枝干节点的数量
        uint32_t limb_slot_size;                //枝干节点槽位数量
        uint32_t limb_slot_mask;                //枝干节点槽位掩码
        uint32_t limb_slot_bits;                //枝干节点槽位比特数
        uint32_t top_slot_size;                 //顶层节点槽位数量
        slot_t slots[top_slots_max_size];       //顶层分支槽位数组
    } radix_bit_t;

    //利用指针的最低位标记节点类型:0为叶子节点;1为枝干节点
    inline bool    is_limb_ptr(void *ptr) {return ((size_t)(ptr)) & 1;}
    inline limb_t* get_limb_ptr(void *ptr) { return (limb_t*)(((size_t)(ptr)) - 1); }
    inline void*   set_limb_ptr(void *ptr) { return (void*)(((size_t)(ptr)) + 1); }

    #define allocator_alloc() malloc(sizeof(slot_t)*rax->limb_slot_size)
    #define allocator_free(p) free(p)

    void rax_init(radix_bit_t* rax)
    {
        for (uint32_t i = 0; i < top_slots_max_size; ++i)
            rax->slots[i] = 0;

        rax->leaf_count = 0;
        rax->limb_count = 0;
        rax->limb_slot_bits = limb_slots_bits;
        rax->limb_slot_mask = limb_slots_mask;
        rax->limb_slot_size = limb_slots_max_size;
    }

    //从上至下遍历rax路径,尝试将key对应的叶子节点插入到适当的位置
    static slot_t* m_insert(radix_bit_t* rax, int32_t shift, slot_t *slot_ptr, bool &is_dup, rax_key_t key)
    {
        is_dup = false;
        leaf_t* leaf;
        //先尝试查找适当的分支指向
        while(1)
        {
            //取出当前的槽位指向
            slot_t ptr = *slot_ptr;
            if (!ptr) 
            {//当前槽位指向为空,直接记录新的待插入节点
                //*slot_ptr=insert;
                return slot_ptr;
            }

            if (is_limb_ptr(ptr)) 
            {//如果当前槽位指向的是中间的枝干,继续向下深入
                //计算向下行走的槽位索引
                uint32_t slot_idx=(key >> shift) & rax->limb_slot_mask;
                //获取向下行走的槽位指针
                slot_ptr = &get_limb_ptr(ptr)->slots[slot_idx];
                //调整槽位索引位移量
                shift -= rax->limb_slot_bits;
                rx_assert(shift>=0&&shift%rax->limb_slot_bits==0);
            }
            else
            {//当前的槽位指向已经是叶子节点了,查找结束
                leaf = (leaf_t*)ptr;
                break;
            }
        }

        if (leaf->key == key) 
        {//如果当前叶子节点的key与待插入节点的key相同,说明遇到重复key节点了
            ////list_insert_tail_not_empty(leaf, insert);
            is_dup = true;
            return slot_ptr;
        }

        //否则需要进行中间叶子节点的层级下移,将叶子节点放入正确的位置
        while(1)
        {
            //分配一个新的枝干
            limb_t *limb = (limb_t*)allocator_alloc();
            for (uint32_t i = 0; i < rax->limb_slot_size; ++i)
                limb->slots[i] = 0;
            ++rax->limb_count;

            //原来的槽位指向需要校正为新的枝干
            *slot_ptr = (leaf_t*)set_limb_ptr(limb);

            //计算中间叶子节点key在当前枝干的槽位索引
            uint32_t leaf_slot_idx = (leaf->key >> shift) & rax->limb_slot_mask;
            //计算目标key在当前枝干的槽位索引
            uint32_t dst_slot_idx = (key >> shift) & rax->limb_slot_mask;

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
                shift -= rax->limb_slot_bits;
                rx_assert(shift>=0&&shift%rax->limb_slot_bits==0);
            }
        }
    }

    void rax_insert(radix_bit_t* rax, leaf_t* node,rax_key_t key)
    {
        bool is_dup;
        slot_t *slot=m_insert(rax, top_slots_shift, &rax->slots[key >> top_slots_shift], is_dup, key);
        if (is_dup) return;

        node->key = key;
        *slot = node;
        ++rax->leaf_count;
    }

    static leaf_t* m_remove(radix_bit_t* rax, int32_t shift, slot_t* slot_ptr, rax_key_t key)
    {
        //记录回归路径,用于自底向上进行枝干收缩
        slot_t *back_path[tree_max_levels];
        uint32_t level = 0;

        slot_t ptr;

        while (1)
        {
            //获取当前的槽位指向
            ptr = *slot_ptr;
            //如果指向为空则目标不存在
            if (!ptr) return 0;

            //如果指向不是枝干则下行结束
            if (!is_limb_ptr(ptr))
                break;

            //记录当前槽位指针,作为回归路径
            back_path[level++] = slot_ptr;

            //获取枝干节点指针
            limb_t *limb = get_limb_ptr(ptr);
            //继续向下层深入
            uint32_t slot_idx = (key >> shift) & rax->limb_slot_mask;
            slot_ptr = &limb->slots[slot_idx];
            shift -= rax->limb_slot_bits;
        }

        //得到了目标叶子节点,需要判断目标节点与指定的key是否相同
        leaf_t* node = (leaf_t*)ptr;
        if (node->key != key)
            return 0;

        //清空最后的槽位指向,摘除目标节点
        *slot_ptr = 0;

        //删除了叶子,需要尝试进行枝干的收缩处理
        while (level)
        {
            //得到上层枝干节点槽位指针
            slot_ptr = back_path[--level];

            //获取上层枝干节点槽位指向的下层枝干节点
            limb_t *limb = get_limb_ptr(*slot_ptr);

            /* check if there is only one child node */
            uint32_t count = 0;
            uint32_t last = 0;
            //遍历当前枝干的槽位,尝试找到还剩余的唯一的其他叶子
            for (uint32_t i = 0; i < limb_slots_max_size; ++i)
            {
                if (!limb->slots[i]) continue;

                //如果当前枝干的槽位还指向了其他枝干,放弃收缩
                if (is_limb_ptr(limb->slots[i]))
                    return node;

                //如果当前枝干的槽位还指向多个叶子,放弃收缩
                if (++count > 1)
                    return node;

                //记录当前槽位索引,上述条件都不满足的时候,此槽位就是当前叶子节点在上层枝干槽位上的位置
                last = i;
            }

            /* here count is never 0, as we cannot have a limb with only one sub node */
            rx_assert(count == 1);

            //调整上层枝干节点槽位指向,记录剩余的唯一的叶子节点
            *slot_ptr = limb->slots[last];

            //释放当前枝干节点
            allocator_free(limb);
            --rax->limb_count;
        }
        return node;
    }

    //从树中摘除指定key对应的节点
    leaf_t* rax_remove(radix_bit_t* rax, rax_key_t key)
    {
         leaf_t *ret = m_remove(rax, top_slots_shift, &rax->slots[key >> top_slots_shift],key);

        if (!ret)
            return 0;

        --rax->leaf_count;

        return ret;
    }

    leaf_t* m_find(radix_bit_t* rax, int32_t shift, slot_t ptr,rax_key_t key)
    {
        while (1)
        {
            //槽位指向空,找不到
            if (!ptr) return 0;

            if (is_limb_ptr(ptr))
            {//槽位指向枝干节点,需要下移
                ptr = get_limb_ptr(ptr)->slots[(key >> shift) & rax->limb_slot_mask];
                shift -= rax->limb_slot_bits;
            }
            else
            {//找到叶子节点了
                leaf_t *node = (leaf_t*)ptr;
                //如果已经存在的叶子节点与目标key不符,则说明目标key不存在
                if (node->key != key)
                    return 0;
                //否则代表确实找到了目标key对应的节点
                return node;
            }
        }
    }
    leaf_t* rax_find(radix_bit_t* rax, rax_key_t key)
    {
        return m_find(rax, top_slots_shift, rax->slots[key >> top_slots_shift], key);
    }
}



#endif

