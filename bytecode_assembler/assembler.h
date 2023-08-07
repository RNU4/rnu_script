
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "../strings.h"
#include "../hashtable.h"
#include "../c_script/opcodes.h"
#include "sanitize.h"
#include "../c_script/struct.h"
#include "../file_handler.h"
typedef struct assembler
{
    /* data */
    hashtable_t bytecodes;
    hashtable_t labels;
    hashtable_t custom_labels;
    hashtable_t macros;
    int external_functions_num;
    int iota;
}assembler_t;

typedef struct macro
{
    /* data */
    string_t* content;
    int arg_count;
}macro_t;

void assembler_init(assembler_t* assembler){
    init_hash_table(&assembler->bytecodes,op_code_count);
    
    init_hash_table(&assembler->labels,10);
    init_hash_table(&assembler->custom_labels,10);
    init_hash_table(&assembler->macros,10);
    //init_hash_table(&assembler->external_functions,10);
    assembler->iota = 0;
    assembler->external_functions_num = 0;
    hash_table_insert(&assembler->custom_labels,"size_t",(void*)(sizeof(rnu_uint)));
    
    /*MACROS*/
    //hash_table_insert(&assembler->labels,"arg0",(void*)0);
    //hash_table_insert(&assembler->labels,"arg1",(void*)0);

    /*OP CODES*/
    hash_table_insert(&assembler->bytecodes,"halt",(void*)halt);
    hash_table_insert(&assembler->bytecodes,"push_num",(void*)push_num);
    hash_table_insert(&assembler->bytecodes,"pop",(void*)pop);
    hash_table_insert(&assembler->bytecodes,"push_pointer",(void*)push_pointer);
    hash_table_insert(&assembler->bytecodes,"heap_get",(void*)heap_get);
    hash_table_insert(&assembler->bytecodes,"heap_set",(void*)heap_set);
    hash_table_insert(&assembler->bytecodes,"ret",(void*)ret);
    hash_table_insert(&assembler->bytecodes,"ret_num",(void*)ret_num);
    hash_table_insert(&assembler->bytecodes,"ret_set",(void*)ret_set);

    hash_table_insert(&assembler->bytecodes,"je",(void*)je);
    hash_table_insert(&assembler->bytecodes,"jez",(void*)jez);
    hash_table_insert(&assembler->bytecodes,"eql",(void*)eql);

    hash_table_insert(&assembler->bytecodes,"sp_add",(void*)sp_add);
    hash_table_insert(&assembler->bytecodes,"sp_get",(void*)sp_get);
    hash_table_insert(&assembler->bytecodes,"sp_set",(void*)sp_set);

    hash_table_insert(&assembler->bytecodes,"sp_get_pointer",(void*)sp_get_pointer);
    hash_table_insert(&assembler->bytecodes,"set_pointer",(void*)set_pointer);
    hash_table_insert(&assembler->bytecodes,"set_pointer_value",(void*)set_pointer_value);
    hash_table_insert(&assembler->bytecodes,"get_pointer",(void*)get_pointer);

    hash_table_insert(&assembler->bytecodes,"sp_push",(void*)sp_push);
    hash_table_insert(&assembler->bytecodes,"sp_pop",(void*)sp_pop);

    hash_table_insert(&assembler->bytecodes,"jmp",(void*)jmp);
    hash_table_insert(&assembler->bytecodes,"jmp_p",(void*)jmp_p);
    
    hash_table_insert(&assembler->bytecodes,"call",(void*)call);
    hash_table_insert(&assembler->bytecodes,"call_p",(void*)call_p);
    hash_table_insert(&assembler->bytecodes,"call_ex",(void*)call_ex);

    hash_table_insert(&assembler->bytecodes,"thread",(void*)thread);
    hash_table_insert(&assembler->bytecodes,"thread_p",(void*)thread_p);
    hash_table_insert(&assembler->bytecodes,"thread_pw",(void*)thread_pw);
    hash_table_insert(&assembler->bytecodes,"thread_start",(void*)thread_start);
    hash_table_insert(&assembler->bytecodes,"thread_push",(void*)thread_push);

    hash_table_insert(&assembler->bytecodes,"lock",(void*)lock);
    hash_table_insert(&assembler->bytecodes,"unlock",(void*)unlock);
    hash_table_insert(&assembler->bytecodes,"wait",(void*)wait);

    hash_table_insert(&assembler->bytecodes,"add_signed",(void*)add_signed);
    hash_table_insert(&assembler->bytecodes,"mul_signed",(void*)mul_signed);
    
    hash_table_insert(&assembler->bytecodes,"add_unsigned",(void*)add_unsigned);
    hash_table_insert(&assembler->bytecodes,"mul_unsigned",(void*)mul_unsigned);

    hash_table_insert(&assembler->bytecodes,"add_float",(void*)add_float);
    //print_table(&assembler->bytecodes);
}

