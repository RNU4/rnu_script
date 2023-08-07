#ifndef STRUCT_H
#define STRUCT_H

#include "linked_list.h"
#include "../hashtable.h"
#include <stdbool.h>
#include <stdint.h>
#define max_threads 16

#define rnu_int int32_t
#define rnu_uint uint32_t
#define rnu_float float
#define stack_size sizeof(rnu_uint)*256
typedef instance;

typedef union number{
  rnu_float f;
  rnu_uint u;
  rnu_int i;
}number_u;


typedef struct thread{
    rnu_uint* stack;
    rnu_uint* stack_frame;
    rnu_uint* stack_start;
    rnu_uint priority;
    rnu_uint* code_pointer;
    rnu_uint* code_pointer_start;
    rnu_uint ret_value;
    bool active;
    bool lock;
}thread_t;



typedef struct instance{
    /* data */
    rnu_uint* heap;
    
    rnu_uint thread_count;
    node_t* threads;
    node_t* available_threads;
    node_t* next_thread;
    node_t* void_thread;
    //external_func* external_functions; /*list of external functions*/
    rnu_uint** enternal_functions;
    //external_func *external_functions; /*list of external functions*/
}instance_t;

typedef rnu_int (*external_func)(instance_t* instance); 

#endif