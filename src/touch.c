/*
Copyright (c) 2026 @kforkernel
All rights reserved.

Licensed under the kforkernel Use License (KUL) v1.0.

See LICENSE.md for terms and conditions.
*/

#include <fcntl.h>
#include <utime.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]){
    int error_check = 0;

    if (argc < 2){
        fprintf(stderr, "%s: Error: Usage: '%s <file>'\n", argv[0], argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++){
        int fd = open(argv[i], O_WRONLY | O_CREAT, 0644);
        
        if (fd == -1){
            fprintf(stderr, "%s: cannot touch '%s': %s\n", argv[0], argv[i], strerror(errno));
            error_check = 1;
            continue;
        }
      
        close(fd);

        if (utime(argv[i], NULL) == -1){
            fprintf(stderr, "%s: cannot update timestamps of '%s': %s\n", argv[0], argv[i], strerror(errno));
            error_check = 1;
        }
    }

    return error_check;
}
