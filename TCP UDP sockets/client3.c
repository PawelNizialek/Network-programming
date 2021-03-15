#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_pton() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <errno.h>

void check(int returned_number);

int main(int argc, char** argv) {

    if(argc!= 3){
        fprintf(stderr, "Invocation: %s <IPv4 ADDRESS> <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int ipv4;
    // deskryptor gniazda
    int sockfd = -1; 
    // bufor dla funkcji send()
    char buff[256];
    // rozmiar bufora w bajtach
    int nbytes = sizeof(buff);
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
    servaddr.sin_port = htons(atoi(argv[2]));
    /* przypisanie adresu IP wraz z przeksztalceniem i walidacja danych */
    ipv4 = inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    if(ipv4 == 0){
        fprintf(stderr, "Invalid network address!");
        exit(EXIT_FAILURE);
    }
    /* skojarzenie adresu zdalnego z gniazdem
    moga zostac wykorzystane funkcje send() oraz write()
    dane wysylane na adres okreslony przez funkcje connect() */
    check(connect(sockfd, (const struct sockaddr*) &servaddr, addrlen));
    /* wyslanie danych za pomoca funkcji send() na adres
    okreslony przez funkcje connect() 
    zogdnie z poleceniem znak konca linii powoduje przerwanie petli*/
    do{
        printf("Enter a message:\n");
        fgets(buff, 255, stdin);
        send(sockfd, buff, nbytes, 0);
    }while(buff[0]!='\n');
    recv(sockfd, buff, nbytes, 0);
    close(sockfd);
}
void check(int returned_number){
    if(returned_number==-1) exit(-1);
}
