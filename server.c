#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#define THR_NUM 3
#define MAX_LINE 20
static int PORT = 6000;
char * msg_handler(char * msg){
    if(msg != NULL)
        for(int i = 0;msg[i]!='\0';i++)
            msg[i] = (char)tolower(msg[i]);
    return msg;
}
typedef struct{
    struct sockaddr_in sin,cin;
    int lfd,cfd;
    int order;
}ARG;
void * thfn(void * arg){
    int *p = (int*) arg;
    char buf[MAX_LINE];
    int n;
    if((n = (int)read(*p,buf,MAX_LINE))==-1){
        perror("fail at function read");
        return (void *)-1;
    }
    if(write(STDOUT_FILENO,buf,n)==-1){
        perror("fail at function write");
        return (void *)-1;
    }
    msg_handler(buf);
    if(write(*p,buf,(size_t)n)==-1){
        perror("fail at function write");
        return (void *)-1;
    }
    return (void *)0;
}

int main(){
    struct sockaddr_in sin,cin;
    int lfd,cfd;
    ARG arg[THR_NUM];
    pthread_t mtid;
    for(int i = 0;i<THR_NUM;i++)
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
    while(1) {
        socklen_t len = sizeof(struct sockaddr);
        if((cfd = accept(lfd,(struct sockaddr*)&(cin),&len))==-1){
            perror("fail at function accept");
            exit(1);
        }
        mtid = pthread_self();
        //pthread_create(&mtid, NULL, thfn, &cfd);
        thfn(&cfd);
    }
    close(lfd);
    sleep(1000);
    exit(0);
}