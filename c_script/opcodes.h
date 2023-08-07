#ifndef OPCODES_H
#define OPCODES_H
typedef enum opcode{
  halt
, nop /*no operation used for space that can be replaced by dymamic generated code*/
, push_num
, push_pointer
, heap_get
, heap_set
, pop
, ret
, ret_num /*return while ignoring last stack push*/
, ret_set
, jmp
, jmp_p /*jump to value from stack*/

/*Stack*/
, sp_add /*signed sp_add,(-+)num,*/
, sp_get /*signed push sp_get,(-+)num,*/
, sp_set /*signed sp_set,(-+)num,pop*/

, sp_sget /*signed push sp_get,(-+)num,*/
, sp_sset /*signed sp_set,(-+)num,pop*/
, sp_get_pointer /*signed push sp_get,(-+)num,*/
, sp_push
, sp_pop

/*Pointer*/
, set_pointer /*signed push sp_get,(-+)num,*/
, set_pointer_value
, get_pointer

/*Conditions*/
, je /*jump equal to 1*/
, jez /*jump equal to 0*/
, jelz /*jump less than 0*/
, jegz /*jump greater then 0*/

/*comparisons (pushes 1 if true and 0 if false)*/
, grt /*greater*/
, lss /*less*/
, eql /*equal*/
, grte /*greater or equal*/
, lsse /*less or equal*/

/*THREADS&FUNCTIONS*/
, call /*call function*/
, thread /*create thread and execute code*/
, thread_start /*start thread*/
, thread_push /*create thread and execute code*/
, call_p /*call function pointer from stack*/
, call_ex /*call external function pointer from stack*/
, thread_p /*create thread*/
, thread_pw /*create thread from stack and returns thread pointer*/
, lock /*lock current thread (keep executing until unlocked)*/
, unlock /*unlock thread*/
, wait /*wait for thread to finish*/

/*ADD*/
, add_signed
, add_unsigned
, add_float
/*SUB*/
, sub_signed
, sub_unsigned
, sub_float
/*MUL*/
, mul_signed
, mul_unsigned
, mul_float
/*DIV*/
, div_signed
, div_unsigned
, div_float



, op_code_count
};
#endif