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

    int                     sockfd;
    int                     retval, bytes;
    char                    *retptr;
    struct addrinfo hints, *result;
    char                    buff[BUFF_SIZE];
    struct sctp_initmsg initmsg;
    struct sctp_status status;
    struct sctp_sndrcvinfo sndrcvinfo;
    socklen_t optlen = sizeof(struct sctp_status);
    int i = 0;

    if (argc != 3) {
        fprintf(stderr, "Invocation: %s <IP ADDRESS> <PORT NUMBER>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family         = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype       = SOCK_STREAM;
    hints.ai_flags          = 0;
    hints.ai_protocol       = 0;

    retval = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (retval != 0) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(retval));
        exit(EXIT_FAILURE);
    }

    if (result == NULL) {
        fprintf(stderr, "Could not connect!\n");
        exit(EXIT_FAILURE);
    }

    // socket()
    sockfd = socket(result->ai_family, result->ai_socktype, IPPROTO_SCTP);
    if (sockfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    memset(&initmsg, 0, sizeof(struct sctp_initmsg));
    initmsg.sinit_max_instreams = IN_STREAMS;
    initmsg.sinit_num_ostreams = O_STREAMS;
    initmsg.sinit_max_attempts = MAX_ATTEPTS;

    retval = setsockopt(sockfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(struct sctp_initmsg));

    if(retval<0){
        fprintf(stderr, "setsockopt()\n");
        exit(EXIT_FAILURE);
    }

    // connect()
    if (connect(sockfd, result->ai_addr, result->ai_addrlen) == -1) {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    retval = getsockopt(sockfd, IPPROTO_SCTP, SCTP_STATUS, &status, &optlen);

    if(retval<0){
        fprintf(stderr, "getsockopt()\n");
        exit(EXIT_FAILURE);
    }

    printf("[CLIENT] Connected; ID: %u\n", status.sstat_assoc_id);
    printf("[CLIENT] Status: %u\n", status.sstat_state);
    printf("[CLIENT] Input streams: %u\n", status.sstat_instrms);
    printf("[CLIENT] Output streams: %u\n", status.sstat_outstrms);

    freeaddrinfo(result);

    while (i < REC_STREAMS) {
        bytes = sctp_recvmsg(sockfd, buff, sizeof(buff), NULL, 0, &sndrcvinfo, NULL);
        if (bytes == -1) {
            perror("recv()");
            exit(EXIT_FAILURE);
        }
        i++;
        retval = write(STDOUT_FILENO, buff, bytes);
        printf("\n");
    }
    close(sockfd);
    exit(EXIT_SUCCESS);
}
