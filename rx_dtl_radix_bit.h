
#ifndef __RX_BIT_RAX_H_
#define __RX_BIT_RAX_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"
#include "rx_assert.h"

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

    //最终的叶子节点类型
    typedef struct leaf_t
    {
        rax_key_t   key;
        void       *data;
    }leaf_t;

    //分支指针类型
    typedef void* slot_t;

    //中间枝干节点类型
    typedef struct limb_t
    {
        slot_t slots[limb_slots_max_size];      //每个枝干节点的槽位记录分支指针
    }limb_t;

    //基于key的bit段计算分支的radix树
    typedef struct bit_rax_t {
        slot_t slots[top_slots_max_size];       //顶层分支槽位
        uint32_t leaf_count;                    //叶子节点的数量
        uint32_t limb_count;                    //枝干节点的数量
    } bit_rax_t;

    //利用指针的最低位标记节点类型:0为叶子节点;1为枝干节点
    #define is_limb_ptr(ptr) (((size_t)(ptr)) & 1)
    #define get_limb_ptr(ptr) ((limb_t*)(((size_t)(ptr)) - 1))
    #define set_limb_ptr(ptr) (void*)(((size_t)(ptr)) + 1)

    #define allocator_alloc() malloc(sizeof(limb_t))
    #define allocator_free(p) free(p)

    void rax_init(bit_rax_t* rax)
    {
        for (uint32_t i = 0; i < top_slots_max_size; ++i)
            rax->slots[i] = 0;

        rax->leaf_count = 0;
        rax->limb_count = 0;
    }

    //遍历
    static slot_t* m_insert(bit_rax_t* rax, int32_t shift, slot_t *slot_ptr, leaf_t* insert, rax_key_t key)
    {
        leaf_t* leaf;
        //先尝试查找适当的分支指向
        while(1)
        {
            //取出当前的槽位指向
            slot_t ptr = *slot_ptr;
            if (!ptr) 
            {//当前槽位指向为空,直接记录新的待插入节点
                *slot_ptr=insert;
                return slot_ptr;
            }

            if (is_limb_ptr(ptr)) 
            {//如果当前槽位指向的是中间的枝干,继续向下深入
                //计算向下行走的槽位索引
                uint32_t slot_idx=(key >> shift) & limb_slots_mask;
                //获取向下行走的槽位指针
                slot_ptr = &get_limb_ptr(ptr)->slots[slot_idx];
                //调整槽位索引位移量
                shift -= limb_slots_bits;
                rx_assert(shift>=0&&shift%limb_slots_bits==0);
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
            return slot_ptr;
        }

        //否则需要进行中间叶子节点的层级下移,将叶子节点放入正确的位置
        while(1)
        {
            //分配一个新的枝干
            limb_t *limb = (limb_t*)allocator_alloc();
            for (uint32_t i = 0; i < limb_slots_max_size; ++i)
                limb->slots[i] = 0;
            ++rax->limb_count;

            //原来的槽位指向需要校正为新的枝干
            *slot_ptr = (leaf_t*)set_limb_ptr(limb);

            //计算中间叶子节点key在当前枝干的槽位索引
            uint32_t leaf_slot_idx = (leaf->key >> shift) & limb_slots_mask;
            //计算目标key在当前枝干的槽位索引
            uint32_t dst_slot_idx = (key >> shift) & limb_slots_mask;

            //根据原有叶子key与目标key的槽位冲突情况,判断处理方式
            if (leaf_slot_idx != dst_slot_idx) 
            {//槽位不冲突,查找结束
                //原有的叶子节点放到当前枝干的正确槽位上
                limb->slots[leaf_slot_idx] = leaf;
                //目标key对应的节点放在当前枝干的正确槽位上
                limb->slots[dst_slot_idx] = insert;
                //返回目标槽位指针
                return &limb->slots[dst_slot_idx];
            }
            else
            {//槽位冲突,当前叶子节点的位置需要继续下移
                slot_ptr = &limb->slots[leaf_slot_idx];
                shift -= limb_slots_bits;
                rx_assert(shift>=0&&shift%limb_slots_bits==0);
            }
        }
    }

    void rax_insert(bit_rax_t* rax, leaf_t* node,rax_key_t key)
    {
        node->key = key;
        m_insert(rax, top_slots_shift, &rax->slots[key >> top_slots_shift], node, key);
        ++rax->leaf_count;
    }

    static leaf_t* m_remove(bit_rax_t* rax, uint32_t shift, slot_t* slot_ptr, leaf_t* remove, rax_key_t key)
    {
        leaf_t* node;
        limb_t* limb;
        void* ptr;
        void** let_back[tree_max_levels];
        uint32_t level;
        uint32_t i;
        uint32_t count;
        uint32_t last;

        level = 0;
recurse:
        ptr = *slot_ptr;

        if (!ptr)
            return 0;

        if (is_limb_ptr(ptr)) {
            limb = get_limb_ptr(ptr);

            /* save the path */
            let_back[level++] = slot_ptr;

            /* go down one level */
            uint32_t slot_idx=(key >> shift) & limb_slots_mask;
            slot_ptr = &limb->slots[slot_idx];
            shift -= limb_slots_bits;

            goto recurse;
        }

        node = (leaf_t*)ptr;

        /* if the node to remove is not specified */
        if (!remove) {
            /* remove the first */
            remove = node;

            /* check if it's really the element to remove */
            if (remove->key != key)
                return 0;
        }

        ////list_remove_existing(slot_ptr, remove);

        /* if the list is not empty, try to reduce */
        if (*slot_ptr || !level)
            return remove;

reduce:
        /* go one level up */
        slot_ptr = let_back[--level];

        limb = get_limb_ptr(*slot_ptr);

        /* check if there is only one child node */
        count = 0;
        last = 0;
        for (i = 0; i < limb_slots_max_size; ++i) {
            if (limb->slots[i]) {
                /* if we have a sub limb, we cannot reduce */
                if (is_limb_ptr(limb->slots[i]))
                    return remove;
                /* if more than one node, we cannot reduce */
                if (++count > 1)
                    return remove;
                last = i;
            }
        }

        /* here count is never 0, as we cannot have a limb with only one sub node */
        rx_assert(count == 1);

        *slot_ptr = limb->slots[last];

        allocator_free(limb);
        --rax->limb_count;

        /* repeat until more level */
        if (level)
            goto reduce;

        return remove;
    }

    void* rax_remove(bit_rax_t* rax, rax_key_t key)
    {
        leaf_t* ret;
        slot_t* slot_ptr;

        slot_ptr = &rax->slots[key >> top_slots_shift];

        ret = m_remove(rax, top_slots_shift, slot_ptr, 0, key);

        if (!ret)
            return 0;

        --rax->leaf_count;

        return ret->data;
    }

    void* rax_remove_existing(bit_rax_t* rax, leaf_t* node)
    {
        leaf_t* ret;
        rax_key_t key = node->key;
        slot_t* slot_ptr;

        slot_ptr = &rax->slots[key >> top_slots_shift];

        ret = m_remove(rax, top_slots_shift, slot_ptr, node, key);

        /* the element removed must match the one passed */
        rx_assert(ret == node);

        --rax->leaf_count;

        return ret->data;
    }

    leaf_t* rax_find(bit_rax_t* rax, rax_key_t key)
    {
        leaf_t* node;
        void* ptr;
        uint32_t type;
        uint32_t shift;

        ptr = rax->slots[key >> top_slots_shift];

        shift = top_slots_shift;

recurse:
        if (!ptr)
            return 0;

        type = is_limb_ptr(ptr);

        switch (type) {
        case 0 :
            node = (leaf_t*)ptr;
            if (node->key != key)
                return 0;
            return node;
        default :
        case 1 :
            ptr = get_limb_ptr(ptr)->slots[(key >> shift) & limb_slots_mask];
            shift -= limb_slots_bits;
            goto recurse;
        }
    }
}



#endif

