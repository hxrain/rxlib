
#ifndef __RX_BIT_RAX_H_
#define __RX_BIT_RAX_H_

#include <assert.h> /* for assert */

#define LOG2(n)

//初始规格
//key的类型
#define key_t uint32_t

//中间层枝干节点的最大槽数,建议以cache line尺寸对齐
#define edge_slots_max_size (64 / sizeof(void*))

//衍生数据:
//key的最大比特数
#define key_max_bits (sizeof(key_t)*8)
//中间层枝干节点的槽位bit段数
#define edge_slots_bits LOG2(edge_slots_max_size)
//枝干节点的槽位分支掩码
#define edge_slots_mask (edge_slots_max_size - 1)
//顶层节点的槽位bit段数(下层节点槽位数量定长,用顶层槽位数来对齐key的剩余比特数)
#define top_slots_bits ((key_max_bits % edge_slots_bits) + edge_slots_bits)
//顶层节点的最大槽位数量
#define top_slots_max_size (1 << top_slots_bits)
//key在顶层槽位计算分支的位移数(相当于对顶层槽位bit段的掩码做与运算,或者说是对顶层槽位数量取模)
#define top_slots_shift (key_max_bits - top_slots_bits)
//当前配置下的rax树的最大层数
#define tree_max_levels (((key_max_bits - top_slots_bits) / edge_slots_bits)+1)

//最终的叶子节点类型
typedef struct node_t
{
    key_t       key;
    void       *data;
}node_t;

//中间枝干节点类型
typedef struct edge_t
{
    void* slots[edge_slots_max_size];
}edge_t;

//基于key的bit段计算分支的radix树
typedef struct bit_rax_t {
	void* slots[top_slots_max_size];        //顶层分支槽位
	uint32_t node_count;                    //叶子节点的数量
	uint32_t edge_count;                    //枝干节点的数量
} bit_rax_t;

//利用指针的最低位标记节点类型:0为叶子节点;1为枝干节点
#define is_edge_ptr(ptr) (((size_t)(ptr)) & 1)
#define get_edge_ptr(ptr) ((edge_t*)(((size_t)(ptr)) - 1))
#define set_edge_ptr(ptr) (void*)(((size_t)(ptr)) + 1)

#define allocator_alloc() malloc(sizeof(edge_t))
#define allocator_free(p) free(p)

void rax_init(bit_rax_t* rax)
{
	for (uint32_t i = 0; i < top_slots_max_size; ++i)
		rax->slots[i] = 0;

	rax->node_count = 0;
	rax->edge_count = 0;
}

static void m_insert(bit_rax_t* rax, uint32_t shift, node_t** let_ptr, node_t* insert, key_t key)
{
	edge_t* edge;
	node_t* node;
	void* ptr;
	uint32_t i;
	uint32_t j;

recurse:
	ptr = *let_ptr;

	/* if null, just insert the node */
	if (!ptr) {
		/* setup the node as a list */
		list_insert_first(let_ptr, insert);
		return;
	}

	if (is_edge_ptr(ptr)) {
		/* repeat the process one level down */
		let_ptr = &get_edge_ptr(ptr)->slots[(key >> shift) & edge_slots_mask];
		shift -= edge_slots_bits;
		goto recurse;
	}

	node = (node_t*)ptr;

	/* if it's the same key, insert in the list */
	if (node->key == key) {
		list_insert_tail_not_empty(node, insert);
		return;
	}

expand:
	/* convert to a edge */
	edge = (edge_t*)allocator_alloc();
	++rax->edge_count;
	*let_ptr = (node_t*)set_edge_ptr(edge);

	/* initialize it */
	for (i = 0; i < edge_slots_max_size; ++i)
		edge->slots[i] = 0;

	/* get the position of the two elements */
	i = (node->key >> shift) & edge_slots_mask;
	j = (key >> shift) & edge_slots_mask;

	/* if they don't collide */
	if (i != j) {
		/* insert the already existing element */
		edge->slots[i] = node;

		/* insert the new node */
		list_insert_first(&edge->slots[j], insert);
		return;
	}

	/* expand one more level */
	let_ptr = &edge->slots[i];
	shift -= edge_slots_bits;
	goto expand;
}

