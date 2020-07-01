#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

typedef struct list_t {
	void* data;
	struct list_t* next;
} list_t;

list_t* list_create(void);
void list_destroy(list_t** head_ref);
void list_add(list_t** head_ref, void* data, size_t data_size);
void list_remove(list_t** head_ref, void* data, size_t data_size);
void list_compare_remove(list_t** head_ref, void* data, int(*compare_func)(void*,void*));
void list_clear(list_t* head);
size_t list_length(const list_t* head);

#endif // LINKED_LIST_H