void add_external_function(assembler_t* assembler,char* name,external_func func){
   // external_func *external_functions
   //hash_table_insert(&assembler->custom_labels,name,(void*)assembler->external_functions_num);
   hash_table_insert(&assembler->custom_labels,name,(void*)func);
   //instance->external_functions[assembler->external_functions_num] = func;
   assembler->external_functions_num++;
}


enum macro_type {include,macro,label,b8,b16}; 

void macro_handler(assembler_t* assembler,string_t* string){
    bool string_char = 0; //
    char last_char = 0;
    char* new_str=(char*)calloc(sizeof(char),string->length+1);
    string_t* includes = create_string(100);
    int i2 = 0;
    int start = 0;

    /*Macro vars*/
    int arg_count = -1;
    int macro_begin = 0;
    int macro_end = 0;
    string_t* macro_name = NULL;
    string_t* label_name = NULL;
    bool label_has_value = false;
    for(int i = 0; i<string->length; i++){
        if (string->string[i] =='"')
            string_char=!string_char;
        
        if (string_char==0 &&  (check_char(string,i)=='[')){
            //while(check_char(string,i)==',' && check_char(string,i+1)==','){i++;}
            start = i;
            int j = start;
            int type = -1;
            
            while(check_char(string,j)!=']' && j< string->length){j++;}
            if (j >= string->length-1) break;
            string_t* token;
            int bytecode_start = start;
            //printf("%d %d \n",start,j);
            for(int d = start ; d <= j; d++){
                if (string->string[d]==',' || string->string[d]==']'){

                    

                    if (type !=-1) switch (type){
                    case include: //INCLUDE
                        token = copy_string(string,bytecode_start+2,d-1); // remove ""
                        //printf("token {%s}",token->string);
                        if (hash_table_check(&assembler->labels,token->string)){
                            free_string(token); break;
                        } else { hash_table_insert(&assembler->labels,token->string,(void*)0);}
                        char* file = read_file(token->string);
                        //printf("file : %s\n",file);
                        string_t* temp_str = create_string(100);
                        //append_charpointer(temp_str,"\n"); //! this will stop some random ahh crash (╯°□°)╯︵ ┻━┻
                        append_charpointer(temp_str,file);
                        //append_charpointer(temp_str,"\n"); //! this will stop some random ahh crash (╯°□°)╯︵ ┻━┻
                        remove_comments(temp_str);
                        //printf("handle comma\n");
                        comma_handler(temp_str);
                       // printf("handle comma done\n");
                        macro_handler(assembler,temp_str);
                        string_remove_illegal_chars(temp_str);
                        //printf("\nlen %d size %d\n %s",temp_str->length,temp_str->size,temp_str->string);
                        append_string(includes,temp_str);
                        
                        free_string(temp_str);
                        free_string(token);
                        free(file);
                        temp_str = NULL;
                    break;
                    case macro:
                        if (macro_name == NULL){
                            token = copy_string(string,bytecode_start+1,d);
                            macro_name = c_string(token->string);
                            free_string(token);
                        }else if (arg_count ==-1 ){
                            token = copy_string(string,bytecode_start+1,d);
                            arg_count = atoll(token->string);
                            free_string(token);
                            macro_begin = d+1;
                        }
                        macro_end = d;
                    break;
                    case label:
                        if (label_name == NULL){
                            token = copy_string(string,bytecode_start+1,d);
                            label_name = c_string(token->string);
                            free_string(token);
                        }else if (label_has_value == false ){
                            token = copy_string(string,bytecode_start+1,d);
                            if (hash_table_check(&assembler->custom_labels,label_name->string) == false){
                            if (strcmp(token->string,"iota")==0){
                                hash_table_insert(&assembler->custom_labels,label_name->string,(void*)assembler->iota++);
                            }else
                                hash_table_insert(&assembler->custom_labels,label_name->string,(void*)atoll(token->string));
                            }
                            free_string(token);
                        }
                    break;

                    case b8: //B8
                        token = copy_string(string,bytecode_start+1,d);
                        new_str[i2++] = (char)atoll(token->string);
                        free_string(token);
                    break;
                    case b16: // B16
                        token = copy_string(string,bytecode_start+1,d);
                        uint16_t b = 0xAB;
                        char low = b & 0x0FF;
                        char high = b >> 8;
                        new_str[i2++] = low;
                        new_str[i2++] = high;
                        free_string(token);
                    default:
                        break;
                    }else{
                        token = copy_string(string,bytecode_start+1,d);
                        if (check_char(token,0) == '$'){

                            if (strcmp(token->string,"$include") == 0)type = include;
                            if (strcmp(token->string,"$macro") == 0){type = macro;}
                            if (strcmp(token->string,"$label") == 0){type = label;}
                            if (strcmp(token->string,"$b8") == 0){type = b8; new_str[i2++]='"';}
                            if (strcmp(token->string,"$b16") == 0){type = b16; new_str[i2++]='"';}
                            
                        }
                        free_string(token);
                    }
                    bytecode_start = d;
                }
            }
            if (type >= b8 && type <=b16){
            new_str[i2++]='"';
            new_str[i2++]=',';}
            //macro
            if (type == macro){
                // d is end
                
                
                macro_t* m = malloc(sizeof(macro_t));
                m->content = copy_string(string,macro_begin,macro_end);
                m->arg_count = arg_count;
                
               // printf("\nMACRO TEST: {%s} %d\n%s\n",macro_name->string,m->arg_count,m->content->string);
                hash_table_insert(&assembler->macros,macro_name->string,(void*)m);
                free_string(macro_name);
                macro_name = NULL;
                arg_count = -1;
                macro_begin = 0;
                macro_end = 0;
                //exit(2);
            }

            if (type == label){
                label_has_value = false;
                free_string(label_name);
                label_name = NULL;
            }

            i = j+1;
        }else{
            new_str[i2++]=string->string[i];
        }
    }
    new_str[i2]=0;
    free(string->string); //free old string
    string->string=new_str;
    string->size=string->length+1; //set size to old length
    string->length=i2; //set length to size of char pointer array
    if (includes->length!=0)
    append_string(string,includes);
    free_string(includes);

}


