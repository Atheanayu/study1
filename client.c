#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#define MAX_LINE 20
#define THR_NUM 3
static int PORT = 6000;
static char msg[THR_NUM][MAX_LINE] = {"AAA","BBB","CCC"};
typedef struct {
    struct sockaddr_in sin;
    int sfd,order;
}ARG;
void * thfn(void *arg){
    ARG * p = (ARG*)arg;
    char msg2[MAX_LINE];
    if((p->sfd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("fail at function socket");
        exit(1);
    }
    if(connect(p->sfd,(const struct sockaddr *)&(p->sin),sizeof(struct sockaddr))==-1){
        perror("fail at function connect");
        exit(1);
    }
    if(write(p->sfd,msg[p->order],strlen(msg[p->order]))==-1){
        perror("fail at function write");
        return (void *)-1;
    }
    if(read(p->sfd,msg2,MAX_LINE)==-1){
        perror("fail at function read");
        return (void *)-1;
    }
    if(write(STDOUT_FILENO,msg2,strlen(msg2))==-1){
        perror("fail at function write");
        return (void *)-1;
    }
    close(p->sfd);
    return (void *)0;

}
int main(){
    ARG arg[THR_NUM];
    pthread_t mtid;
    for(int i = 0;i<THR_NUM;i++)
        arg[i].order = i;
    for(int i = 0;i<THR_NUM;i++){
        bzero(&(arg[i].sin),sizeof(struct sockaddr_in));
        arg[i].sin.sin_family = AF_INET;
        inet_pton(AF_INET, "127.0.0.1", &((arg[i].sin).sin_addr));
        arg[i].sin.sin_port = htons(PORT);
        mtid = pthread_self();
        //pthread_create(&mtid,NULL,thfn,&(arg[i]));
        thfn(&(arg[i]));
    }
    sleep(10);
    exit(0);
}