#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h>
#include <errno.h>
#include <net/if.h>

int main(int argc, char** argv) {

    if (argc != 4) {
        fprintf(stderr, "Invocation: %s <IPv4 ADDRESS> <PORT> <INTERFACE NAME>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    int retval;
    struct sockaddr_in6 remote_addr;
    socklen_t addr_len;
    char buff[256];

    sockfd = socket(PF_INET6, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    memset(&remote_addr, 0, sizeof(remote_addr));

    remote_addr.sin6_family = AF_INET6;

    retval = inet_pton(AF_INET6, argv[1], &remote_addr.sin6_addr);
    if (retval == 0) {
        fprintf(stderr, "inet_pton(): invalid network address!\n");
        exit(EXIT_FAILURE);
    } else if (retval == -1) {
        perror("inet_pton()");
        exit(EXIT_FAILURE);
    }

    remote_addr.sin6_port = htons(atoi(argv[2]));
    remote_addr.sin6_scope_id = htons(if_nametoindex(argv[3]));
    addr_len = sizeof(remote_addr);


    if (connect(sockfd, (struct sockaddr*) &remote_addr, addr_len) == -1) {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "After three-way handshake. Waiting for server response...\n");

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
