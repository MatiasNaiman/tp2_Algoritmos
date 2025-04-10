#include "tp2.h"
#include <stdlib.h>
#include <stdbool.h>

struct node;
typedef struct node node_t;

struct node {
    void* value;
    node_t* next;
    node_t* prev;
};

struct list {
    node_t* head;
    node_t* tail;
    size_t size;
};

struct list_iter {
    list_t* list;
    node_t* curr;
};

list_t *list_new(){
    list_t* list = (list_t*)malloc(sizeof(list_t));
    if(!list) return NULL;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

size_t list_length(const list_t *list){
    return list->size;
}

bool list_is_empty(const list_t *list){
    return (list_length(list) == 0) ? true:false;
}

bool list_insert_head(list_t *list, void *value){
    node_t* node = (node_t*)malloc(sizeof(node_t));
    if(!node) return false;
    node->value = value;
    node->prev = NULL;
    if (list_is_empty(list)){
        list->head = node;
        list->tail = node;
        node->next = NULL;
    }
    else{
        list->head->prev = node;
        node->next = list->head;
        list->head = node;
    }
    list->size++;
    return true;
}

bool list_insert_tail(list_t *list, void *value){
    node_t* node = (node_t*)malloc(sizeof(node_t));
    if(!node) return false;
    node->value = value;
    node->next = NULL;
    if (list_is_empty(list)){
        list->head = node;
        list->tail = node;
        node->prev = NULL;
    }
    else{
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }
    list->size++;
    return true;
}

void *list_peek_head(const list_t *list){
    if(list_is_empty(list)) return NULL;
    return list->head->value;
    
}

void *list_peek_tail(const list_t *list){
    if(list_is_empty(list)) return NULL;
    return list->tail->value;
}

void *list_pop_head(list_t *list){
    if(list_is_empty(list)) return NULL;
    node_t *oldHead = list->head;
    void *val = oldHead->value;
    list->head = oldHead->next;

    if(list->head){
        list->head->prev = NULL;
    }
    else{
        // No additional action needed as list->tail is already NULL
    }
    free(oldHead);
    list->size--;
    return val;
}

void *list_pop_tail(list_t *list){
    if(list_is_empty(list)) return NULL;
    node_t *oldTail = list->tail;
    void *val = oldTail->value;
    list->tail = oldTail->prev;
    if(list->tail){
        list->tail->next = NULL;
    }
    else{
        list->tail = NULL;
        
    }
    free(oldTail);
    list->size--;
    return val;
}

void list_destroy(list_t *list, void destroy_value(void *)){
    if (list_is_empty(list)) return;
    node_t* actual = list->head;
    while(actual){
        node_t* sig = actual->next;
        if(destroy_value){
            destroy_value(actual->value);
        }
        free(actual);
        actual = sig;
    }
    free(list);
}

list_iter_t *list_iter_create_head(list_t *list_){
    if(list_ == NULL) return NULL;
    list_iter_t* iterable = (list_iter_t*)malloc(sizeof(list_iter_t));
    if(!iterable) return NULL;
    iterable->list = list_;
    iterable->curr = list_->head;
    return iterable;
}

list_iter_t *list_iter_create_tail(list_t *list_){
    if(list_ == NULL) return NULL;
    list_iter_t* iterable = (list_iter_t*)malloc(sizeof(list_iter_t));
    if(!iterable) return NULL;
    iterable->list = list_;
    iterable->curr = list_->tail;
    return iterable;
}

bool list_iter_forward(list_iter_t *iter){
    if(iter == NULL || iter->curr == NULL) return false;
    if(!iter->curr->next) return false;
    iter->curr = iter->curr->next;
    return true;
}

bool list_iter_backward(list_iter_t *iter){
    if(iter == NULL || iter->curr == NULL) return false;
    if(!iter->curr->prev) return false;
    iter->curr = iter->curr->prev;
    return true;
}

void *list_iter_peek_current(const list_iter_t *iter){
    if(list_is_empty(iter->list)) return NULL;
    return iter->curr->value;
}

bool list_iter_at_last(const list_iter_t *iter){
    if(list_is_empty(iter->list) || iter->curr == iter->list->tail) return true;
    return false;
}

bool list_iter_at_first(const list_iter_t *iter){
    if(list_is_empty(iter->list) || iter->curr == iter->list->head) return true;
    return false;
}

void list_iter_destroy(list_iter_t *iter){
    free(iter);
    return;
}

bool list_iter_insert_after(list_iter_t *iter, void *value){
    if(!iter) return false;
    
    node_t* new = (node_t*)malloc(sizeof(node_t));
    if(!new) return false;
    new->value = value;

    if(iter->curr == NULL){
        if(list_is_empty(iter->list)){
            iter->list->head = new;
            new -> prev = NULL;
        } else{
            new->prev = iter->list->tail;
            iter->list->tail->next = new;
        }
        new->next = NULL;
        iter->list->tail = new;
    }
    else{
        new->prev = iter->curr->prev;
        new->next = iter->curr->next;
        iter->curr->next = new;
    }
    iter->list->size++;
    return true;
}



bool list_iter_insert_before(list_iter_t *iter, void *value){
    if(!iter) return false;
    
    node_t* new = (node_t*)malloc(sizeof(node_t));
    if(!new) return false;
    new->value = value;

    if(iter->curr == NULL){ //o la lista esta vacia o curr esta fuera de indice al principio
        if(list_is_empty(iter->list)){
            iter->list->head = new;
            iter->list->tail = new;
            new -> next = NULL;
            new -> prev = NULL;
        }
        new->next = iter->list->head;
        iter->list->head->prev = new;
        new -> prev = NULL;
        iter->list->head = new;
    }
    else{
        iter->curr->prev = new;
        new->prev = NULL;
    }
    iter->list->size++;
    return true;
}

void *list_iter_delete(list_iter_t *iter){
    if(list_is_empty(iter->list)) return NULL;
    void* val = iter->curr->value;
    if(!iter->curr->next){
        list_iter_backward(iter);
        free(iter->curr->next);
    }
    if(!iter->curr->prev){
        list_iter_forward(iter);
        free(iter->curr->prev);
    }
    return val;
}