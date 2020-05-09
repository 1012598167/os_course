#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> ////////////////////////////////////////
#include <unistd.h>

#include <string.h>
#include <signal.h>

#include <sys/wait.h>
#include <lib.h>
#include <time.h>

int main(void)
{
	int id;
	int count = 0;
	for (id = 0; id < 3; id++)
	{
		if (fork() == 0)
		{
			switch (id)
			{
			case 0:
				chrt(20);
				while (1)
				{
					sleep(1);
					printf("P %d heart beat %d \n", id, count++);
					if (count == 5)
					{

						chrt(5);
					}
				}
				break;

			case 1:
				chrt(15);
				while (1)
				{
					sleep(1);
					printf("P %d heart beat %d \n", id, count++);
				}
				break;

			case 2:
				while (1)
				{
					sleep(1);
					printf("P %d heart beat %d \n", id, count++);
					if (count == 10)
					{
						chrt(3);
					}
				}
				break;

				//case 3:
				//	chrt(2);
				//	for (cnt = 1; cnt < 14; cnt++) {
				//		sleep(1);
				//		printf("\n");
				//	}
			}
			exit(0);
		}
	}
	return 0;
}