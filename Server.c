#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <ifaddrs.h>
#include <time.h> //get time since 1970 1.1

#include <signal.h>
#include <sys/stat.h>

#include <pthread.h> //pthread_create
#include <sys/utsname.h> //call  uname() to get local os info
#define BUFSIZE 112
typedef struct
{
     int    *time;
     char   *valid;
} GET_LOCAL_TIME;
typedef struct
{
     char   *OS;
     char   *valid;
} GET_LOCAL_OS;
void byte_processing(char* byte);
void byteToInt(unsigned char* bytes);
void GetLocalTime(GET_LOCAL_TIME *ds);
void GetLocalOS(GET_LOCAL_OS *os);
void execute(int childfd);
void *CmdProcessor (void *args);
int receive_one_byte(int client_socket, char *cur_char);
int receiveFully(int client_socket, char *buffer, int length);


void error(char *msg) {
    perror(msg);
    exit(1);
}

char CmdId[100]; /* message buffer */
unsigned char CmdLength[4]; /* message buffer */
char CmdBuf[8]; /* message buffer */
uint32_t myInt1;
GET_LOCAL_TIME now_time;
GET_LOCAL_OS os_info;
 char buf[5];
 char buf1[21];
 char msg[BUFSIZE];
  long n; /* message byte size */

int main(int argc, char *argv[]) {
    pthread_t thread1;
    int svrSock; /* parent socket */
    int childfd; /* child socket */
    unsigned short svrPort = atoi("7777");; /* port to listen on 7777*/
    unsigned int clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    char *hostaddrp; /* dotted decimal host addr string */
    char buf[BUFSIZE]; /* message buffer */

    printf("%d\n",svrPort);
    memset (&serveraddr, 0, sizeof (serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serveraddr.sin_port = htons(svrPort);

    svrSock = socket(AF_INET, SOCK_STREAM, 0);
    if (svrSock < 0)
        error("ERROR opening socket");
    if (bind(svrSock, (struct sockaddr *) &serveraddr,sizeof(serveraddr)) < 0)
        error("ERROR on binding");
    if (listen(svrSock, 5) < 0) /* allow 5 requests to queue up */
        error("ERROR on listen");

    clientlen = sizeof(clientaddr);
    while (1) {

        printf("listen to client connection\n");
        childfd = accept(svrSock, (struct sockaddr *) &clientaddr, &clientlen);
        if (childfd < 0)
            error("ERROR on accept");
        printf("1 client already connected\n");
        int *pointer = &childfd;
        if (pthread_create(&thread1, NULL,CmdProcessor, pointer)!=0)  //create thread to call CmdAgent.
          printf("error\n");
        /*
         * gethostbyaddr: determine who sent the message
         */
        // hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
        //                       sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        // if (hostp == NULL)
        //     error("ERROR on gethostbyaddr");
        // hostaddrp = inet_ntoa(clientaddr.sin_addr);
        // if (hostaddrp == NULL)
        //     error("ERROR on inet_ntoa\n");
        // printf("server established connection with %s (%s)\n",
        //        hostp->h_name, hostaddrp);

        /*
         * read: read input string from the client
         */
    }
    //pthread_join(thread1, NULL);
    return 0;
}

void byte_processing(char* byte){
    bzero(CmdId, 100);
    bzero(CmdLength, 4);
    bzero(CmdBuf, 8);
    for (size_t i = 0; i < 100; i++) { //get CmdId
        CmdId[i] = byte[i];
    } //printf("the CmdId : %s \n", CmdId); test okay!
    for (size_t i = 0; i < 100; i++) {
        if (CmdId[i]== 0) {  //CmdId 97 98 101 0 0 0 0  begin with 0 means null.
            CmdId[i] = '\0';
            break;
        }
    }
    int j = 0;
    for (size_t i = 100; i < 104; i++) { //get CmdLength
        CmdLength[j] = byte[i];
        j++;
    } j = 0;
    for (size_t i = 104; i < sizeof(byte); i++) { //get CmdBuf    temporarily unuseful
        CmdBuf[j] = byte[i];
        j++;
    }
    byteToInt(CmdLength);
    //printf("CmdLength : %d\n", myInt1); test okay!

}

void byteToInt(unsigned char* bytes){ //LITTLE_ENDIAN
  //extern uint8_t *bytes;
  myInt1 = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24);
}

void GetLocalTime(GET_LOCAL_TIME *ds){
    printf("CmdId: 'GetLocalTime' \n");
    long time_int = (long)time(NULL); printf("local time: %ld\n",time_int);
    buf[0] = (time_int >> 24) & 0xFF;
    buf[1] = (time_int >> 16) & 0xFF;
    buf[2] = (time_int >> 8) & 0xFF;
    buf[3] = time_int & 0xFF;
    buf[4] = 'T';
    // printf("%d %d %d %d %c %ld\n", (unsigned char)buf[0],
    //                     (unsigned char)buf[1],
    //                     (unsigned char)buf[2],
    //                     (unsigned char)buf[3],
    //                     (unsigned char)buf[4],
    //                      sizeof(buf)  );
    // ds->time = &buf;
    // ds->valid = &buf[4];
    // *(ds->time) = time_int;
    // *(ds->valid) = 'T';
}

void GetLocalOS(GET_LOCAL_OS *os){
    printf("CmdId: 'GetLocalOS' \n");
    bzero(buf1, 21);
    os->OS = buf1;
    os->valid = &buf1[20];
    strncpy(os->OS,"MacOS 10.12.2",20); //printf("---%c\n", buf1[0]);
    *(os->valid) = 'T';
}

void execute(int childfd){
  switch (strcmp(CmdId, "GetLocalTime")) {
    case 0:
        GetLocalTime(&now_time);
        n = write(childfd, buf, sizeof(buf));
        if (n < 0)
            error("ERROR writing to socket");
        break;
    default:
        GetLocalOS(&os_info);
        n = write(childfd, buf1, sizeof(buf1));
        if (n < 0)
            error("ERROR writing to socket");
  }
}

void *CmdProcessor (void *args) {
  int childfd = *((int *)args);
  bzero(msg, BUFSIZE);
  n = read(childfd, msg, BUFSIZE);
  if (n < 0) error("ERROR reading from socket");
  printf("server received %ld bytes\n", n);
  byte_processing(msg);
  execute(childfd);
  close(childfd);
  return 0;
}

int receive_one_byte(int client_socket, char *cur_char)
{
    ssize_t bytes_received = 0;
	while (bytes_received != 1)
	{
		bytes_received = recv(client_socket, cur_char, 1, 0);
	}

	return 1;
}

int receiveFully(int client_socket, char *buffer, int length)
{
	char *cur_char = buffer;
	ssize_t bytes_received = 0;
	while (bytes_received != length)
	{
	    receive_one_byte(client_socket, cur_char);
	    cur_char++;
	    bytes_received++;
	}

	return 1;
}
