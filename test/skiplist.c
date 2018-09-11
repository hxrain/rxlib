#include "skiplist.h"
#include <stdio.h>
static node *create_node(int level, int key, object *obj)
{
	int i;
	node *nd = (node *)malloc(sizeof(node) + level * sizeof(node *));
	nd->obj = obj;
	nd->key = key;
	
	for (i = 0; i < level; i++) {
		nd->forward[i] = NULL;
	}

	return nd;
}

skiplist *create_skiplist(void)
{
	skiplist *sl = (skiplist *)malloc(sizeof(skiplist));
	sl->head = create_node(MAX_LEVEL, 0, NULL);
	sl->level = 1;
	return sl;
}

static void free_node(node *nd)
{
	free(nd);
}

void free_skiplist(skiplist *sl)
{
	node *nd, *next;

	nd = sl->head->forward[0];
	free_node(sl->head);

	while (nd) {
		next = nd->forward[0];
		free_node(nd);
		nd = next;
	}

	free(sl);
}

static int random_level()
{
	int level = 1;
	while ((rand() & 0x0FFF) < (0x0FFF>>1)&&level<MAX_LEVEL) {
		level += 1;
	}
	return (level < MAX_LEVEL) ? level : MAX_LEVEL;
}

void insert(skiplist *sl, int key, object *obj)
{
	node  *update[MAX_LEVEL];
	node  *nd;

	int i, level;

	nd = sl->head;

	for (i = sl->level - 1; i >= 0; i--) {
		while (nd->forward[i] != NULL && nd->forward[i]->key < key)
			nd = nd->forward[i];
		update[i] = nd;
	}

	level = random_level();
	if (level > sl->level) {
		for (i = sl->level; i < level; i++) {
			update[i] = sl->head;
		}
		sl->level = level;
	}
	
	nd = create_node(level, key, obj);
	for (i = 0; i < level; i++) {
		nd->forward[i] = update[i]->forward[i];
		update[i]->forward[i] = nd;
	}
}	

static void delete_node(skiplist *sl, node *nd, node **update)
{
	int i;
	for (i = 0; i < sl->level; i++) {
		if (update[i]->forward[i] == nd) {
			update[i]->forward[i] = nd->forward[i];
		}
	}

	for (i = i - 1; i >= 0; i--) {
		if (sl->head->forward[i] == NULL) {
			sl->level--;
		}
	}
}

void earse(skiplist *sl, int key)
{
	node *update[MAX_LEVEL], *nd;
	int i;

	nd = sl->head;
	for (i = sl->level - 1; i >= 0; i--) {
		while (nd->forward[i] && nd->forward[i]->key < key) {
			nd = nd->forward[i];
		}
		update[i] = nd;
	}

	nd = nd->forward[0];

	if (nd && nd->key == key) {
		delete_node(sl, nd, update);
		free_node(nd);
	}
}

node *find(skiplist *sl, int key)
{
	node *nd;
	int i;

	nd = sl->head;
	for (i = sl->level - 1; i >= 0; i--) {
		while (nd->forward[i] != NULL) {
			if (nd->forward[i]->key < key) 
				nd = nd->forward[i];
			else if (nd->forward[i]->key == key)
				return nd->forward[i];
			else
				break;
		}
	}

	printf("Oops... %d not found\n", key);
	return NULL;
}

void print(skiplist *sl)
{
	node *nd;
	int i;

	for (i = 0; i < MAX_LEVEL; i++) {
		nd = sl->head->forward[i];
		printf("Level[%d]:", i);

		while (nd) {
			printf("%d->", nd->key);
			nd = nd->forward[i];
		}
		printf("\n");
	}
}

