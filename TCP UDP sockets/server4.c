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

    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char buff[256];
    char addr_buff[256];
    int sockfd=0;
    int connfd=0;
    // kolekcje deskrytporow pliku
    fd_set c_sockets, r_sockets; 

    struct sockaddr_in servaddr; 
    socklen_t addrlen = 0; 

    /* utworzenie gniazda komunikacyjnego; zwraca deskryptor 
    wykorzystywany typ gniazda: strumieniowe */
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    check(sockfd);
    /* stosowanie konwencji wyzerowania calej gniazdowej struktury adresowej */
    memset(&servaddr, 0, sizeof(addrlen));
    // okreslenie rozmiaru struktury gniazdowej
    addrlen = sizeof(servaddr); 
    servaddr.sin_family = AF_INET;
    // przypisanie numeru portu
    servaddr.sin_port = htons(atoi(argv[1]));
    // serwer moze odbierac dane z dowolnego interfejsu sieciowego
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    check(bind(sockfd, (struct sockaddr*) &servaddr, addrlen));
    /* Przeksztalcenie gniazda niepolaczonego w bierne: 
    Nadanie dlugosci kolejki */
    check(listen(sockfd, 10));
    FD_ZERO(&c_sockets);
    FD_SET(sockfd, &c_sockets);
    while (1)
    {
        r_sockets = c_sockets;
        if(select(FD_SETSIZE,&r_sockets, NULL, NULL, NULL)<0){
            perror("error with select function!");
            exit(EXIT_FAILURE);
        }
        
        for(int i = 0; i < FD_SETSIZE; i++){
            if(FD_ISSET(i, &r_sockets)) {
                fprintf(stdout, "Server is listening for incoming connection...\n");
                if(i == sockfd) {
                    connfd = accept(sockfd, (struct sockaddr*)&servaddr, &addrlen);
                    FD_SET(connfd, &c_sockets);
                } else {
                    read(connfd, buff, sizeof(buff));
                    fprintf(stdout, "UDP datagram received from %s:%d. Message: %s",
                        inet_ntop(AF_INET, &servaddr.sin_addr, addr_buff, sizeof(addr_buff)),
                        ntohs(servaddr.sin_port), buff
                         );
                    write(connfd, buff, strlen(buff));
                }
            }
        }
        close(sockfd);
    }
}