void replace_code(string_t* string1,char* rep,char* with){
                //get values
    int str_start = 0;
    bool string_char = 0;
    string_t* tok;
    string_t* new_str = create_string(string1->length);
    for (int i = 0; i<=string1->length; i++){
        if (string1->string[i] =='"')
            string_char=!string_char;
        if ((string1->string[i]==',' || i == string1->length) && string_char==false){
            tok = copy_string(string1,str_start,i);
            if (strcmp(tok->string,rep) == 0){
                append_charpointer(new_str,with);
                
            }else{
                append_string(new_str,tok);
            }
            if (i != string1->length)
            append_charpointer(new_str,",");

            str_start = i+1;
            free_string(tok);
        }
    }
   //if (string->string!=NULL)
    free(string1->string); //!this will randomly make the program fuck up
    //string->string=NULL;
    string1->string=new_str->string;
    string1->size=new_str->size; //set size to old length
    string1->length=new_str->length; //set length to size of char pointer array
    free(new_str);
}


bool macro_insert(assembler_t* assembler,string_t* string){
    bool string_char = 0; //
    char last_char = 0;
    int byte_code_start = 0;
    int arg_num = 0;
    bool macro_found = false;
    string_t* new_str = create_string(string->length);
    string_t* token;
    macro_t* macro = NULL;
    char spf[10];
    //printf("{%s}\n",string->string);
    for(int i = 0; i<string->length; i++){
        if (string->string[i] =='"')
            string_char=!string_char;

        if (check_char(string,i)==',' && check_char(string,i+1)!=',' && string_char==false){
            token = copy_string(string,byte_code_start,i);
            //printf("hash name {%s}\n",hash_name->string);
            if (token->length>0)
            if (hash_table_check(&assembler->macros,token->string) == true){
                macro = (macro_t*)hash_table_get(&assembler->macros,token->string);
                free_string(token);
                //printf("start {%s} %d\n",token->string,token->length);
               // printf("content : %s\n",macro->content->string);
                string_t* macro_content = copy_string(macro->content,0,macro->content->length); // copy content for changing !error
               // printf("done\n");
                string_t* tok;
                int arg_start = 0;
                arg_num = macro->arg_count;
                arg_start = i+1;
                //get values
                int j = 0;
                for (j = i+1; j<string->length; j++){
                    if (string->string[j] =='"')
                        string_char=!string_char;
                    if (string->string[j]==',' && string_char==false){
                        tok = copy_string(string,arg_start,j);
                        sprintf(spf,"arg%d",macro->arg_count-arg_num);
                       // printf("ARG NAME : %s \n",spf);
                       // printf("before %s\n",macro_content->string);
                        replace_code(macro_content,spf,tok->string);
                       // printf("token : %s\n",tok->string);
                        //printf("after %s\n",macro_content->string);
                        arg_num--;
                        arg_start = j+1;
                        free_string(tok);
                    }
                    if (arg_num == 0) break;
                }
                //printf("macro_content {%s}\n",macro_content->string);
                append_string(new_str,macro_content);
                free_string(macro_content);
                
                i=j;
                macro_found = true;
                
            }else{
            //printf("token {%s}\n",token->string);
            append_string(new_str,token);
            free_string(token);
            }
            byte_code_start=i+1;
            append_charpointer(new_str,","); 
        }
    }
    if (string!=NULL)
    free(string->string); //!this will randomly make the program fuck up
    //string->string=NULL;
    string->string=new_str->string;
    string->size=new_str->size; //set size to old length
    string->length=new_str->length; //set length to size of char pointer array
    free(new_str);
    return macro_found;
}

