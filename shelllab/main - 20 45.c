#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<string.h>
#include<ctype.h>
#include<fcntl.h>
int i;
char* CommandHistory[50] = {NULL};
int HistoryIndex=0;
int houtai_flag=0;
void UpdateHistory(char *buf)
{
    if (strcmp(buf, "\n") == 0)
        return;
    char *temp = (char *)malloc(sizeof(buf));
    strcpy(temp, buf);
    CommandHistory[HistoryIndex++]=temp;
    return;
}
int fd_pre;
int fd_post;char buffer1[1024];char buffer2[1024];
int tiaoshi=0;
int main()
{
    while(1)
    {
        printf("input command:");
        fflush(stdout);

        //解析输入到shell上的字符串 ls   -a -l
        char buffer[1024];
        //printf("buffer:%s\n",buffer);
        int read_size = read(1, buffer, sizeof(buffer));
        fflush(stdout);
        printf("\n");
        //printf("%d\n",read_size);
        UpdateHistory(buffer);

        fflush(stdout);
        if (read_size > 0)
        {
            buffer[read_size - 1] = 0;//shell_argv的最后一位需要为0
        }


        char* shell_argv[32] = {NULL};//argv
        int shell_index = 0;//argc
        char* start = buffer;
        while (*start != '\0')
        {
            while (*start != '\0' && isspace(*start))
            {
                *start = '\0';
                start++;
            }
            shell_argv[shell_index++] = start;
            while (*start != '\0' && !isspace(*start))
            {
                start++;
            }
        }
        printf("argc::%d\n",shell_index);
        fflush(stdout);
        printf("shell_argv[0]:%s\n",shell_argv[0]);
        printf("shell_argv[1]:%s\n",shell_argv[1]);
        printf("shell_argv[2]:%s\n",shell_argv[2]);
        printf("shell_argv[3]:%s\n",shell_argv[3]);
        printf("shell_argv[4]:%s\n",shell_argv[4]);
        fflush(stdout);
        //exit
        if(strcmp("exit", shell_argv[0]) == 0)
            break;

        //history
        if(strcmp("history", shell_argv[0]) == 0)
        {
            if(shell_argv[1])
            {
                int times=atoi(shell_argv[1]);
                if(times>HistoryIndex-1 || times<0)
                {
                    printf("number error/n");
                    fflush(stdout);
                }
                else
                {
                    printf("times:%d\n",times);
                    for (i=HistoryIndex-2;i>HistoryIndex-2-times;i--)
                        printf("CommandHistory[%d]:%s\n",i,CommandHistory[i]);
                    fflush(stdout);
                }
            }
            else
            {
                for (i=HistoryIndex-2;i>=0;i--)
                    printf("CommandHistory[%d]:%s\n",i,CommandHistory[i]);
                fflush(stdout);
            }
        }
        //cd
        if(strcmp("cd", shell_argv[0]) == 0)
        {
            if(shell_argv[1])
            {
                int chdir_return= chdir(shell_argv[1]);
                if(chdir_return<0)
                    printf("cd error\n");
            }
            else
            {
                chdir("/root");
            }
        }
        //创建子进程来exec
        pid_t pid = vfork();//子进程与父进程共享相同的内存空间
        //vfork 之后得到的子进程，可以保证在调用 exit 或者 exec 系列调用之前，父进程都不会被执行。这是一个非常重要的特性

        if (pid < 0)
        {
            printf("vfork failure\n");
            exit(1);
        }
        else if (pid == 0)
        {
            printf("\n\nson:%d",tiaoshi++);fflush(stdout);
            //考虑重定向 >
            //在字符串数组中找重定向标志

            int ii = 0;
            int j=0;int k=0;
            int fugaixie_flag=0;
            int shuru_flag=0;
            int zhuijiaxie_flag=0;

            if (strcmp("&", shell_argv[HistoryIndex-1]) == 0)
                houtai_flag = 1;

            for (; shell_argv[ii] != NULL; ++ii )
            {
                if (strcmp(">", shell_argv[ii]) == 0)
                {
                    fugaixie_flag = 1;
                    break;
                }
            }
            for (; shell_argv[j] != NULL; ++j )
            {
                if (strcmp("<", shell_argv[j]) == 0)
                {
                    shuru_flag = 1;
                    break;
                }
            }
            for (; shell_argv[k] != NULL; ++k )
            {
                if (strcmp(">>", shell_argv[k]) == 0)
                {
                    zhuijiaxie_flag = 1;
                    break;
                }
            }

            int copyFd;
            shell_argv[i] = NULL;


            if (fugaixie_flag)
            {
                if (shell_argv[ii+1] == NULL)
                {
                    printf("command error\n");
                    fflush(stdout);
                    exit(1);
                }

                close(1);//关了输出
                int fd = open(shell_argv[ii+1], O_WRONLY | O_CREAT, 0777);//只写打开且不存在则创建 fd新建一个文件描述符

                copyFd = dup2(1, fd);//dup2(int oldfd,int newfd) 把输出重定向给fd
                //copyFd为新文件描述符
            }
            if (shuru_flag)
            {
                if (shell_argv[j+1] == NULL)
                {
                    printf("command error\n");
                    fflush(stdout);
                    exit(1);
                }

                close(0);//关了输入
                int fd2 = open(shell_argv[j+1], O_WRONLY | O_CREAT, 0777);//只读打开且不存在则创建 fd新建一个文件描述符

                copyFd = dup2(0, fd2);//dup2(int oldfd,int newfd) 把输出重定向给fd
                //copyFd为新文件描述符
            }

            if(zhuijiaxie_flag)
            {
                if (shell_argv[k+1] == NULL)
                {
                    printf("command error\n");
                    fflush(stdout);
                    exit(1);
                }

                //printf("buffer:%s\n",buffer);
                fd_pre = open(shell_argv[k+1], O_WRONLY | O_CREAT, 0777);
                read(fd_pre, buffer1, sizeof(buffer1));
                printf("\n\n\nbuffer1:%s\n\n\n",buffer1);fflush(stdout);
                close(1);//关了输出
                int fd = open(shell_argv[k+1], O_WRONLY | O_CREAT, 0777);//只写打开且不存在则创建 fd新建一个文件描述符

                copyFd = dup2(1, fd);//dup2(int oldfd,int newfd) 把输出重定向给fd
                printf("\n\n\nsuccess212\n\n\n");fflush(stdout);
            }

            //execvp(shell_argv[0], shell_argv);//环境变量中找ls文件 并执行 将shell_argv传给该文件

            if (fugaixie_flag)//执行完了关了重定向的输出再dup从copyFd回给标准输出
            {
                close(1);
                dup2(copyFd, 1);
            }
            if(shuru_flag)
            {
                close(0);
                dup2(copyFd, 0);
            }
            if(zhuijiaxie_flag)
            {
                close(1);
                dup2(copyFd, 1);
                fd_post = open(shell_argv[k+1], O_WRONLY | O_CREAT, 0777);
                read(fd_post, buffer2, sizeof(buffer2));
                strcat(buffer1,buffer2);

                close(1);
                copyFd = dup2(1, open(shell_argv[k+1], O_WRONLY | O_CREAT, 0777));
                printf("%s",buffer1);fflush(stdout);
                close(1);
                dup2(copyFd, 1);
            }
            exit(1);
        }
        else //father process
        {
            if(houtai_flag)
            {printf("run background\n");fflush(stdout);continue;}
            else
            {
                int status = 0;
                //int ret = waitpid(pid, &status, 0);//返回子进程结束状态值 即等子进程跑完再跑


                printf("father:%d\n\n",tiaoshi++);fflush(stdout);
                /*if (ret == pid)
                {
                    if (WIFEXITED(status))//子进程正常退出
                    {
                        // printf("exitCode is %d\n", WEXITSTATUS(status));
                    } else if (WIFSIGNALED(status))//进程异常终止
                    {
                        printf("signal is %d\n", WTERMSIG(status));
                        fflush(stdout);
                    }
                }*/
            }
        }
        //houtai_flag=0;buffer1[0]='\0';buffer2[0]='\0';//clear
    }
    return 0;
}