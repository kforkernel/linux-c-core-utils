/*
Copyright (c) 2026 @kforkernel
All rights reserved.

Licensed under the kforkernel Use License (KUL) v1.0.

See LICENSE.md for terms and conditions.
*/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]){
    char buf[BUFFER_SIZE] = {0};
    ssize_t bytes_read;
    int exit_code = 0;

    char *default_args[] = {argv[0], "-"};
    char **files = argv;
    int file_count = argc;

    if (argc == 1) {
        files = default_args;
        file_count = 2;
    }


    for (int i = 1; i < file_count; i++){
        char *file_name = files[i];
        int fd;
        if (strcmp(file_name, "-") == 0) {
            fd = STDIN_FILENO;
        } 
        else {
            fd = open(file_name, O_RDONLY);
        }




        if (fd == -1){
            exit_code = 1;
            if (errno == ENOENT){
                fprintf(stderr, "%s: %s: No such file or directory\n", argv[0], file_name);
                continue;
            }
            else if (errno == EACCES){
                fprintf(stderr, "%s: %s: Permission denied\n", argv[0], file_name);
                continue;
            }
            else{
                fprintf(stderr, "Error: open failed\n");
                continue;
            }
        }



        while ((bytes_read = read(fd, buf, sizeof(buf))) > 0){
            
            ssize_t total_written = 0;
            while (total_written < bytes_read){
                ssize_t written = write(STDOUT_FILENO, buf + total_written, bytes_read - total_written);
                // btw STDOUT_FILENO is just = 1 its literally just alt name for 1 aka stdout (standard output)

                if (written == -1){
                    perror(argv[0]);
                    return 1;
                }

                total_written += written;
            }
        }  

        if (bytes_read == -1){
            fprintf(stderr, "%s: %s: %s\n", argv[0], file_name, strerror(errno)); // or perror(file_name);
            exit_code = 1;
        }

        if (fd != STDIN_FILENO) {
            close(fd);
        }

    }
    
    return exit_code;
}
