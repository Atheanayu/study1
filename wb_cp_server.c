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
void * thfn(void * arg){
    ARG *p = (ARG*) arg;
    int len=0;
    char *buf=NULL;
    char command[MAX_LINE];
    int n;
    if(read(p->cfd,command,MAX_LINE)==-1){
        perror("fail at function read1");
        return (void *)-1;
    }
    for(int i = 4;command[i] !='\0';i++){
        if('0'<=command[i]&&command[i]<='9'){
            len*=10;
            len+=(command[i]-'0');
        }else if(command[i]==' '){
            buf = (char *)malloc((size_t)len);
            p->max = len;
            len = 0;
        }
    }
    p->order = len;

    if(lseek(p->fd,(p->order)*(p->max),SEEK_SET)==-1){
        perror("fail at function lseek");
        return (void *)-1;
    }
    if((n = (int)read(p->cfd,buf,(size_t)p->max))==-1){
        perror("fail at function read2");
        return (void *)-1;
    }
    if(write(p->fd,buf,(size_t)n)==-1){
        perror("fail at function write");
        return (void *)-1;
    }
    close(p->fd);
    free(buf);
    return (void *)0;
}

int main(){
    struct sockaddr_in sin,cin;
    int lfd,i,fd;
    ARG arg[THR_NUM];
    pthread_t mtid;
    for(i = 0;i<THR_NUM;i++)
        arg[i].order = i;
    memset(&(sin),0,sizeof(struct sockaddr_in));
    sin.sin_port = htons(PORT);
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_family = AF_INET;
    if((lfd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("fail at function socket");
        exit(1);
    }
    if(bind(lfd,(const struct sockaddr*)&(sin),sizeof(struct sockaddr))==-1){
        perror("fail at function bind");
        exit(1);
    }
    if(listen(lfd,10)==-1){
        perror("fail at function listen");
        exit(1);
    }
    i = 0;
    while(1){
        socklen_t len = sizeof(struct sockaddr);
        if((arg[i].cfd = accept(lfd,(struct sockaddr*)&(cin),&len))==-1){
            perror("fail at function accept");
            exit(1);
        }
        if((arg[i].fd = open("t2.txt",O_WRONLY))==-1){
            perror("fail at function open");
            exit(1);
        }
        mtid = pthread_self();
        if(pthread_create(&mtid, NULL, thfn, &(arg[i].cfd))==-1) {
            perror("fail at function pthread_create");
            exit(1);
        }
        i++;
    }
    close(lfd);
    exit(0);
}