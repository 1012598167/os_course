
#include <stdio.h>

#include <stdlib.h>

#include <dirent.h>

#include <sys/stat.h>

#include <unistd.h>

#include <string.h>

#include <time.h>



#define MAX_COUNT 100 //总共能够统计的文件项数，用这个值去分配保存信息的数组的大小

#define FILE_LENGTH_MAX 20 // 自定义的一个文件的最长长度。



enum EOPT

{

    E,E_a,E_l,E_r,E_num

}; // 相应的控制选项，E为ls，E_a为ls -a，E_r为ls -R，E_l为ls -l

int getOPT[E_num] = {0}; // 一个保存相应选项的状态数组



typedef struct

{

    mode_t	st_mode;

    uid_t	st_uid;

    gid_t	st_gid;

    off_t	st_size;

    nlink_t	st_nlink;

    char*	fname;

    struct timespec	st_time;



}fileInfo; // 由于很多信息在这里没用，所以自定义了一个文件项信息结构

static fileInfo* fInfo; // 所有的函数通过这个指针对查询结果进行操作

static int fileCount; // 统计目前已经统计了多少个文件项



int query(char*); // 普通查询，将结果存于fInfo数组中

int query_r(char*); // 递归遍历查询

int isHideFile(const char*); // 判断是否是隐藏文件

void print(); // 没有-l选项的打印

void printl();  // 附加-l选项

void printInfo(const fileInfo);

int getmode(const char*); // 获取相应选项信息

void rprintl(const char*,struct stat); // 递归遍历中的附加-l选项打印

void rprint(const char*); // 递归遍历中的普通打印

int main(int argc,char *argv[])

{

    char buf[1024]; // 用来保存文件名

    //getcwd(buf,1024);

    if(argv[argc-1][0] == '-' || argc == 1)

        getcwd(buf,1024);

    else

        strcpy(buf,argv[argc-1]);

    int i = 1;

    for(;i<argc;i++)

        getmode(argv[i]);

    if(getOPT[E_l]!=1) // 如果没有指定-l，默认就是不打印文件的详细信息

        getOPT[E] = 1;

    if(getOPT[E_r] == 0){

        query(buf);

        if(getOPT[E_l] == 1)

            printl();

        else

            print();

    }

    else

        query_r(buf);

    return EXIT_SUCCESS;

}

int getmode(const char* arg) // 将相应的选项变为1

{

    if(arg[0]!='-')

        return -1;

    const char *p;

    p = &arg[1];

    while(*p!=0)

    {

        if(*p == 'a')

            getOPT[E_a] = 1;

        else if(*p == 'l')

            getOPT[E_l] = 1;

        else if(*p == 'R')

            getOPT[E_r] = 1;

        p++;

    }

    return 0;

}

int query(char* pathname)

{

    fInfo = (fileInfo*)malloc(sizeof(fileInfo)*MAX_COUNT);

    if(fInfo == NULL)

    {

        printf("fInfo allocate error\n");

        return -1;

    }

    struct dirent *dirp;

    DIR *dp;

    struct stat bufstat;

    fileInfo *tmp = fInfo;

    if((dp = opendir(pathname)) == NULL)

    {

        printf("opendir error\n");

        return -1;

    }

    while((dirp = readdir(dp))!=NULL)

    {

        fileCount++;

        strcpy(&pathname[n],dirp->d_name);

        if(lstat(pathname,&bufstat) < 0)

        {

            printf("lstat error\n");

            return -1;

        }

        tmp->fname = (char*)malloc(sizeof(char)*(strlen(dirp->d_name)+1));

        strcpy(tmp->fname,dirp->d_name);

        tmp->st_mode = bufstat.st_mode;

        tmp->st_uid = bufstat.st_uid;

        tmp->st_gid = bufstat.st_gid;

        tmp->st_size = bufstat.st_size;

        tmp->st_nlink = bufstat.st_nlink;

        tmp->st_time = bufstat.st_ctim;

        tmp++;

    }

    pathname[n-1] = 0;

    closedir(dp);

    return 0;

}

int isHideFile(const char*pathname)

{

    if(pathname[0] == '.')

        return 1;

    else

        return 0;

}

void print()

{

    int i = 0;

    for(;i<fileCount;i++){

        if(getOPT[E_a] == 0 && isHideFile(fInfo[i].fname)) // 如果没有指定-a选项，并且当前文件为隐藏文件，则跳过

            continue;

        printf("%s\n",fInfo[i].fname);

    }

}

void printl()

{

    int i = 0;

    for(;i<fileCount;i++)

    {

        if(getOPT[E_a] == 0 && isHideFile(fInfo[i].fname))

            continue;

        printInfo(fInfo[i]);

        printf("\n");

    }

}

void printInfo(const fileInfo file) // 打印详细的信息，这里是仿windows下的信息显示

{

    struct tm *p;

    p = gmtime(&(file.st_time.tv_sec));

    printf("%d/%d/%d\t%d:%d:%d\t",(1900+p->tm_year),(1+p->tm_mon),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

    if(S_ISDIR(file.st_mode))

        printf("\t<DIR>\t\t");

    else

        printf("\t\t\t");

    printf("%s",file.fname);

}



int query_r(char* pathname)

{

    struct dirent *dirp;

    DIR * dp;

    struct stat bufstat;

    if((lstat(pathname,&bufstat)) < 0)

    {

        printf("lstat error\n");

        return -1;

    }

    if(S_ISDIR(bufstat.st_mode) == 0) {// 对普通文件进行处理

        if(getOPT[E_l] == 1)

            rprintl(pathname,bufstat);

        else

            rprint(pathname);

        return 0;

    }

    int n = strlen(pathname);

    pathname[n++] = '/';

    pathname[n] = 0;

    if((dp = opendir(pathname)) == NULL)

    {

        printf("opendir error\n");

        return -1;

    }

    printf("%s:\n",pathname);

    while((dirp = readdir(dp)) != NULL)

    {

        if(strcmp(dirp->d_name,".") == 0 || strcmp(dirp->d_name,"..") == 0) // 跳过递归遍历.和..防止死循环

        {

            if(getOPT[E_a] == 1 && getOPT[E] == 1)

                printf("%s\n",dirp->d_name);

            if(getOPT[E_a] == 1 && getOPT[E_l] == 1)

                rprintl(dirp->d_name,bufstat);

            continue;

        }

        strcpy(&pathname[n],dirp->d_name);

        query_r(pathname);

    }

    pathname[n] = 0;

    closedir(dp);

}

const char* func(const char* pathname) // 去除文件名前相应的路径

{

    if(pathname[0] == '.')

        return pathname;

    const char*p = &pathname[strlen(pathname)-1];

    while(*p!='/')

        --p;

    return p+1;

}

void rprintl(const char*pathname,struct stat bufstat)

{

    struct tm *p;

    p = gmtime(&(bufstat.st_ctim.tv_sec));

    printf("%d/%d/%d\t%2d:%2d:%2d\t",(1900+p->tm_year),1+p->tm_mon,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

    printf("\t\t%s\n",func(pathname));

}

void rprint(const char* pathname)

{

    printf("%s\n",func(pathname));

}
