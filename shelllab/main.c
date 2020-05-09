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

void UpdateHistory(char *buf)
{
    if (strcmp(buf, "\n") == 0)
        return;
    char *temp = (char *)malloc(sizeof(buf));
    strcpy(temp, buf);
    CommandHistory[HistoryIndex++]=temp;
    return;
}
int pipe_i;
int fugaixie_flag=0;
int shuru_flag=0;
int houtai_flag=0;
int zhuijiaxie_flag=0;
int pipe_flag=0;
int copyFd;
int pipe_fd[2],pipe_fd2[2];
int pipe_flag_pos[2];
pid_t pipe_pid;
int main()
{
    while(1)
    {
        printf("input command:");
        fflush(stdout);

        //解析输入到shell上的字符串 ls   -a -l
        char buffer[1024];
        int read_size = read(1, buffer, sizeof(buffer));
        if (strcmp(buffer, "\n") == 0)
            continue;
        //printf("buffer:%s\n",buffer);
        fflush(stdout);
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
                    printf("number error\n");
                    fflush(stdout);
                }
                else
                {
                    for (i=HistoryIndex-2;i>HistoryIndex-2-times;i--)
                        printf("CommandHistory[%d]:%s",i,CommandHistory[i]);
                    fflush(stdout);
                }
            }
            else
            {
                for (i=HistoryIndex-2;i>=0;i--)
                    printf("CommandHistory[%d]:%s",i,CommandHistory[i]);
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

            //考虑重定向 >
            //在字符串数组中找重定向标志

            int ii = 0;
            int j=0;int k=0;int l=0;


            if (strcmp("&", shell_argv[shell_index-1]) == 0)
            {houtai_flag = 1;shell_argv[shell_index-1] =NULL;}

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
            int i_pos=0;
            for (; shell_argv[l] != NULL; ++l )
            {
                if (strcmp("|", shell_argv[l]) == 0)
                {
                    pipe_flag += 1;
                    pipe_flag_pos[i_pos++]=l;
                }
            }

            shell_argv[ii] = NULL;
            shell_argv[j] = NULL;
            shell_argv[k] = NULL;

            if (fugaixie_flag)
            {
                if (shell_argv[ii+1] == NULL)
                {
                    printf("command error\n");
                    fflush(stdout);
                    exit(1);
                }

                close(1);//关了输出
                int fd = open(shell_argv[ii+1], O_WRONLY | O_CREAT |O_TRUNC, 0777);//只写打开且不存在则创建 fd新建一个文件描述符

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
                int fd2 = open(shell_argv[j+1], O_RDONLY, 0777);//只读打开 fd新建一个文件描述符

                copyFd = dup2(0, fd2);//dup2(int oldfd,int newfd) 把输出重定向给fd
                //copyFd为新文件描述符
            }
            if (zhuijiaxie_flag)
            {
                if (shell_argv[k+1] == NULL)
                {
                    printf("command error\n");
                    fflush(stdout);
                    exit(1);
                }

                close(1);//关了输出
                int fd = open(shell_argv[k+1], O_WRONLY | O_APPEND| O_CREAT, 0777,'a');//只写打开且不存在则创建 fd新建一个文件描述符

                copyFd = dup2(1, fd);//dup2(int oldfd,int newfd) 把输出重定向给fd
                //copyFd为新文件描述符
            }
            if(pipe_flag)
            {
                int status;
                if(pipe_flag==1)
                {
                    close(pipe_fd2[0]);
                    close(pipe_fd2[1]);
                    pipe(pipe_fd);
                    for(pipe_i=0;pipe_i<2;pipe_i++)
                    {
                        if((pipe_pid=fork())==0)
                            break;
                        else if(pid<0)
                            exit(1);
                    }
                    if(pipe_i==0)
                    {
                        close(pipe_fd[0]);
                        dup2(pipe_fd[1],1);
                        shell_argv[pipe_flag_pos[0]]=NULL;
                        execvp(shell_argv[0],shell_argv);
                    }
                    else if(pipe_i==1)
                    {
                        close(pipe_fd[1]);
                        dup2(pipe_fd[0],0);
                        execvp(shell_argv[pipe_flag_pos[0]+1],shell_argv+pipe_flag_pos[0]+1);
                    }
                    if(pipe_i==2)
                    {
                        close(pipe_fd[0]);
                        close(pipe_fd[1]);
                        while( waitpid(-1,&status,WNOHANG) != -1 ){}
                        //waitpid(pipe_pid, &status, 0);
                    }
                }
                else if (pipe_flag==2)
                {
                    pipe(pipe_fd);
                    pipe(pipe_fd2);
                    for(pipe_i=0;pipe_i<3;pipe_i++)
                    {
                        if((pipe_pid=fork())==0)
                            break;
                        else if(pid<0)
                            exit(1);
                    }
                    if(pipe_i==0)
                    {
                        close(pipe_fd2[0]);
                        close(pipe_fd2[1]);
                        close(pipe_fd[0]);
                        dup2(pipe_fd[1],1);
                        shell_argv[pipe_flag_pos[0]]=NULL;
                        execvp(shell_argv[0],shell_argv);
                    }
                    else if(pipe_i==1)
                    {
                        close(pipe_fd[1]);
                        close(pipe_fd2[0]);
                        dup2(pipe_fd[0],0);
                        dup2(pipe_fd2[1],1);
                        shell_argv[pipe_flag_pos[1]]=NULL;
                        execvp(shell_argv[pipe_flag_pos[0]+1],shell_argv+pipe_flag_pos[0]+1);
                    }
                    else if(pipe_i==2)
                    {
                        close(pipe_fd[0]);
                        close(pipe_fd[1]);
                        close(pipe_fd2[1]);
                        //子进程3 wc -l 从管道2中读入数据,将最终结果输出的屏幕
                        dup2(pipe_fd2[0],0);
                        execvp(shell_argv[pipe_flag_pos[1]+1],shell_argv+pipe_flag_pos[1]+1);
                    }
                    if(pipe_i==3)
                    {
                        close(pipe_fd[0]);
                        close(pipe_fd[1]);
                        close(pipe_fd2[0]);
                        close(pipe_fd2[1]);
                        while( waitpid(-1,&status,WNOHANG) != -1 ){}
                        //waitpid(pipe_pid, &status, 0);
                    }
                }
            }

            if(!pipe_flag)
                execvp(shell_argv[0], shell_argv);//环境变量中找ls文件 并执行 将shell_argv传给该文件



            exit(1);
        }
        else //father process
        {
            if (houtai_flag)
            {
                printf("run background\n");
                fflush(stdout);
                continue;
            }
            else
            {
                int status = 0;
                int ret = waitpid(pid, &status, 0);//返回子进程结束状态值 即等子进程跑完再跑

                /*if(pipe_flag)
                {
                    printf("\npipepipepipe\n");
                    if(pipe_flag==1)
                    {
                        if(pipe_i==2)
                        {
                            close(pipe_fd[0]);
                            close(pipe_fd[1]);
                            while( waitpid(-1,&status,WNOHANG) != -1 ){}
                            //waitpid(pipe_pid, &status, 0);
                        }
                    }
                    else if (pipe_flag==2)
                    {
                        if(pipe_i==3)
                        {
                            close(pipe_fd[0]);
                            close(pipe_fd[1]);
                            close(pipe_fd2[0]);
                            close(pipe_fd2[1]);
                            while( waitpid(-1,&status,WNOHANG) != -1 ){}
                            //waitpid(pipe_pid, &status, 0);
                        }
                    }
                    pipe_flag=0;
                }*/

                if (ret == pid)
                {
                    if (WIFEXITED(status))//子进程正常退出
                    {
                        // printf("exitCode is %d\n", WEXITSTATUS(status));
                        if (fugaixie_flag)//执行完了关了重定向的输出再dup从copyFd回给标准输出
                        {
                            close(1);
                            dup2(copyFd, 1);
                            fugaixie_flag = 0;
                        }
                        if (shuru_flag)
                        {
                            close(0);
                            dup2(copyFd, 0);
                            shuru_flag = 0;
                        }
                        if (zhuijiaxie_flag)//执行完了关了重定向的输出再dup从copyFd回给标准输出
                        {
                            close(1);
                            dup2(copyFd, 1);
                            zhuijiaxie_flag = 0;
                        }



                    } else if (WIFSIGNALED(status))//进程异常终止
                    {
                        printf("signal is %d\n", WTERMSIG(status));
                        fflush(stdout);
                    }
                }
            }
        }
        houtai_flag=0;
        // buffer1[0]='\0';buffer2[0]='\0';//clear
    }
    return 0;
}