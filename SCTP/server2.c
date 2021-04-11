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
#define REC_STREAMS 2
#define IN_STREAMS 2
#define O_STREAMS 2
#define MAX_ATTEPTS 5

int main(int argc, char** argv) {

    int                     listenfd, connfd;
    int                     retval, bytes;
    struct sockaddr_in      servaddr;
    char                    buffer[BUFF_SIZE];
    char message[BUFF_SIZE] = "message";
    struct sctp_initmsg initmsg;

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

    memset(&initmsg, 0, sizeof(struct sctp_initmsg));
    initmsg.sinit_max_instreams = IN_STREAMS;
    initmsg.sinit_num_ostreams = O_STREAMS;
    initmsg.sinit_max_attempts = MAX_ATTEPTS;

    retval = setsockopt(listenfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(struct sctp_initmsg));

    if(retval<0){
        fprintf(stderr, "setsockopt()\n");
        exit(EXIT_FAILURE);
    }

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
    }
    close(connfd);
    close(listenfd);
}
