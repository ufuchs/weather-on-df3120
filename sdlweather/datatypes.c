#include "datatypes.h"

void llist_set_type(struct LList_t *node, int type) {
	node->type = type;
}

int llist_get_type(struct LList_t *node) {
	return node->type;
}

void llist_set_data(struct LList_t *node, void *data) {
	node->data = data;
}

void *llist_get_data(struct LList_t *node) {
	return node->data;
}

struct LList_t *llist_insert_node(struct LList_t **head, struct LList_t *node) {
	struct LList_t *ptr = *head;
	//debug_printf("got head: %p (%p)\n", head, *head);
	//llist_all_nodes(*head);
	do {
		if (ptr == NULL || ptr->next == NULL) {
			if (ptr == NULL) {
				*head = node;
				ptr = *head;
			} else {
				ptr->next = node;
				return node;
			}
		}
		ptr = ptr->next;
//		debug_printf("cptr: %p\n", ptr);
	} while (ptr != NULL);
	return node;
}

struct LList_t *llist_add_node(struct LList_t **head, int id, void *data) {
	struct LList_t *ptr = *head;
	struct LList_t *newnode = NULL;

        debug_printf("Adding new node id: %d\n", id);
        newnode = (struct LList_t *)malloc(sizeof(struct LList_t));
        newnode->id = id;
        newnode->data = data;
        newnode->next = NULL;

	//llist_print_node(newnode);	
	llist_insert_node(head, newnode);

/*	do {
		if (ptr == NULL || ptr->next == NULL) {
			debug_printf("Adding new node id: %d\n", id);
			newnode = (struct LList_t *)malloc(sizeof(struct LList_t));
			newnode->id = id;
			newnode->data = data;
			newnode->next = NULL;
			if (ptr == NULL) {
				*head = newnode;
				ptr = *head;
			} else {
				ptr->next = newnode;
				return newnode;
			}
		}
		ptr = ptr->next;		
	} while (ptr != NULL);
*/
	return newnode;
}

int llist_del_node(struct LList_t **head, int id) {
	struct LList_t *ptr = *head;
	struct LList_t *prev = *head;
	int found = 0;
	if (*head == NULL) return 0;

	do {
		if (ptr != NULL && prev != NULL && ptr->id == id) {
			prev->next = (struct LList_t *)ptr->next;
			if (ptr->data != NULL)
				free(ptr->data);
			free(ptr);
			found = 1;
		}
		prev = (struct LList_t *)ptr;
		ptr = ptr->next;
	} while (ptr != NULL);
	return found;
}

struct LList_t *llist_find_node(struct LList_t *head, int id) {
	struct LList_t *ptr = head;
	struct LList_t *res = NULL;
	if (head == NULL) return NULL;

	do {
		if (ptr != NULL && ptr->id == id) {
			res = ptr;
			break;
		} 
		ptr = ptr->next;
	} while (ptr != NULL);
	return res;
}

void llist_print_node(struct LList_t *node) {
	printf("*************\n");
	if (node == NULL) {
		printf("NULL node\n");
		return;
	}
	printf("ID: %d (%p)\n", node->id, node);
	printf("Type: %d\n", node->type);
	printf("Data: %p\n", node->data);
	printf("Next: %p\n", node->next);
}

void llist_all_nodes(struct LList_t *head) {
	struct LList_t *ptr = head;
	do {
		llist_print_node(ptr);
		if (ptr)
			ptr = ptr->next;
	} while (ptr != NULL);
}

