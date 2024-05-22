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

   int client_socket;
   struct sockaddr_in server_sockaddr_in;
   client_socket = socket(AF_INET, SOCK_DGRAM, 0);
   memset(&server_sockaddr_in, 0, sizeof(struct sockaddr_in));
   inet_pton(AF_INET, "127.0.0.1", &server_sockaddr_in.sin_addr);
   server_sockaddr_in.sin_port = htons(PORT);
   server_sockaddr_in.sin_family = AF_INET;

   while (1) {
      char buff[BUFF_SIZE];
      memset(buff, 0, BUFF_SIZE);  
      char* temp = fgets(buff, BUFF_SIZE, stdin);
      int ind = 0;
      while(temp[ind] != '\n') {
         ind++;
      }
      ind++;
      buff[ind] = '\0';
      sendto(client_socket, buff, BUFF_SIZE, 0, 
             (struct sockaddr*) &server_sockaddr_in, sizeof(server_sockaddr_in));
      server_sockaddr_in.sin_family = AF_INET;
      recvfrom(client_socket, buff, BUFF_SIZE, 0, (struct sockaddr*) &server_sockaddr_in, NULL);
      printf("server sent msg: %s", buff);
   }
   return 0;
}

