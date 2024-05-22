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

int main() {

   int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
   struct sockaddr_in server_socketaddr_in, client_socketaddr_in;
   memset(&server_socketaddr_in, 0, sizeof(server_socketaddr_in));
   server_socketaddr_in.sin_port = PORT;
   server_socketaddr_in.sin_family = AF_INET;
   inet_pton(AF_INET, "127.0.0.1", &server_socketaddr_in.sin_addr);
   bzero(&(server_socketaddr_in.sin_zero), 8);

   int bind_value = bind(server_socket, 
                         (struct sockaddr*) &server_socketaddr_in, sizeof(server_socketaddr_in));


   if (bind_value == -1) {
      perror("error while binding");
      exit(1);
   }
   while(1) {
      char buff[BUFF_SIZE];
      printf("in the state: listening\n");
      unsigned int addrlen;
      recvfrom(server_socket, buff, BUFF_SIZE, 0, 
               (struct sockaddr *) &client_socketaddr_in, &addrlen);
      printf("message that we recieved: %s", buff);
      client_socketaddr_in.sin_family = AF_INET; 
      sendto(server_socket, buff, BUFF_SIZE, 0, 
             (struct sockaddr *) &client_socketaddr_in, sizeof(client_socketaddr_in));
   }   
   return 0;
}
