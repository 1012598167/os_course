#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <lib.h>
#include <time.h>




void loop(char);




int main(int argc, char const *argv[])
{
	int count = 2;
	pid_t the_pid;

	the_pid=fork();

	if(the_pid>0)
	{
		chrt(20);
		loop('A');

	}


	else
	{

		the_pid=fork();
		if(the_pid>0)
		{
			chrt(15);
			loop('B');
		}
		else
			loop('c');
	}





	return 0;
}


void loop(char name)
{
	int loop = 0;
	while(1)
	{
		if(name=='A' && loop==5)
			chrt(5);
		if(name=='B' && loop==10)
			chrt(3);
		sleep(1);
		printf("P %c heart beat %d \n",name,loop );
		++loop;
	}
}







