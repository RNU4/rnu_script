
#define debug_mode false

#include "c_script/struct.h"
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "c_script/opcodes.h"
#include "c_script/vm.h"
#include "c_script/threads.h"
#include "bytecode_assembler/assembler.h"
#include "file_handler.h"

/**
#define get_left *(_thread->stack-1)
#define get_right *(_thread->stack)

*(_thread->stack-1) = (int64_t)get_left+(int64_t)get_right;
_thread->stack--;
_thread->code_pointer+=1; // 0 argument
*/

#define get_left *(((thread_t*)instance->next_thread->value)->stack-1)
#define get_right *(((thread_t*)instance->next_thread->value)->stack)

#define get_stack_offset(x) *(((thread_t*)instance->next_thread->value)->stack+x)
#define pop_stack(x) ((thread_t*)instance->next_thread->value)->stack-=x
#define push_stack(x) *(++((thread_t*)instance->next_thread->value)->stack) = x

assembler_t assember;

rnu_int test(instance_t* instance){
    
    get_left = get_left+get_right;
    printf("helloooo from program %d\n",get_left);
    pop_stack(1);

    return 0;
}

rnu_int rnu_print(instance_t* instance){
    printf((char*)get_right);
    pop_stack(1);
    return 0;
}

rnu_int rnu_println(instance_t* instance){
    printf("%s\n",(char*)get_right);
    pop_stack(1);
    return 0;
}

rnu_int rnu_println_num(instance_t* instance){
    printf("%d\n",get_right);
    pop_stack(1);
    return 0;
}

rnu_int rnu_malloc(instance_t* instance){
    #if debug_mode == true
    printf("Malloc : %d bytes\n",get_right);
    #endif
    get_right = (rnu_uint)malloc(get_right);
    return 0;
}

rnu_int rnu_free(instance_t* instance){
    #if debug_mode == true
    printf("free : %d bytes\n",get_right);
    #endif
    free((void*)get_right);
    pop_stack(1);
    return 0;
}

rnu_int rnu_memcpy(instance_t* instance){
    #if debug_mode == true
    printf("memcpy %d %d %d\n",get_stack_offset(-2),get_stack_offset(-1),get_stack_offset(0));
    #endif
    memcpy((char*)get_stack_offset(-2),(char*)get_stack_offset(-1),get_stack_offset(0));
    pop_stack(3);
    return 0;
}


rnu_int rnu_compile(instance_t* instance){
    string_t* str = c_string((char*)get_right); 
    get_right =  (rnu_uint)assemble_bytecode(&assember,str);
    free_string(str);
    return 0;
}

// typedef int (*external_func)(instance* instance); 
int main(int argc, char *argv[]){

    if( argc == 2 ) {
      if (file_exist(argv[1])==false){
        printf("ERROR FILE %s DOES NOT EXIST",argv[1]);
        exit(2);
      }
   }
   else if( argc > 2 ) {
      printf("Too many arguments supplied.\n");
   }
   else {
      printf("One argument [file] expected.\n");
   }


    clock_t start_time;
    double elapsed_time;


    instance_t instance;
    instance.heap = calloc(1,sizeof(rnu_uint)*256); // 2 kb - 256 64bit ints
    instance.enternal_functions = calloc(1,sizeof(rnu_uint)*256); // 2 kb - 256 64bit ints
    instance.thread_count=0;
    instance.threads=NULL;
    instance.void_thread = create_new_node(0);
    instance.available_threads=NULL;
    string_t* string = c_string(read_file(argv[1]));

    //assembler_t assember;
    assembler_init(&assember);
    add_external_function(&assember,"ex_test",(external_func)test);
    add_external_function(&assember,"print",(external_func)rnu_print);
    add_external_function(&assember,"println",(external_func)rnu_println);
    add_external_function(&assember,"println_num",(external_func)rnu_println_num);
    add_external_function(&assember,"malloc",(external_func)rnu_malloc);
    add_external_function(&assember,"free",(external_func)rnu_free);
    add_external_function(&assember,"memcpy",(external_func)rnu_memcpy);

    add_external_function(&assember,"compile",(external_func)rnu_compile);

    printf("starting compiling\n");
    start_time = clock();
    rnu_uint* compiled_code = assemble_bytecode(&assember,string);
    elapsed_time = (double)((double)clock() - start_time) / (double)CLOCKS_PER_SEC;
    printf("time to compile : %.10f\n",elapsed_time);

    add_thread(&instance,create_thread(&instance,(rnu_uint*)compiled_code,1)); // create thread with code


    printf("########### PROGRAM ############\n\n");
    start_time = clock();

    while (instance.thread_count!=0){
        instance.next_thread = instance.threads;
        /*Go through list*/
        
        while(instance.next_thread!=NULL){
            for (int i = 0; opcode_run(&instance,(thread_t*)instance.next_thread->value,instance.next_thread) 
            && (i<((thread_t*)instance.next_thread->value)->priority 
            || ((thread_t*)instance.next_thread->value)->lock==true)
            ; i++){

            }
            instance.next_thread = instance.next_thread->next;
            
        }
    }
    elapsed_time = (double)((double)clock() - start_time) / (double)CLOCKS_PER_SEC;
    printf("\n################################\n");
    printf("time to execute : %.10f\n",elapsed_time);
   // for (int i = 0; i<=10; i++)
    //    printf("stack %d: %d \n",i,*(instance.stack+i));

   // printf("stack pointer : %d\n",instance.stack_pointer);
    printf("Heap : \n");
    for (int i = 0; i<=10; i++)
        printf("[%d] = %d   ",i,*(instance.heap+i)); printf("\n");
    return 0;
}