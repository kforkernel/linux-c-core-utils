/*
Copyright (c) 2026 @kforkernel
All rights reserved.

Licensed under the kforkernel Use License (KUL) v1.0.

See LICENSE.md for terms and conditions.
*/

#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


int main(int argc, char *argv[]){

    int error_check = 0;

    if (argc < 2){
        fprintf(stderr, "%s: Error: Usage: '%s <dir>\n", argv[0], argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++){
        if (mkdir(argv[i], 0755) == -1){
            fprintf(stderr, "%s: cannot create directory '%s': %s\n", argv[0], argv[i], strerror(errno));
            error_check = 1;
            continue;
        }

    }
    if (!error_check){
        return 0;
    }
    return 1;
    
    
}
