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
/* ʹ��ϵͳʱ�亯������1�� */
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
  /*��������A��B��C��D*/
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
  /*������A��B��Ϊʵʱ����*/
  case 'A':
    chrt(1);
    break;
  case 'B':
    chrt(5);
    printf("finish chrt,name = %c\n",name);
    break;
  /*���ڽ���C���õ�chrt(0)�����Բ���������*/
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
/* ʵ��һ�� A���һ�룬B���4�룬Ȼ��C��D���*/
/* ʵ�����ȥ��73��ע�ͣ��� A���һ�룬B���4�룬Ȼ��A��C��D���*/
/* ʵ������ȥ��74��ע�ͣ��� A���3�룬B���1�룬Ȼ��A��C��D���*/
