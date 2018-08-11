/*
 * 多线程拷贝,从t1.txt到t2.txt
 * */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#define MAX_THREAD 5/*线程数*/
#define PATH_LEN 20
typedef struct{
    int ifd;
    int ofd;
    int max;
}ARG;
off_t get_len(char * path_name){/* 获得文件的长度 */
    int fd;
    off_t len;
    if((fd = open("t1.txt",O_RDONLY))==-1){/* 打开读取文件 */
        perror("fail at function open");
        return -1;
    }
    if((len = lseek(fd,0,SEEK_END))==-1){/* 获得文件长度 */
        perror("fail at function lseek");
        return -1;
    }
    if(close(fd)==-1){
        perror("fail at function close");
        return -1;
    }
    return len;/* 返回文件长度 */
}
void * thfn(void * parg){/* 线程的函数 */
    ARG *arg = (ARG *)parg;
    ssize_t len;
    char buf[arg->max];
    if((len = read(arg->ifd,buf,(size_t)arg->max))==-1){/* 读操作 */
        perror("fail at function read");
        return (void *)1;
    }
    if(write(arg->ofd,buf,(size_t)len)==-1){/* 写操作 */
        perror("fail at function write");
        return (void *)1;
    }
    return NULL;
}
int main(){
    int i,max;
    ARG arg[MAX_THREAD];
    off_t ilen;
    pthread_t mtid;
    if((ilen = get_len("t1.txt"))==-1)/* 获得文件长度 */
        exit(1);
    max = (int)ilen/MAX_THREAD+1;/* 一次性读取的最大长度 */
    mtid = pthread_self();/* 主线程ID */
    i = 0;
    while(1){
        if(i >= MAX_THREAD) break;/* 文件读取结束 */
        else{/* 开线程 */
            arg[i].max = max;
            if((arg[i].ifd = open("t1.txt",O_RDONLY))==-1){/* 打开文件 */
                perror("fail at function open");
                exit(1);
            }
            if((arg[i].ofd = open("t2.txt",O_WRONLY))==-1){
                perror("fail at function open");
                exit(1);
            }
            if(lseek(arg[i].ifd,arg[i].max*i,SEEK_SET)==-1){/* 指针定位 */
                perror("fail at function lseek");
                exit(1);
            }
            if(lseek(arg[i].ofd,arg[i].max*i,SEEK_SET)==-1){
                perror("fail at function lseek");
                exit(1);
            }
            pthread_create(&mtid,NULL,thfn,&(arg[i]));/* 开线程 */
        }
        i++;
    }
    exit(0);
}