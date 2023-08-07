#ifndef THREADS_H
#define THREADS_H
#include "struct.h"
#include "linked_list.h"
/*
thread_t* create_thread(instance_t* instance,uint64_t* code_pointer,uint64_t priority){
    thread_t* thread = malloc(sizeof(thread_t));
    thread->stack_pointer = 0;
    thread->code_pointer = code_pointer;
    thread->code_pointer_start = code_pointer;
    
    thread->stack = calloc(1,stack_size); // 2 kb - 256 64bit ints

    thread->stack_pointer = 0;
    thread->priority = priority;
    thread->top_stack_pointer= 0;
    thread->lock = false;
    insert_at_head(&instance->threads,create_new_node(thread,instance->threads));
    instance->thread_count++;

    return thread;
}

void destroy_thread(instance_t* instance,node_t* node){
    free(((thread_t*)node->value)->stack); //free stack
    free(((thread_t*)node->value)); //free thread
    remove_node(&instance->threads,node); //remove dead thread from list
    instance->thread_count--;
}

*/

node_t* create_thread(instance_t* instance,rnu_uint* code_pointer,rnu_uint priority){

    thread_t* _thread;
    node_t* node;
    if (instance->available_threads == NULL){ //no threads avalible
    _thread = malloc(sizeof(thread_t));
    
    _thread->code_pointer = code_pointer;
    _thread->code_pointer_start = code_pointer;
    
    _thread->stack = calloc(1,stack_size); // 2 kb - 256 64bit ints
    _thread->stack_frame = _thread->stack;
    _thread->priority = priority; 
    _thread->stack_start = _thread->stack;
    _thread->lock = false;
    _thread->active = true;
    node = create_new_node(_thread);
    //insert_at_head(&instance->threads,create_new_node(_thread));
    }else{ //get used thread
        node_t* popped = instance->available_threads;
        remove_node(&instance->available_threads,instance->available_threads); // pop
        _thread = popped->value;
        
        _thread->priority = priority;
        _thread->stack = _thread->stack_start;
        _thread->stack_frame = _thread->stack_start;
        _thread->lock = false;
        _thread->active = true;
        
        _thread->code_pointer = code_pointer;
        _thread->code_pointer_start = code_pointer;
        node = popped;
        //insert_at_head(&instance->threads,popped);
    }
    
    
    return node;
}

void add_thread(instance_t* instance,node_t* node){
    insert_at_head(&instance->threads,node);
    instance->thread_count++;
}

void destroy_thread(instance_t* instance,node_t* node){
    //free(((thread_t*)node->value)->stack); //free stack
    //free(((thread_t*)node->value)); //free thread

    instance->next_thread = node->next;
    
    remove_node(&instance->threads,node); //remove dead thread from list
    
    ((thread_t*)node->value)->active = false; // mark thread as not in use
    insert_at_head(&instance->available_threads,node); // put it into available threads
    instance->thread_count--;
    
    if (instance->next_thread == NULL){
    instance->next_thread = instance->void_thread;
    //printf("true;");
    }

    //printf("thread count: %d\n",instance->thread_count);
}

#endif