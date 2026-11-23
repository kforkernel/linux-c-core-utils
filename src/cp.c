/*
Copyright (c) 2026 @kforkernel
All rights reserved.

Licensed under the kforkernel Use License (KUL) v1.0.

See LICENSE.md for terms and conditions.
*/


#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUFFER_SIZE 131072


int main(int argc, char *argv[]){

    struct stat src_stat, dst_stat;

    if (argc < 3){
        fprintf(stderr,"%s: Missing file argument\n", argv[0]);
        fprintf(stderr,"Usage: '%s <src> <dst>\n", argv[0]);
        //fprintf(stderr,"'%s --help' for more info\n", argv[0]);
        return 1;
    }





    unsigned char buf[BUFFER_SIZE] = {0};
    const char *const_dir_last_arg = argv[argc - 1];
    struct stat path_stat; // for checking if path exist
    size_t bytes_read = 0;
    int error_check = 1; // aka true (non 0 num are true e.g -1, 5, 1, 22 etc)

    // removing trailing '/' to prevent error if user enters dst/
    char final_path[4096] = {0};
    char dst_copy_no_slash[4096] = {0};

    snprintf(dst_copy_no_slash, sizeof(dst_copy_no_slash), "%s", const_dir_last_arg); // copy dst so we can modify it
    size_t len = strlen(dst_copy_no_slash);

    if (len > 1 && dst_copy_no_slash[len - 1] == '/'){
        dst_copy_no_slash[len - 1] = '\0'; // replace '/' with null terminator to shorten string
    }





    if (argc == 3){
        FILE *src = fopen(argv[1], "rb");
        if (src == NULL){
            perror(argv[1]);
            return 1;
        }

        if (stat(const_dir_last_arg, &path_stat) == 0 && S_ISDIR(path_stat.st_mode)){
            
            const char *slash = strrchr(argv[1], '/');
            const char *filename = (slash != NULL) ? slash + 1 : argv[1];

            // snprint syntax final_path = where to print to, sizeof(final_path) = max amount to print,
            // "%s/%s" = prints formatted text e.g if 1st %s represents hello and 2nd %s represents world
            // it will become hello/world (the / between %s/%s adds the /), dst = first %s, argv[1] = 2nd %s
            snprintf(final_path, sizeof(final_path), "%s/%s", dst_copy_no_slash, filename);            
            
            if (stat(argv[1], &src_stat) == 0 && stat(final_path, &dst_stat) == 0){
                if (src_stat.st_dev == dst_stat.st_dev && src_stat.st_ino == dst_stat.st_ino){
                    fprintf(stderr, "%s: '%s' and '%s' are the same file\n", argv[0], argv[1], final_path);
                    fclose(src);
                    return 1;
                }
            }

            FILE *dst_file = fopen(final_path, "wb");
            if (dst_file == NULL){
                perror(final_path);
                fclose(src);
                return 1;
            }

            // fread syntax buf = where we place the read bytes into, 1 = how much at a time, sizeof(buf)
            // is the max amount to read, src = from where we are reading
            while ((bytes_read = fread(buf, 1, sizeof(buf), src)) > 0){
                // fwrite syntax buf = where we write FROM, 1 = size of how much me writing, 
                // bytes_read = the actual amount, dst_file = the destination
                if (fwrite(buf, 1, bytes_read, dst_file) != bytes_read){
                    perror("Write failed");
                    error_check = 0;
                    break;
                }
            }

            fclose(dst_file);
        }

        else{
            if (stat(argv[1], &src_stat) == 0 && stat(argv[2], &dst_stat) == 0){
                if (src_stat.st_dev == dst_stat.st_dev && src_stat.st_ino == dst_stat.st_ino){
                    fprintf(stderr, "%s: '%s' and '%s' are the same file\n", argv[0], argv[1], argv[2]);
                    fclose(src);
                    return 1;
                }
            }
            
            // copy file contents of argv[1] to argv[2] cos both files
            FILE *copy_of_file_one = fopen(argv[2], "wb");
            if (copy_of_file_one == NULL){
                perror(argv[2]);
                fclose(src);
                return 1;
            }

            while ((bytes_read = fread(buf, 1, sizeof(buf), src)) > 0){
                if (fwrite(buf, 1, bytes_read, copy_of_file_one) != bytes_read){
                    perror("Write failed");
                    error_check = 0;
                    break;
                }
            }

            fclose(copy_of_file_one);
        }

        fclose(src);

        if (!error_check){return 1;}
        return 0;
    }





    else if (stat(dst_copy_no_slash, &path_stat) == 0 && S_ISDIR(path_stat.st_mode)){

        for (int i = 1; i < (argc - 1); i++){
            char final_path[4096] = {0};
            const char *slash = strrchr(argv[i], '/');
            const char *filename = (slash != NULL) ? slash + 1 : argv[i];

            snprintf(final_path, sizeof(final_path), "%s/%s", dst_copy_no_slash, filename);

            FILE *src = fopen(argv[i], "rb");
            if (src == NULL){
                perror(argv[i]);
                error_check = 0;
                continue;
            }

            if (stat(argv[i], &src_stat) == 0 && stat(final_path, &dst_stat) == 0){
                if (src_stat.st_dev == dst_stat.st_dev && src_stat.st_ino == dst_stat.st_ino){
                    fprintf(stderr, "%s: '%s' and '%s' are the same file\n", argv[0], argv[i], final_path);
                    error_check = 0;
                    fclose(src);
                    continue;
                }
            }


            FILE *dst_dir_with_cp_file = fopen(final_path, "wb");
            if (dst_dir_with_cp_file == NULL){
                perror(final_path);
                fclose(src);
                return 1;
            }

            while ((bytes_read = fread(buf, 1, sizeof(buf), src)) > 0){
                if (fwrite(buf, 1, bytes_read, dst_dir_with_cp_file) != bytes_read){
                    perror("Write failed");
                    error_check = 0;
                    break;
                }
            }

            fclose(dst_dir_with_cp_file);
            fclose(src);
        }

        if (!error_check){
            return 1;
        }
        return 0;
    }





    else{
        fprintf(stderr, "%s: target '%s' is not a directory\n", argv[0], const_dir_last_arg);
        return 1;
    }
    
    

}

