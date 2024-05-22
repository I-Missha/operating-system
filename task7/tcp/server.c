#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h>
#include <arpa/inet.h>

#define PORT 777
#define BUFF_SIZE 2048 
#define MAX_NUM_PENDING_CONNECTIONS 3

int main() {

   int server_socket = socket(AF_INET, SOCK_STREAM, 0);
   struct sockaddr_in server_socketaddr_in, client_socketaddr_in;
   memset(&server_socketaddr_in, 0, sizeof(server_socketaddr_in));
   server_socketaddr_in.sin_port = PORT;
   server_socketaddr_in.sin_family = AF_INET;
   inet_pton(AF_INET, "127.0.0.1", &server_socketaddr_in.sin_addr);
   bzero(&(server_socketaddr_in.sin_zero), 8);
   int bind_value = bind(server_socket, 
                         (struct sockaddr*) &server_socketaddr_in, sizeof(server_socketaddr_in));
   listen(server_socket, MAX_NUM_PENDING_CONNECTIONS);
   if (bind_value == -1) {
      perror("error while binding");
      exit(1);
   }
   while(1) {
      printf("accept status\n");
      int new_cfd = accept(server_socket, (struct sockaddr*) &(client_socketaddr_in), NULL);
      if (new_cfd == -1) {
         perror("accept error");
         exit(1);
      }
      int new_cpid = fork();
      if (new_cpid == 0) {
         while (1) {
            char buff[BUFF_SIZE];
            memset(buff, 0, BUFF_SIZE);
            int read_bytes = recv(new_cfd, buff, BUFF_SIZE, 0);
            printf("recieved str: %s", buff);
            int write_bytes = write(new_cfd, buff, BUFF_SIZE);
            if (!strcmp(buff, "exit\n")) {
               exit(0);
            }
         }
      }
   }   
   return 0;
}
