#ifndef FILE_HANDLER_H_   /* Include guard */
#define FILE_HANDLER_H_
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

char *read_file(char *filename) {
    FILE *f = fopen(filename, "rt");
    assert(f);
    fseek(f, 0, SEEK_END);
    long length = ftell(f); //-8 to remove extra chars that are somehow added to end of the file
    fseek(f, 0, SEEK_SET);
    char *buffer = (char *) calloc(1,length + 1);
    buffer[length] = '\0';
    fread(buffer, 1, length, f);
    fclose(f);
    return buffer;
}


bool file_exist(char *filename){
    FILE *f;
    if((f = fopen(filename,"r"))!=NULL)
        {
            // file exists
            fclose(f);
            return true;
        }
    else
        {
            //File not found, no memory leak since 'file' == NULL
            //fclose(file) would cause an error
            return false;
        }
}

#endif