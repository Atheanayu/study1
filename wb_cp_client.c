#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define MAX_LINE 20
#define THR_NUM 5
static int PORT = 6000;
typedef struct {
    struct sockaddr_in sin;
    int sfd,order,fd,max;
}ARG;
int get_len(char * path){/* 获得文件的长度 */
    int fd,len;
    if((fd = open(path,O_RDONLY))==-1){
        perror("fail at function open");
        return -1;
    }
    if((len = (int)lseek(fd,0,SEEK_END))==-1){
        perror("fail at function lseek");
        return -1;
    }
    return len;
}
void * thfn(void *arg){/* 线程函数 */
    ARG * p = (ARG*)arg;
    char msg[p->max];
    char command[MAX_LINE];
    if((p->sfd = socket(AF_INET,SOCK_STREAM,0))==-1){/* 创建socket */
        perror("fail at function socket");
        return (void *)-1;
    }
    if(connect(p->sfd,(const struct sockaddr *)&(p->sin),sizeof(struct sockaddr))==-1){/* 发出连接请求 */
        perror("fail at function connect");
        return (void *)-1;
    }
    if(lseek(p->fd,(p->order)*(p->max),SEEK_SET)==-1){/* 定位文件指针 */
        perror("fail at function lseek");
        exit(1);
    }
    sprintf(command,"SUC %d %d",p->max,p->order);/* 写入命令 */
    if((int)read(p->fd,msg,(size_t)p->max)==-1){/* 从本地文件中读入 */
        perror("fail at function read");
        return (void *)-1;
    }
    if(write(p->sfd,command,MAX_LINE)==-1){/* 将命令传到server端 */
        perror("fail at function write");
        return (void *)-1;
    }
    if(write(p->sfd,msg,strlen(msg))==-1){/* 将读入内容传到server端 */
        perror("fail at function write");
        return (void *)-1;
    }
    close(p->sfd);
    close(p->fd);
    return (void *)0;
}
int main(){
    ARG arg[THR_NUM];
    pthread_t mtid;
    int len = get_len("t1.txt");/* 获得文件长度 */
    int max = len/THR_NUM + 1;/* 获得一次读入的最大长度 */
    for(int i = 0;i<THR_NUM;i++)/* 为文件指针排好顺序 */
        arg[i].order = i;
    for(int i = 0;i<THR_NUM;i++){
        bzero(&(arg[i].sin),sizeof(struct sockaddr_in));/* 清空地址结构 */
        /* 赋值 */
        arg[i].sin.sin_family = AF_INET;
        inet_pton(AF_INET, "127.0.0.1", &((arg[i].sin).sin_addr));
        arg[i].sin.sin_port = htons(PORT);

        arg[i].max = max;
        if((arg[i].fd=open("t1.txt",O_RDONLY))==-1){/* 打开要读的文件 */
            perror("fail at function open");
            exit(1);
        }
        mtid = pthread_self();
        pthread_create(&mtid,NULL,thfn,&(arg[i]));/* 开线程 */
    }
    sleep(10);
    exit(0);
}