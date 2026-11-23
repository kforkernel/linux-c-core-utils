/*
Copyright (c) 2026 @kforkernel
All rights reserved.

Licensed under the kforkernel Use License (KUL) v1.0.

See LICENSE.md for terms and conditions.
*/

#include <unistd.h>
#include <stdio.h>

int main(){

    char cwd[4096] = {0};

    if (getcwd(cwd, sizeof(cwd)) != NULL){
        printf("%s\n", cwd);
        return 0;
    }
    else{
        perror("pwd");
        return 1;
    }

}
