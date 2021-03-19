#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
// #include <sys/select.h>

void check(int returned_number){
    if(returned_number==-1) {
        fprintf(stdout, "error");
        exit(-1);
    }
}
int main(int argc, char** argv) {
    char buff[256];
    char addr_buff[256];
    char *message = "Laboratorium PUS";;
    int sockfd=0;
    int connfd=0;
    socklen_t addrlen = 0; 
    struct sockaddr_in server;                
    /* utworzenie gniazda komunikacyjnego; zwraca deskryptor 
    wykorzystywany typ gniazda: strumieniowe */
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd==-1){
        fprintf(stdout, "socket()");
        exit(EXIT_FAILURE);
    }
    /* stosowanie konwencji wyzerowania calej gniazdowej struktury adresowej */
    memset(&server, 0, sizeof(server));
    addrlen = sizeof(server); 
    // okreslenie rozmiaru struktury gniazdowej
    server.sin_family = AF_INET;
    // przypisanie numeru portu
    server.sin_port = htons(atoi(argv[1]));
    // serwer moze odbierac dane z dowolnego interfejsu sieciowego
    server.sin_addr.s_addr = htonl(INADDR_ANY);;
    // server.sin6_flowinfo = 
    // server.sin6_scope_id = 
    if(bind(sockfd, (struct sockaddr*) &server, sizeof(struct sockaddr_in))==-1){
        fprintf(stdout, "bind()");
        exit(EXIT_FAILURE);
    }
    /* Przeksztalcenie gniazda niepolaczonego w bierne: 
    Nadanie dlugosci kolejki */
    if(listen(sockfd, 10)==-1){
        fprintf(stdout, "listen()");
        exit(EXIT_FAILURE);
    }
    for(;;)
    {
        puts("Waiting for connection.");
        connfd = accept(sockfd, (struct sockaddr*)&server, &addrlen);
        fprintf(stdout, "Connected. Address: %s:%d.\n",
                        inet_ntop(AF_INET, &server.sin_addr, addr_buff, sizeof(addr_buff)),
                        ntohs(server.sin_port));
        fprintf(stdout, "Sending message\n");
        sleep(1);
        write(connfd, message, strlen(message));
        // if(IN6_IS_ADDR_V4MAPPED(&server.sin_addr)){
        //     fprintf(stdout, "\n%s is IPv4-mapped IPv6\n", inet_ntop(AF_INET, &server.sin_addr, addr_buff, sizeof(addr_buff)));
        // }
        fprintf(stdout, "Closing client socket.\n");
        close(connfd);
    }
    close(sockfd);
}
