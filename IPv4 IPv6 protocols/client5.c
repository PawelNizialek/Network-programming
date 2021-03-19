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
    struct sockaddr_in remote_addr;
    struct addrinfo* addr_info;
    socklen_t addr_len;
    char buff[256];

    struct addrinfo tmp;
    struct addrinfo *res;
    struct addrinfo *rec;
    struct sockaddr_storage addr_storage;
    int sock_name=0;
    int sock_addr=0;
    memset(&tmp, 0, sizeof(struct addrinfo));
    tmp.ai_family = AF_INET;
    tmp.ai_socktype = SOCK_STREAM;
    tmp.ai_protocol = IPPROTO_TCP;
    socklen_t storagelen=0;

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
    sock_name = getsockname(sockfd, (struct sockaddr*)&addr_storage, &storagelen);
    fprintf(stdout,"%d", sock_name);

    memset(buff, 0, 256);

    retval = read(sockfd, buff, sizeof(buff));
    sleep(1);
    fprintf(stdout, "Received server response: %s\n", buff);
    fprintf(stdout, "Closing socket (sending FIN to server)...\n");
    close(sockfd);
    fprintf(stdout, "Terminating application. After receiving FIN from server, "
            "TCP connection will go into TIME_WAIT state.\n");

    exit(EXIT_SUCCESS);




    // sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // if (sockfd == -1) {
    //     perror("socket()");
    //     exit(EXIT_FAILURE);
    // }
    // fprintf(stdout, "After three-way handshake. Waiting for server response...\n");

    // memset(&remote_addr, 0, sizeof(remote_addr));

    // remote_addr.sin_family = AF_INET;

    // // getaddrinfo(argv[1], NULL, NULL, &addr_info);
    // fprintf(stdout, "After three-way handshake. Waiting for server response...\n");
    // remote_addr = addr_info;

    
    // retval = inet_pton(AF_INET, argv[1], &remote_addr.sin_addr);
    // if (retval == 0) {
    //     fprintf(stderr, "inet_pton(): invalid network address!\n");
    //     exit(EXIT_FAILURE);
    // } else if (retval == -1) {
    //     perror("inet_pton()");
    //     exit(EXIT_FAILURE);
    // }

    // remote_addr.sin_port = htons(atoi(argv[2]));
    // addr_len = sizeof(remote_addr);

    // fprintf(stdout, "After three-way handshake. Waiting for server response...\n");

    // if (connect(sockfd, (struct sockaddr*) &remote_addr, addr_len) == -1) {
    //     perror("connect()");
    //     exit(EXIT_FAILURE);
    // }

    // fprintf(stdout, "After three-way handshake. Waiting for server response...\n");

    // memset(buff, 0, 256);

    // retval = read(sockfd, buff, sizeof(buff));
    // sleep(1);
    // fprintf(stdout, "Received server response: %s\n", buff);
    // fprintf(stdout, "Closing socket (sending FIN to server)...\n");
    // close(sockfd);
    // fprintf(stdout, "Terminating application. After receiving FIN from server, "
    //         "TCP connection will go into TIME_WAIT state.\n");

    // exit(EXIT_SUCCESS);
}
