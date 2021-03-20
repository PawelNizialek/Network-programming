#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>

int main(int argc, char** argv) {

    if (argc != 3) {
        fprintf(stderr, "Invocation: %s <IPv4/IPv6 ADDRESS> <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    int retval;
    char buff[256];

    struct addrinfo tmp;
    struct addrinfo *res;
    struct addrinfo *rec;
    struct sockaddr_storage addr_storage;
    memset(&tmp, 0, sizeof(struct addrinfo));
    tmp.ai_family = AF_INET;
    tmp.ai_socktype = SOCK_STREAM;
    tmp.ai_protocol = IPPROTO_TCP;
    socklen_t storagelen=0;
    char hbuf[10];
    char sbuf[10];

    if (getaddrinfo(argv[1], argv[2], &tmp, &res) != 0){
        perror("Translation error");
        exit(EXIT_FAILURE);
    }

    for(rec=res; rec!=NULL; rec=rec->ai_next){
        sockfd = socket(rec->ai_family, rec->ai_socktype, 0);
        addr_storage.ss_family=rec->ai_family;
        if(sockfd == -1) continue;
        if (connect(sockfd, rec->ai_addr, rec->ai_addrlen) != -1){
            fprintf(stdout, "connected!\n");
            break;
        } 
    }
    if(sockfd==-1){
        fprintf(stdout, "socket()");
        exit(EXIT_FAILURE);
    }

    storagelen = sizeof(addr_storage);
    getsockname(sockfd, (struct sockaddr*)&addr_storage, &storagelen);
    fprintf(stdout, "ss_family: %d\n", addr_storage.ss_family);
    getnameinfo((struct sockaddr*)&addr_storage, storagelen, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), 10);
    fprintf(stdout, "host=%s, serv=%s\n", hbuf, sbuf);

    memset(buff, 0, 256);
    retval = read(sockfd, buff, sizeof(buff));
    sleep(1);
    fprintf(stdout, "Received server response: %s\n", buff);
    fprintf(stdout, "Closing socket (sending FIN to server)...\n");
    close(sockfd);
    fprintf(stdout, "Terminating application. After receiving FIN from server, "
            "TCP connection will go into TIME_WAIT state.\n");

    exit(EXIT_SUCCESS);
}
