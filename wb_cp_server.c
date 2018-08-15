#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#define THR_NUM 5
#define MAX_LINE 20
static int PORT = 6000;
typedef struct{
    int cfd,fd;
    int order,max;
}ARG;
void * thfn(void * arg){/* 线程函数 */
    ARG *p = (ARG*) arg;
    int len=0;
    char *buf=NULL;
    char command[MAX_LINE];
    int n;
    if(read(p->cfd,command,MAX_LINE)==-1){/* 读取命令 */
        perror("fail at function read1");
        return (void *)-1;
    }
    for(int i = 4;command[i] !='\0';i++){/* 读取命令中的有用信息：一次读取的最大长度和自己是第几个文件指针 */
        if('0'<=command[i]&&command[i]<='9'){
            len*=10;
            len+=(command[i]-'0');
        }else if(command[i]==' '){/* 说明到了下一个参数 */
            buf = (char *)malloc((size_t)len);/* max即为给buf分配的长度 */
            p->max = len;/* len第一次读的是max，将max赋值 */
            len = 0;/* 为读下一个参数做准备 */
        }
    }
    p->order = len;/* 第二次读出的len是order的值 */

    if(lseek(p->fd,(p->order)*(p->max),SEEK_SET)==-1){/* 为文件指针定位 */
        perror("fail at function lseek");
        return (void *)-1;
    }
    if((n = (int)read(p->cfd,buf,(size_t)p->max))==-1){/* 读从client端传入的内容 */
        perror("fail at function read2");
        return (void *)-1;
    }
    if(write(p->fd,buf,(size_t)n)==-1){/* 将读入的内容传到本地的文件中 */
        perror("fail at function write");
        return (void *)-1;
    }
    close(p->fd);/* 关闭文件描述符 */
    free(buf);/* free掉malloc的内存 */
    return (void *)0;
}

int main(){
    struct sockaddr_in sin,cin;
    int lfd,i;
    ARG arg[THR_NUM];
    pthread_t mtid;
    memset(&(sin),0,sizeof(struct sockaddr_in));/* 清空地址结构 */
    sin.sin_port = htons(PORT);
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_family = AF_INET;
    if((lfd = socket(AF_INET,SOCK_STREAM,0))==-1){/* 创建连接socket */
        perror("fail at function socket");
        exit(1);
    }
    if(bind(lfd,(const struct sockaddr*)&(sin),sizeof(struct sockaddr))==-1){/* 将连接socket和地址结构绑定 */
        perror("fail at function bind");
        exit(1);
    }
    if(listen(lfd,10)==-1){/* 监听，此为lfd负责的 */
        perror("fail at function listen");
        exit(1);
    }
    i = 0;
    while(1){
        socklen_t len = sizeof(struct sockaddr);
        if((arg[i].cfd = accept(lfd,(struct sockaddr*)&(cin),&len))==-1){/* 如果接收到了来自client的连接请求 */
            perror("fail at function accept");
            exit(1);
        }
        if((arg[i].fd = open("t2.txt",O_WRONLY))==-1){/* 打开本地文件 */
            perror("fail at function open");
            exit(1);
        }
        mtid = pthread_self();
        if(pthread_create(&mtid, NULL, thfn, &(arg[i].cfd))==-1) {/* 创建线程 */
            perror("fail at function pthread_create");
            exit(1);
        }
        i++;
    }
    close(lfd);
    exit(0);
}