#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <lib.h>
#include <time.h>

void child(char name);
/* 使用系统时间函数计数1秒 */
void loop()
{
  time_t t_start, t_end;
  t_start = time(NULL);
  while (1)
  {
    t_end = time(NULL);
    double sum = difftime(t_end, t_start);
    if (sum >= 1)
    {
      return;
    }
  }
}
int main(void)
{
  float a;
  int seconds, i;
  char name;
  name = 'A';
  /*创建进程A、B、C、D*/
  for (i = 0; i < 4; i++)
  {
    if (fork() == 0)
    {
      child(name);
    }
    name++;
  }
  // printf("father end\n");
  return 0;
}
void child(char name)
{
  float f;
  int count;
  switch (name)
  {
  /*将进程A、B设为实时进程*/
  case 'A':
    chrt(1);
    break;
  case 'B':
    chrt(5);
    printf("finish chrt,name = %c\n",name);
    break;
  /*由于进程C调用的chrt(0)，所以不产生作用*/
  case 'C':
    chrt(0);
    printf("finish chrt,name = %c\n",name);
    break;
  }
  for (count = 1; count < 10; count++)
  {
    if (name=='A' && count == 2) chrt(0);
    // if (name=='A' && count == 2) chrt(4);
    loop();
    printf("%c:%d\n", name, count);
  }
  printf("%c end\n", name);
  exit(0);
}
/* 实验一： A输出一秒，B输出4秒，然后C、D输出*/
/* 实验二（去掉73行注释）： A输出一秒，B输出4秒，然后A、C、D输出*/
/* 实验三（去掉74行注释）： A输出3秒，B输出1秒，然后A、C、D输出*/
