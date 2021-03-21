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
#include <sys/select.h>

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
    struct sockaddr_in6 server;                
    /* utworzenie gniazda komunikacyjnego; zwraca deskryptor 
    wykorzystywany typ gniazda: strumieniowe */
    sockfd = socket(PF_INET6, SOCK_STREAM, 0);
    check(sockfd);
    /* stosowanie konwencji wyzerowania calej gniazdowej struktury adresowej */
    memset(&server, 0, sizeof(server));
    addrlen = sizeof(server); 
    // okreslenie rozmiaru struktury gniazdowej
    server.sin6_family = AF_INET6;
    // przypisanie numeru portu
    server.sin6_port = htons(atoi(argv[1]));
    // serwer moze odbierac dane z dowolnego interfejsu sieciowego
    server.sin6_addr = in6addr_any;
    check(bind(sockfd, (struct sockaddr*) &server, sizeof(struct sockaddr_in6)));
    /* Przeksztalcenie gniazda niepolaczonego w bierne: 
    Nadanie dlugosci kolejki */
    check(listen(sockfd, 10));
    for(;;)
    {
        connfd = accept(sockfd, (struct sockaddr*)&server, &addrlen);
        fprintf(stdout, "Connected. Address: %s:%d.",
                        inet_ntop(AF_INET6, &server.sin6_addr, addr_buff, sizeof(addr_buff)),
                        ntohs(server.sin6_port));
        write(connfd, message, strlen(message));
        if(IN6_IS_ADDR_V4MAPPED(&server.sin6_addr)){
            fprintf(stdout, "\n%s is IPv4-mapped IPv6\n", inet_ntop(AF_INET6, &server.sin6_addr, addr_buff, sizeof(addr_buff)));
        }
        close(connfd);
    }
    close(sockfd);
}
