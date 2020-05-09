#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <lib.h>
#include <time.h>

void test(char name);

void loop(){
    sleep(1);
//   time_t t_start, t_end;
//   t_start = time(NULL);
//   while (1){
//     t_end = time(NULL);
//     double sum = difftime(t_end, t_start);
//     if (sum >= 1){
//       return;
//     }
//   }
}

int main(void){
  char name;
  name = 'A';
  for (int i = 0; i < 3; i++){
    if (fork() == 0){
      test(name);
    }
    name++;
  }
  return 0;
}

void test(char name){
  int count;
  switch (name){
  case 'A':
    chrt(20);
    printf("chrt name = %c\n",name);
    sleep(1);
    break;

  case 'B':
    chrt(15);
    printf("chrt name = %c\n",name);
    sleep(1);
    break;

  case 'C':
    printf("chrt name = %c\n",name);
    break;
  }

  for (count = 1; count < 10; count++){
    if (name=='A' && count == 5) {
        chrt(5);
        printf("A execute chrt(5) at 5s");
    }
    loop();

    printf("%c:%d\n", name, count);

  }

  printf("%c end\n", name);
  exit(0);

}