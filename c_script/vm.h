#include "opcodes.h"
#include "struct.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "threads.h"

#define get_left *(_thread->stack-1)
#define get_right *(_thread->stack)
#define arg_count(x) (_thread->code_pointer+=1+x)
number_u number; //needed for converting float to a binary form

bool opcode_run(instance_t* instance,thread_t* _thread,node_t* node){
#if debug_mode == true
    //printf("code value : %d\n",*_thread->code_pointer);
#endif

switch (*_thread->code_pointer){
    
    case halt:

       // for (int i = 0; i<=10; i++)
       // printf("{%d : %d} ",i,*(_thread->stack+i));

       // printf("stack pointer : %d\n",_thread->stack_pointer);
       
        _thread->active = false; // not active
        destroy_thread(instance,node);
        #if debug_mode == true
        printf("halt\n");
        #endif
        return false;
    break;

    case nop: /*does nothing*/
        _thread->code_pointer+=1;
    break;
    //[][2][2][][]
    case push_num:
        //printf("push %d \n",*(_thread->code_pointer+1));
        
        *(++_thread->stack) = *(_thread->code_pointer+1);
        _thread->code_pointer+=2; // 1 argument
    break;
    
    case pop:
        _thread->stack-= *(_thread->code_pointer+1);
        arg_count(1);
    break;

    case heap_get:

       // printf("push pointer %d \n",*(_thread->stack+(_thread->stack_pointer)));
        //"pop" and push current value that it points too
        #if debug_mode == true
        printf("get_pointer %d = %d \n",get_right,instance->heap[get_right]);
        #endif
        get_right = instance->heap[get_right];
        _thread->code_pointer+=1; // 0 argument
    break;
    
    case heap_set:
        instance->heap[get_right] = get_left;
        #if debug_mode == true
        printf("set_pointer %d = %d \n",get_right,get_left);
        #endif
        _thread->stack-=2; // "pop" the values
        arg_count(0);
    break;
    case jmp:
        _thread->code_pointer = (rnu_uint*)*(_thread->code_pointer+1);
    break;

    case jmp_p:
        #if debug_mode == true
        printf("jmp_p %d \n",get_right); 
        #endif
        _thread->code_pointer = (rnu_uint*)get_right;
        _thread->stack--;
    break;
    
    case sp_push: //!find better way..

    _thread->stack+=(rnu_uint)*(_thread->code_pointer+1);

    *(++_thread->stack) = (rnu_uint)_thread->stack_frame;
    *(++_thread->stack) = (rnu_uint)*(_thread->code_pointer+1);
    
    _thread->stack_frame = _thread->stack-2;
    arg_count(1);
    break;
    
    case sp_pop: //!find better way..
    _thread->stack_frame = (rnu_uint*)get_left;
    _thread->stack -= (rnu_uint)get_right+2;
    arg_count(0);
    break;

    case sp_set:
        *(_thread->stack_frame+((rnu_int)*(_thread->code_pointer+1))) = get_right;
        _thread->stack--;
        arg_count(1);
    break;

    case sp_get:
        #if debug_mode == true
        printf("sp_get : %d = %d\n",(rnu_int)*(_thread->code_pointer+1),*(_thread->stack_frame+((rnu_int)*(_thread->code_pointer+1))));
        #endif
        *(++_thread->stack) = *(_thread->stack_frame+((rnu_int)*(_thread->code_pointer+1)));
        arg_count(1);
    break;
    /*
    case sp_sset:
        *(_thread->stack_frame+((rnu_int)*(_thread->code_pointer+1))) = get_right;
        _thread->stack--;
        arg_count(1);
    break;*/

    case sp_get_pointer:
        *(++_thread->stack) = (rnu_uint)(_thread->stack_frame+((rnu_uint)*(_thread->code_pointer+1)));
        arg_count(1);
    break;

    case set_pointer_value:
        (*(rnu_uint*)(get_right)) = get_left;
        _thread->stack-=2;
        arg_count(0);
    break;

    case set_pointer:
        *(rnu_uint*)(get_right) = get_left;
        _thread->stack-=2;
        arg_count(0);
    break;

    case get_pointer: 
        get_right = (rnu_uint)(*(rnu_uint*)get_right);
        arg_count(0);
    break;

    #define vm_con_macro(x) get_left = (get_left x get_right); _thread->stack--; arg_count(0);
    case eql:vm_con_macro(==);break;
    case grt:vm_con_macro(<);break;
    case lss:vm_con_macro(>);break;
    case grte:vm_con_macro(<=);break;
    case lsse:vm_con_macro(>=);break;
    #undef vm_con_macro

    case je:
        _thread->code_pointer = get_right==1 ? (rnu_uint*)*(_thread->code_pointer+1) : _thread->code_pointer+1;
        _thread->stack--;
    break;
    case jez:
        get_right==0 ? _thread->code_pointer = (rnu_uint*)*(_thread->code_pointer+1) : arg_count(1);
        _thread->stack--;
    break;
   // case sp_add:
 //   break;
    //case call:
     //   *(_thread->stack+(++_thread->stack_pointer)) = (rnu_uint)(_thread->code_pointer+2); /*push current location +1
      //  _thread->code_pointer = _thread->code_pointer_start+*(_thread->code_pointer+1); 
    //break;
    

   case call:
        
        *(++_thread->stack) = (rnu_uint)(_thread->code_pointer+2); /*push current location +1*/
        _thread->code_pointer = (rnu_uint*)*(_thread->code_pointer+1); /*jump to function*/
        //printf("jump too %d\n",_thread->code_pointer-_thread->code_pointer_start);
    break;

    case call_p:{
        rnu_uint tem = get_right;
        get_right = (rnu_uint)(_thread->code_pointer+1); /*push current location +1*/
        _thread->code_pointer = (rnu_uint*)tem; /*jump to function*/
    break;}

    case call_ex:
        _thread->code_pointer+=((external_func)*(_thread->code_pointer+1))(instance)+2;
    break;
    case ret:
        #if debug_mode == true
        printf("ret %d \n",get_right);
        #endif
        _thread->code_pointer = (rnu_uint*)get_right;
        _thread->stack--;

    break;
    case ret_set:
        _thread->ret_value = get_right;
        _thread->stack--;
    break;
    case ret_num:
        #if debug_mode == true
        printf("ret %d \n",get_right);
        #endif
        _thread->code_pointer = (rnu_uint*)get_right;
        get_right = _thread->ret_value;

    break;

    case thread:
        add_thread(instance,create_thread(instance,(rnu_uint*)(*(_thread->code_pointer+1)),*(_thread->code_pointer+2)));
        arg_count(2);
    break;

    case thread_pw:
        //push thread pointer
        get_left = (rnu_uint)create_thread(instance,(rnu_uint*)get_left,get_right);
        _thread->stack--;
        arg_count(0);
    break;
    case thread_push:
    *(((thread_t*)(((node_t*)get_right)->value))->stack++) = get_left;
    


        //printf("thread stack\n");
        ///for (int i = 0; i<=10; i++)
        //printf("[%d] = %d   ",i ,*(((thread_t*)(((node_t*)get_right)->value))->stack-1+i)); printf("\n");
    _thread->stack-=2;
    arg_count(0);
    break;
    case thread_start:
        add_thread(instance,(node_t*)*(_thread->stack--)); // add thread and pop value
        arg_count(0);
    break;

    case wait:
    if (((thread_t*)(((node_t*)get_right)->value))->active == false){
        _thread->code_pointer+=1;
        _thread->stack--; //pop
    }else{return false;}
    break;


    case lock:
        _thread->lock = true;
        _thread->code_pointer+=1;
    break;

    case unlock:
        _thread->lock = false;
        _thread->code_pointer+=1;
    break;

    /*FLOAT*/
    case add_float:
        //cheeky illegal union stuff
        number.f = ((number_u)get_left).f+((number_u)get_right).f;
        *(--_thread->stack) = number.u;
        _thread->code_pointer+=1; // 0 argument
    break;

    case sub_float:
        //cheeky illegal union stuff
        number.f = ((number_u)get_left).f-((number_u)get_right).f;
        *(--_thread->stack) = number.u;
        _thread->code_pointer+=1; // 0 argument
    break;

    case mul_float:
        //cheeky illegal union stuff
        number.f = ((number_u)get_left).f*((number_u)get_right).f;
        *(--_thread->stack) = number.u;
        _thread->code_pointer+=1; // 0 argument
    break;

    case div_float:
        //cheeky illegal union stuff
        number.f = ((number_u)get_left).f/((number_u)get_right).f;
        *(--_thread->stack) = number.u;
        _thread->code_pointer+=1; // 0 argument
    break;

    /*SIGNED INT*/
    case add_signed:
        #if debug_mode == true
        printf("add signed %d + %d = %d\n",(rnu_int)get_left,(rnu_int)get_right,
        (rnu_int)get_left+(rnu_int)get_right);
        #endif

        *(_thread->stack-1) = (rnu_int)get_left+(rnu_int)get_right;
        _thread->stack--;
        _thread->code_pointer+=1; // 0 argument
        
    break;

    case mul_signed:
        #if debug_mode == true
        printf("mul signed %d * %d = %d\n",(rnu_int)get_left,(rnu_int)get_right,
        (rnu_int)get_left*(rnu_int)get_right);
        #endif
        *(_thread->stack-1) =  (rnu_int)get_left*(rnu_int)get_right;
        _thread->stack--;
        _thread->code_pointer+=1; // 0 argument
    break;

    case mul_unsigned:
        *(_thread->stack--) = get_left*get_right;
        _thread->code_pointer+=1; // 0 argument
    break;

    case add_unsigned:
        *(_thread->stack--) = get_left*get_right;
        _thread->code_pointer+=1; // 0 argument
    break;

    default:
    printf("error %d\n",*(_thread->code_pointer));
    exit(2);
        return false;
    break;
}
      // for (int i = 0; i<=3; i++)
      // printf("{%d : %d} ",i,*(_thread->stack+i));
      // printf("\n");
return true;
}