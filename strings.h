#ifndef STRINGS_H_   /* Include guard */
#define STRINGS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define STRING_ERROR 1
typedef struct _string {
	char * string;
	size_t size;   /* Size of allocated memory. */
	size_t length; /* Length of the actual string. */
} string_t;

typedef struct _string2d {
	string_t** string;
	size_t size;   /* Size of allocated memory. */
	size_t length; /* Length of the actual string. */
} string2d_t;

string_t* create_string(size_t size){
    string_t * str = malloc(sizeof(string_t));
    str->size=size;
    str->length=0;
    str->string = calloc(sizeof(char),size);
    return str;
}
char check_char(string_t* string,int i){
    if (i>string->length){
        //i=string->length;
        return '\0'; //
    }
    return string->string[i];

}


// makes string from c string
string_t* c_string(const char* c){
    size_t cl = strlen(c);
    string_t* str = (string_t*)create_string(cl+1);
    strncpy(str->string,c,cl);
    str->length=cl;
    return str;
}
// get char pointer from string
char* p_string(string_t* string){
    return string->string;
}

size_t string_length(string_t* string){
    return string->length;
}

void free_string(string_t* string){
    if (string == NULL) return;
    if (string->string!=NULL)
    free(string->string);
    string->string = NULL;
    free(string);
    string = NULL;
}

int grow_string(string_t* string,size_t size){
    string->size+=size;
    string->string = (char*)realloc(string->string,string->size);
    if (string->string == NULL){
        printf("ERROR REALLOC FAILED\n");
        return STRING_ERROR;
    }
    return 0;
}

bool compare_strings(string_t* string1,string_t* string2){
    if (string1->length!=string2->length)
        return false;

    return strcmp(string1->string, string2->string)==0;    
}

int append_string(string_t * string1, string_t * string2){
    // is there more space 
    while (string1->size-string1->length<=string2->length){
        grow_string(string1,string2->length+512);
    }

    strncat(string1->string,string2->string,string2->length);
    string1->length+=string2->length;
}

int append_charpointer(string_t * string, char * string_to_add){
    int strl = strlen(string_to_add);

    // is there more space 
    if (string->size-string->length<=strl){
        grow_string(string,strl+512);
    }

    strncat(string->string,string_to_add,strl);
    string->length+=strl;
}

string_t* join_strings(string_t * string1, string_t * string2){
    string_t* string3 = create_string(string1->length+string2->length+8);
    append_string(string3,string1);
    append_string(string3,string2);
    return string3;
}

string_t* copy_string(string_t * str, size_t start,size_t end){
    string_t* new_string = create_string((end-start)+4);
    memcpy(new_string->string,str->string+start,(end-start));
    new_string->length = (end-start);
    return new_string;
}

#endif