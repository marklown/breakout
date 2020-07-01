#include "linked_list.h"
#include <stdlib.h>
#include <string.h>

list_t* list_create(void)
{
	list_t* head = (list_t*)malloc(sizeof(list_t));
	memset(head,0,sizeof(list_t));
	return head;
}

void list_destroy(list_t** head_ref)
{
	list_clear(*head_ref);
	free(*head_ref);
	(*head_ref) = NULL;
}

void list_clear(list_t* head)
{
	list_t* next = head->next;
	while (next != NULL) {
		list_t* tmp = next;
		next = next->next;
		free(tmp->data);
		free(tmp);
	}
	free(head->data);
	head->data = NULL;
	head->next = NULL;
}

void list_add(list_t** head_ref, void* data, size_t data_size)
{
	list_t* new_node = NULL;
	if ((*head_ref)->data==NULL) new_node=(*head_ref);	/* empty list, use head as new_node */
	else new_node = (list_t*)malloc(sizeof(list_t));	/* else allocate memory for new_node */
	new_node->data = malloc(data_size);					/* allocate memory for the data */
	memcpy(new_node->data,data,data_size);				/* copy the data into the list */
	if (new_node == *head_ref) {
		new_node->next = NULL;							/* new_node is head so next is NULL */
	} else {
		new_node->next = *head_ref;						/* insert at front of list */
		*head_ref = new_node;							/* change head pointer */
	}
}

void list_remove(list_t** head_ref, void* data, size_t data_size)
{
	list_t* head = *head_ref;
	if (memcmp(head->data,data,data_size) == 0) {		/* head holds the data */
		if (head->next == NULL) {						/* head is the only node */
			free(head->data);
			head->data = NULL;
		} else {										/* head is not the only node */
			*head_ref = head->next;
			free(head->data);
			free(head);
		}
	} else {											/* head does not hold the data */
		list_t* prev = head;
		list_t* next = head->next;
		while (next != NULL) {
			if (memcmp(next->data,data,data_size) == 0) {
				prev->next = next->next;
				free(next->data);
				free(next);	
				return;
			}
			prev = next;
			next = next->next;
		}
	}
}

void list_compare_remove(list_t** head_ref, void* data, int(*compare_func)(void*,void*))
{
	list_t* head = *head_ref;
	if ((*compare_func)(head->data,data) == 0) {/* Call the provided comparison func */
		*head_ref = head->next;
		free(head->data);
		free(head);
	} else {
		list_t* prev = head;
		list_t* next = head->next;
		while (next != NULL) {
			if ((*compare_func)(next->data,data) == 0) {/* Call the provided comparison func */
				prev->next = next->next;
				free(next->data);
				free(next);	
				return;
			}
			prev = next;
			next = next->next;
		}
	}
}

size_t list_length(const list_t* head)
{
	if (head->data == NULL) return 0;
	size_t count = 1;
	list_t* next = head->next;
	while (next != NULL) {
		next = next->next;
		count++;
	}
	return count;
}
