#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "libpalindrome.h"

void check(int returned_number);

int main(int argc, char** argv) {

    if(argc!= 2){
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int ipv4;
    // deskryptor gniazda
    int sockfd = -1; 
    // bufor dla funkcji send()
    char buff[256];
    char addr_buff[256];
    // rozmiar bufora w bajtach
    int nbytes = sizeof(buff);
    int IS_NUMBER=0;
    //gniazdowa struktura adresowa
    struct sockaddr_in servaddr; 
    //rozmiar struktury gniazdowej
    socklen_t addrlen = 0; 
    /* utworzenie gniazda komunikacyjnego; zwraca deskryptor 
    wykorzystywany typ gniazda: datagramowe */
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
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
    fprintf(stdout, "Server is listening for incoming connection...\n");
    // przypisanie nazwy gniazdu (kombinacja adresu IP oraz numeru portu)
    // wraz z walidacja
    check(bind(sockfd, (struct sockaddr*) &servaddr, addrlen));
    do{
        IS_NUMBER=1;
        check(recvfrom(sockfd, buff, nbytes, 0, (struct sockaddr*) &servaddr, &addrlen));
        fprintf(stdout, "UDP datagram received from %s:%d. Message: %s",
            inet_ntop(AF_INET, &servaddr.sin_addr, addr_buff, sizeof(addr_buff)),
            ntohs(servaddr.sin_port), buff
           );
        int i=0;
        for (i=0; i<strlen(buff)-1; i++){
            if(!isdigit(buff[i])&&buff[i]!=32) IS_NUMBER=0;
            if(!IS_NUMBER) break;
        }
        if(IS_NUMBER){
            if(is_palindrome(buff, strlen(buff))){
                char message[13] = "Palindrome";
                check(sendto(sockfd, message, sizeof(message), 0, (struct sockaddr*) &servaddr, addrlen));
        }
        }
    }while(buff[0]!='\n');
    check(sendto(sockfd, buff, sizeof(buff), 0, (struct sockaddr*) &servaddr, addrlen));
    close(sockfd);
}
void check(int returned_number){
    if(returned_number==-1) {
        fprintf(stdout, "error");
        exit(-1);
    }
}