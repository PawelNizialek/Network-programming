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

#define BUFF_SIZE 256
#define REC_STREAMS 2
#define IN_STREAMS 4
#define O_STREAMS 3
#define MAX_ATTEPTS 5

int main(int argc, char** argv) {

    int                     listenfd, connfd;
    int                     retval, bytes;
    struct sockaddr_in      servaddr;
    struct sockaddr destination;
    struct sctp_initmsg initmsg;
    struct sctp_status status;
    char                    buffer[BUFF_SIZE];
    struct sctp_sndrcvinfo sndrcvinfo;
    char message[BUFF_SIZE] = "message";
    destination.sa_family = AF_INET;

    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT NUMBER>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // socket()
    listenfd = socket(PF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
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
    

    for(;;)
    {
        bytes = sctp_recvmsg(listenfd, buffer, sizeof(buffer), NULL, 0, &sndrcvinfo, NULL);
        if (bytes == -1) {
            perror("recv()");
            exit(EXIT_FAILURE);
        }

        retval = write(STDOUT_FILENO, buffer, bytes);
        printf("\n");

        retval = sctp_sendmsg(listenfd, (void*)message, BUFF_SIZE, NULL, 0,
        0, 0, 0, 10, 0);
        if (retval == -1) {
            perror("sctp_sendmsg()");
            exit(EXIT_FAILURE);
        }

    }

    


    // for (;;) {

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
    // }

}