void rax_insert(bit_rax_t* rax, node_t* node, void* data, key_t key)
{
	node_t** let_ptr;

	node->data = data;
	node->key = key;

	let_ptr = &rax->slots[key >> top_slots_shift];

	m_insert(rax, top_slots_shift, let_ptr, node, key);

	++rax->node_count;
}

static node_t* m_remove(bit_rax_t* rax, uint32_t shift, node_t** let_ptr, node_t* remove, key_t key)
{
	node_t* node;
	edge_t* edge;
	void* ptr;
	node_t** let_back[tree_max_levels];
	uint32_t level;
	uint32_t i;
	uint32_t count;
	uint32_t last;

	level = 0;
recurse:
	ptr = *let_ptr;

	if (!ptr)
		return 0;

	if (is_edge_ptr(ptr)) {
		edge = get_edge_ptr(ptr);

		/* save the path */
		let_back[level++] = let_ptr;

		/* go down one level */
		let_ptr = &edge->slots[(key >> shift) & edge_slots_mask];
		shift -= edge_slots_bits;

		goto recurse;
	}

	node = (node_t*)ptr;

	/* if the node to remove is not specified */
	if (!remove) {
		/* remove the first */
		remove = node;

		/* check if it's really the element to remove */
		if (remove->key != key)
			return 0;
	}

	list_remove_existing(let_ptr, remove);

	/* if the list is not empty, try to reduce */
	if (*let_ptr || !level)
		return remove;

reduce:
	/* go one level up */
	let_ptr = let_back[--level];

	edge = get_edge_ptr(*let_ptr);

	/* check if there is only one child node */
	count = 0;
	last = 0;
	for (i = 0; i < edge_slots_max_size; ++i) {
		if (edge->slots[i]) {
			/* if we have a sub edge, we cannot reduce */
			if (is_edge_ptr(edge->slots[i]))
				return remove;
			/* if more than one node, we cannot reduce */
			if (++count > 1)
				return remove;
			last = i;
		}
	}

	/* here count is never 0, as we cannot have a edge with only one sub node */
	assert(count == 1);

	*let_ptr = edge->slots[last];

	allocator_free(edge);
	--rax->edge_count;

	/* repeat until more level */
	if (level)
		goto reduce;

	return remove;
}

void* rax_remove(bit_rax_t* rax, key_t key)
{
	node_t* ret;
	node_t** let_ptr;

	let_ptr = &rax->slots[key >> top_slots_shift];

	ret = m_remove(rax, top_slots_shift, let_ptr, 0, key);

	if (!ret)
		return 0;

	--rax->node_count;

	return ret->data;
}

void* rax_remove_existing(bit_rax_t* rax, node_t* node)
{
	node_t* ret;
	key_t key = node->key;
	node_t** let_ptr;

	let_ptr = &rax->slots[key >> top_slots_shift];

	ret = m_remove(rax, top_slots_shift, let_ptr, node, key);

	/* the element removed must match the one passed */
	assert(ret == node);

	--rax->node_count;

	return ret->data;
}

node_t* rax_find(bit_rax_t* rax, key_t key)
{
	node_t* node;
	void* ptr;
	uint32_t type;
	uint32_t shift;

	ptr = rax->slots[key >> top_slots_shift];

	shift = top_slots_shift;

recurse:
	if (!ptr)
		return 0;

	type = is_edge_ptr(ptr);

	switch (type) {
	case 0 :
		node = (node_t*)ptr;
		if (node->key != key)
			return 0;
		return node;
	default :
	case 1 :
		ptr = get_edge_ptr(ptr)->slots[(key >> shift) & edge_slots_mask];
		shift -= edge_slots_bits;
		goto recurse;
	}
}



#endif

