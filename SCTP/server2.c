#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/sctp.h>
#include <libgen.h>
#include <time.h>
#include <assert.h>

#define BUFF_SIZE 256

int main(int argc, char** argv) {

    int                     listenfd, connfd;
    int                     retval, bytes;
    struct sockaddr_in      servaddr;
    char                    buffer[BUFF_SIZE];
    char message[BUFF_SIZE] = "message";

    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT NUMBER>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // socket()
    listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (listenfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }


    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family             =       AF_INET;
    servaddr.sin_port               =       htons(atoi(argv[1]));
    servaddr.sin_addr.s_addr        =       htonl(INADDR_ANY);

    // bind()
    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    // listen()
    if (listen(listenfd, 5) == -1) {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        //  accept()
        connfd = accept(listenfd, NULL, 0);
        if (connfd == -1) {
            perror("accept()");
            exit(EXIT_FAILURE);
        }

        time_t t = time(NULL);
        struct tm* tm = localtime(&t);
        // char [BUFF_SIZE];
        strftime(message, sizeof(message), "[SERVER] %I:%M%p", tm);

        retval = sctp_sendmsg(connfd, (void*)message, BUFF_SIZE, NULL, 0, 0,
        0, 0, 10, 0);

        if (retval == -1) {
            perror("sctp_sendmsg()");
            exit(EXIT_FAILURE);
        }
        strftime(message, sizeof(message), "[SERVER] %x", tm);

        retval = sctp_sendmsg(connfd, (void*)message, BUFF_SIZE, NULL, 0, 0,
        0, 1, 10, 0);

        if (retval == -1) {
            perror("sctp_sendmsg()");
            exit(EXIT_FAILURE);
        }

        // bytes = recv(connfd, (void*)buffer, BUFF_SIZE, 0);
        // if (bytes == -1) {
        //     perror("recv() failed");
        //     exit(EXIT_FAILURE);
        // } else if (bytes == 0) {
        //     close(connfd);
        //     close(listenfd);
        //     exit(EXIT_SUCCESS);
        // }

        // retval = write(STDOUT_FILENO, buffer, bytes);

        // if (send(connfd, buffer, bytes, 0) == -1) {
        //     perror("send()");
        //     exit(EXIT_FAILURE);
        // }
        
    }

}
