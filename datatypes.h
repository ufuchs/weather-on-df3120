#ifndef __H_DATATYPES__
#define __H_DATATYPES__

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

struct LList_t {
	int id;
	int type;
	void *data;
	struct LList_t *next;
};


void llist_set_type(struct LList_t *node, int type);
int llist_get_type(struct LList_t *node);
void llist_set_data(struct LList_t *node, void *data);
void *llist_get_data(struct LList_t *node);
struct LList_t *llist_insert_node(struct LList_t **head, struct LList_t *node);
struct LList_t *llist_add_node(struct LList_t **head, int id, void *data);
int llist_del_node(struct LList_t **head, int id);
struct LList_t *llist_find_node(struct LList_t *head, int id);
void llist_print_node(struct LList_t *node);
void llist_all_nodes(struct LList_t *head);

#endif