bool is_digit(char c){
    if(c <= '9' && c >= '0')
  return 1;
else
  return 0;
}
bool is_num(char *instring) {
  if (*instring != '-' && *instring != '.' && !is_digit(*instring)) return false;
  if (strspn(instring+1, "0123456789.") < strlen(instring+1)) return false;
  int c = 0;
  while (*instring) if (*instring++ == '.') if (++c > 1) return false;
  return true;
}

rnu_uint string_to_bytecode(assembler_t* assembler,char* bytecode){
    number_u num;
    if (hash_table_check(&assembler->bytecodes,bytecode)){
        rnu_uint c = (rnu_uint)hash_table_get(&assembler->bytecodes,bytecode);
        return c;
    }else if (is_num(bytecode)){
        //printf("num %s\n",bytecode);
        if(strchr(bytecode, '.') == NULL){
            num.i = atoll(bytecode);
            return num.u;
        }else{
            num.f = atof(bytecode);
            return num.u;
        }
        return 0;
    }
    //!ERROR
    printf("UNKOWN! NOT NUMBER OR OPCODE : %s",bytecode);
    exit(2);
   // printf("alive3\n");
}


#define max_macro_level 10
rnu_uint* assemble_bytecode(assembler_t* assembler,string_t* code){
    //printf("%s",code->string);
    remove_comments(code);
    comma_handler(code);
    //printf("{%s}\n",code->string);
    comma_handler2(code); //! yeah nah fix it
    
    macro_handler(assembler,code);
    int i = 0;
    while(macro_insert(assembler,code) && i<max_macro_level){i++;}; //allows macros in macros
    string_remove_illegal_chars(code);
    //printf("{%s}\n",code->string);
    //printf("%s",code->string);
    //find size of program
    int program_size = 0;
    int byte_code_start = 0;
    //printf("HELLO %d\n",code->length);
    string_t* token;

    for(i = 0 ; i<code->length; i++){
        if (code->string[i]==','){
            
            program_size++;

            token = copy_string(code,byte_code_start,i);
            #if debug_mode==true
            //printf("token {%s}\n",token->string);
            #endif
            if (token->string[0] == ':'){
                program_size--;
                
                string_t* hash_name =  copy_string(code,byte_code_start+1,i);
                //printf("hash name {%s}\n",hash_name->string);
                if (hash_table_check(&assembler->labels,hash_name->string) != true)
                hash_table_insert(&assembler->labels,hash_name->string,program_size!=0 ? (void*)(program_size) : (void*)(program_size));

                free_string(hash_name);
            } else if (token->string[0] == '"'){
                program_size+=(token->length/sizeof(rnu_uint)); //string size
            }
            
            free_string(token);
            byte_code_start=i+1;
        }
        
    }
    
    if (program_size==0){
        printf("ERROR PROGRAM SIZE IS 0");
        exit(2);
    }
    program_size++; /*add halt instruction at end*/
    rnu_uint* program = (rnu_uint*)calloc(program_size,sizeof(rnu_uint));
    int program_len = 0;
    byte_code_start = 0;
    
    for(int i = 0 ; i<code->length; i++){

        if (code->string[i]==','){
            token = copy_string(code,byte_code_start,i);
            if (token->string[0]!='"'){
            
            if (token->string[0]==':'){
            string_t* hash_name =  copy_string(code,byte_code_start+1,i);
            if (hash_table_check(&assembler->labels,hash_name->string) == true)
                hash_table_get_element(&assembler->labels,hash_name->string)->value = (void*)(program_len);
            
            free_string(hash_name);
            }else if (hash_table_check(&assembler->labels,token->string)){
                program[program_len++] = (rnu_uint)(program+(rnu_uint)hash_table_get(&assembler->labels,token->string));
            } else if (hash_table_check(&assembler->custom_labels,token->string))
                program[program_len++] = (rnu_uint)hash_table_get(&assembler->custom_labels,token->string);
            else{
                
                program[program_len++] = string_to_bytecode(assembler,token->string);
                //printf("Code : %s = %d \n",token->string,string_to_bytecode(assembler,token->string));
            }
            
            
 
            }else if (token->string[0]=='"'){
                // printf("is string true\n");
                // +1 and -1 to not include string
                int j = 0;
                for (j = 0; j<token->length-2; j++)
                ((uint8_t*)(program+program_len))[j] = token->string[j+1];
                ((uint8_t*)(program+program_len))[j+1]=0; //null byte
                program_len+=(token->length/sizeof(rnu_uint))+1;
            }
            free_string(token);
            byte_code_start=i+1;
        }
    }
    #if debug_mode == true
    print_table(&assembler->labels);
    print_table(&assembler->macros);
    print_table(&assembler->custom_labels);
    #endif
   // for (int i = 0; i<program_len; i++){
     //   printf("code : %d \n",program[i]);
   // }



    //printf("size of program : %d\n",sizeof(rnu_uint)*(program_size));
    return program;
}
