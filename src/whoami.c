/*
Copyright (c) 2026 @kforkernel
All rights reserved.

Licensed under the kforkernel Use License (KUL) v1.0.

See LICENSE.md for terms and conditions.
*/

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>  
#include <stdio.h>



int main(void){
    
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);

    if (pw != NULL){
        printf("%s\n", pw->pw_name);
    }
    else{
      perror("whoami");  
      return 1;
    }
    
    return 0;
}
