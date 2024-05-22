#include <limits.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdbool.h>

#define PAGE_SIZE 4096



int main(int argc, char** argv) {
   int pfd[2];
   pipe(pfd);

   int child_pid = fork();

   if (child_pid == 0) {
      unsigned int num = 0;
      while (1) {
         for (int i = 0; i < PAGE_SIZE / sizeof(unsigned int); i++) {
            write(pfd[1], &num, sizeof(unsigned int));
            num++;
            if (num == UINT_MAX) {
               break;
            }
         }
         if (num == UINT_MAX) {
            break;
         }
      }
   } else {
      sleep(1);
      unsigned int curr_num, prev_num = 0;
      while (1) {
         for (int i = 0; i < PAGE_SIZE / sizeof(unsigned int); i++) {
            read(pfd[0], &curr_num, sizeof(unsigned int));
            if (curr_num != prev_num + 1) {
               printf("error in seq: curr_num %d, prev_num %d\n", curr_num, prev_num);
            }
//            printf("curr num %d\n", curr_num);
            prev_num = curr_num;
            if (curr_num + 1 == UINT_MAX) {
               break;
            }
         }
            printf("curr num %d\n", curr_num);
         if (UINT_MAX == curr_num + 1) {
            break;
         }
      }
   }
   close(pfd[0]);
   close(pfd[1]);
   return 0;
}
