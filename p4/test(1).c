#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <lib.h>
#include <time.h>
int main(int argc,char**argv){
    int i;
    int loop = 0;
    for (i = 0; i < 3; i++){
        if(fork()==0){
            if(i==0){
                chrt(20);
                while(1)
                {
                    sleep(1);
                    printf("P %d heart beat %d \n", i, loop++);
                    fflush(stdout);
                    if(loop==5){
                        chrt(5);
                    }
                }
                exit(0);
            }
            else if (i == 1)
            {
                chrt(15);
                while (1)
                {
                    sleep(1);
                    printf("P %d heart beat %d \n", i, loop++);
                    fflush(stdout);
                }
                exit(0);
            }
            else{
                while (1)
                {
                    sleep(1);
                    printf("P %d heart beat %d \n", i, loop++);
                    fflush(stdout);
                    if(loop==10){
                        chrt(3);
                    }
                }
                exit(0);
            }
        }
    }
    return 0;
}