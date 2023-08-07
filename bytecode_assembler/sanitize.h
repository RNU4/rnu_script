#include "../strings.h"
#include <stdio.h>
#include <stdlib.h>

bool legal_char(char c){
    if (c>32 && c<=126)
        return true;
        else
        return false;
}

bool container_char(char c){
    if (c=='"')// || c=='{' || c=='}')
        return true;
        else
        return false;
}

bool replace_illegal_char(char *c){
    if (c[0]=='\n' || c[0]=='\r' || c[0]=='\t'){
        c[0]=' ';
        return true;

    }else
        return false;
}
uint64_t* string_to_base64(char* string,int len){
    uint64_t* str64 = malloc((len/8)+1);
    for (int i = 0; i<len; i++)
    ((char*)str64)[i]=string[i];
    return str64;
}

void string_remove_illegal_chars(string_t* string){ //!REWRITED MIGHT CONTAIN ERROS
    //char* new_str=(char*)calloc(sizeof(char)*string->length*2,1);
    int i2=0;
    bool is_string = false;
    for(int i = 0; i<string->length; i++){
        //string_to_base64
        if (string->string[i]=='"')
            is_string=!is_string;


        if (legal_char(string->string[i]) || is_string==true){
            string->string[i2++]=string->string[i];
        }
        
        /*else{
            char c = string->string[i];
            if (replace_illegal_char(&c)){
                new_str[i2]=c;
                i2++;
            }
        }*/
    }
    string->string[i2]=0;
    //printf("program: \n %s",new_str);
    //exit(2);
    //free(string->string); //free old string
    //string->string=new_str;
    string->size=string->length; //set size to old length
    string->length=i2; //set length to size of char pointer array
}

void array_handle(string_t* string){ //!REWRITED MIGHT CONTAIN ERROS
    bool string_char = 0; //
    char last_char = 0;
   // char* new_str=malloc(sizeof(char)*string->length);
    int i2 = 0;
    //printf(string->string);
    int start = 0;
    for(int i = 0; i<string->length; i++){
        if (string->string[i] =='"')
            string_char=!string_char;
        
        if (string_char==0 &&  (check_char(string,i)=='[')){
            //while(check_char(string,i)==',' && check_char(string,i+1)==','){i++;}
            start = i;
            int j = start;
            string->string[i2++]='"';
            while(check_char(string,j)!=']'){j++;}
            string_t* token;
            int bytecode_start = start;
            //printf("%d %d \n",start,j);
            for(int d = start ; d<=j; d++){
                if (string->string[d]==',' || string->string[d]==']'){

                    token = copy_string(string,bytecode_start+1,d);
                    if (check_char(token,0) == '$'){
                        //!fix this shit so i can do more then u8
                    }else{
                        //printf("Token : %s\n",token->string);
                        string->string[i2++] = (char)atoll(token->string);
                    }
                    bytecode_start = d;
                    free_string(token);
                }
            }
            string->string[i2++]='"';
            string->string[i2++]=',';
            i = j+1;
        }else{
            string->string[i2++]=string->string[i];
        }
    }
    
    string->string[i2]=0;
   // free(string->string); //free old string
    //string->string=new_str;
    //string->size=string->length; //set size to old length
    string->length=i2; //set length to size of char pointer array
}

void comma_handler(string_t* string){
    bool string_char = 0; //
    bool last_char_legal = 0;
    //char* new_str=malloc(sizeof(char)*string->length);
    int i2 = 0;
    //printf("xaxa0\n");
    //printf("string %s\n",string->string);
    /*
    for(int i = 0; i<string->length; i++){
        if (i==0) continue;
        if (string->string[i] =='"')
            string_char=!string_char;
        
        if (string_char==0 &&!legal_char(check_char(string,i))){
            string->string[i] = ',';
        }

    }*/
    //printf("xaxa1\n");
    for(int i = 0; i<string->length; i++){
        if (string->string[i] =='"')
            string_char=!string_char;
        
        if (i == 0 && string->string[i] == ',') continue;
        


        if (string_char==0){
            //while( || check_char(string,i)==',' && (check_char(string,i+1)==',' || check_char(string,i+1)==']')){i++;}
            if (!legal_char(check_char(string,i)) && (legal_char(check_char(string,i+1)) || check_char(string,i+1) == 0)){
                if ((check_char(string,i+1)!=',' && check_char(string,i+1)!=']'))
                if (last_char_legal == false){
                string->string[i2++]=',';
                last_char_legal = true;
                }
            }else if (legal_char(check_char(string,i))){
                string->string[i2++]=string->string[i];
                last_char_legal = false;
            }
        }else{string->string[i2++]=string->string[i];last_char_legal = false;}

   }
   if (i2>0 && string->string[i2-1] != ',')
    string->string[i2++]=',';
   string->string[i2]=0;
   //printf("file %s\n",string->string);

    string->length=i2; //set length to size of char pointer array
}


void comma_handler2(string_t* string){
    bool string_char = 0; //
    bool last_char_legal = 0;
    int i2 = 0;
;
    for(int i = 0; i<string->length; i++){
        if (i == 0 && string->string[i] == ',') continue;
        if (string->string[i] =='"')
            string_char=!string_char;
        if (string_char==false && (check_char(string,i)!=',' || check_char(string,i+1)!=',')){
        string->string[i2++]=string->string[i];
        continue;
        }
        if (string_char==true){
            string->string[i2++]=string->string[i];
        }
   }
   string->string[i2]=0;
   //printf("file %s\n",string->string);

    string->length=i2; //set length to size of char pointer array
}
void remove_comments(string_t* string){ //!REWRITED MIGHT CONTAIN ERROS

   // append_charpointer(string,","); // yeah dont ask free() will crash or not ¯\_(ツ)_/¯ (at this point this function is cursed) somtimes it will crash with and wont without

    bool comment = 0; //
    bool string_char = 0; //
    //printf("\n{%s}\n",string->string);
    //char* new_str=calloc(sizeof(char),string->length);
    int i2 = 0;
    for(int i = 0; i<string->length; i++){
        
        if (string->string[i] =='"')
            string_char=!string_char;
        
        if (string_char==0 && check_char(string,i)=='/' && check_char(string,i+1)=='*'){
            comment=1;
        }else if (string_char==0 && check_char(string,i)=='*' && check_char(string,i+1)=='/'){
            comment=0;
            i+=2; //  */ skip chars
        }
        if (comment==0){
            string->string[i2++]=string->string[i];
        }

    }
    string->string[i2]=0;
   //if (string->string!=NULL)
    //free(string->string); //!this will randomly make the program fuck up
    //string->string=NULL;
    //string->string=new_str;
    string->size=string->length; //set size to old length
    string->length=i2; //set length to size of char pointer array
    //printf("\n{%s}\n",string->string);
}