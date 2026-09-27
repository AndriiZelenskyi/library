#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("socket");
    return -1;
  }
  printf("Connection created! File descriptor: %d\n", server_fd);

  struct sockaddr_in address = {0};

  address.sin_family = AF_INET;
  address.sin_port = htons(8080);
  address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
    perror("bind");
    close(server_fd);
    return -1;
  }

  if (listen(server_fd, 8) == -1) {
    perror("listen");
    close(server_fd);
    return -1;
  }

  printf("Waiting for the client to serve.\n\n");
  int client_fd = accept(server_fd, NULL, NULL);

  printf("Started reading content of the client with fd: %d\n\n", client_fd);
  char buffer[1024];
  for (;;) {
    ssize_t received = read(client_fd, buffer, sizeof(buffer));

    if (received > 0) {
      fwrite(buffer, 1, (size_t)received, stdout);
      fflush(stdout);
    }

    if (received == -1) {
      perror("received");
      close(client_fd);
      break;
    } else if (received == 0) {
      printf("Client disconnected!\n\n");
      break;
    }
  }

  printf("\n\nFinished serving client with fd: %d.\n\n", client_fd);
  close(client_fd);

  printf("Closing the server because program finished!\n");
  close(server_fd);
  return 0;
}
