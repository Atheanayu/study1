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
//static char msg[THR_NUM][MAX_LINE] = {"AAA","BBB","CCC"};
typedef struct {
    struct sockaddr_in sin;
    int sfd,order,fd,max;
}ARG;
int get_len(char * path){
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
void * thfn(void *arg){
    ARG * p = (ARG*)arg;
    char msg[p->max];
    char command[MAX_LINE];
    if((p->sfd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("fail at function socket");
        return (void *)-1;
    }
    if(connect(p->sfd,(const struct sockaddr *)&(p->sin),sizeof(struct sockaddr))==-1){
        perror("fail at function connect");
        return (void *)-1;
    }
    if(lseek(p->fd,(p->order)*(p->max),SEEK_SET)==-1){
        perror("fail at function lseek");
        exit(1);
    }
    sprintf(command,"SUC %d %d",p->max,p->order);
    if((int)read(p->fd,msg,(size_t)p->max)==-1){
        perror("fail at function read");
        return (void *)-1;
    }
    if(write(p->sfd,command,MAX_LINE)==-1){
        perror("fail at function write");
        return (void *)-1;
    }
    if(write(p->sfd,msg,strlen(msg))==-1){
        perror("fail at function write");
        return (void *)-1;
    }
//    if(read(p->sfd,p->msg2,MAX_LINE)==-1){
//        perror("fail at function read");
//        return (void *)-1;
//    }
//    if(write(STDOUT_FILENO,p->msg2,strlen(p->msg2))==-1){
//        perror("fail at function write");
//        return (void *)-1;
//    }
//    fflush(stdout);
    close(p->sfd);
    close(p->fd);
    return (void *)0;
}
int main(){
    ARG arg[THR_NUM];
    pthread_t mtid;
    int fd;
    int len = get_len("t1.txt");
    int max = len/THR_NUM + 1;
    for(int i = 0;i<THR_NUM;i++)
        arg[i].order = i;
    for(int i = 0;i<THR_NUM;i++){
        bzero(&(arg[i].sin),sizeof(struct sockaddr_in));
        arg[i].sin.sin_family = AF_INET;
        inet_pton(AF_INET, "127.0.0.1", &((arg[i].sin).sin_addr));
        arg[i].sin.sin_port = htons(PORT);
        arg[i].max = max;
        if((arg[i].fd=open("t1.txt",O_RDONLY))==-1){
            perror("fail at function open");
            exit(1);
        }
        mtid = pthread_self();
        pthread_create(&mtid,NULL,thfn,&(arg[i]));
    }
    sleep(10);
    exit(0);
}