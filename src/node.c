#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>

#define check(cond) do {	 \
    int rv = (cond);		 \
    if (rv < 0) {		 \
      perror("Error: " #cond);	 \
      exit(-1);			 \
    }				 \
  } while (0)

void safe_send(int sock, unsigned char *buf, size_t len) {
  while (len > 0) {
    int sent;
    check(sent = send(sock, buf, len, 0));
    buf += sent;
    len -= sent;
  }
}

size_t safe_recv(int sock, unsigned char *buf, size_t len) {
  size_t read_tot = 0;
  while (len > 0) {
    int read;
    check(read = recv(sock, buf, len, 0));
    buf += read;
    len -= read;
    read_tot += len;
  }
  return read_tot;
}

void *receive_thread(void *data) {
  int sock = (int)(long) data;
  unsigned char buf[1024];

  while (1) {
    int rv = recv(sock, buf, 1, 0);
    check(rv);
    if (rv == 0)
      break;
    printf("Echoing: %02x\n", buf[0]);
    safe_send(sock, buf, 1);
  }
  check(close(sock));
  return 0;
}

int main(int argc, char *argv[]) {
  int welcomeSocket, newSocket;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);

  int i = 0;
  setsockopt(welcomeSocket, IPPROTO_TCP, SO_REUSEADDR, (void *)&i, sizeof(i));

  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(7891);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Bind the address struct to the socket ----*/
  check(bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)));

  /*---- Listen on the socket, with 5 max connection requests queued ----*/
  check(listen(welcomeSocket,5));
  printf("Listening\n");

  while (1) {
    /*---- Accept call creates a new socket for the incoming connection ----*/
    addr_size = sizeof serverStorage;
    newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
    pthread_t child;
    check(pthread_create(&child, NULL, receive_thread, (void *)(long) newSocket));
    check(pthread_detach(child));
  }

  return 0;
}
