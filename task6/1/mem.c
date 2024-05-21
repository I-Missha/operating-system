#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <limits.h>

#define PAGE_SIZE 4096


int main(int argc, char** argv) {
   
   unsigned int* ptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
   
   int child_proc = fork();
   if (child_proc == 0) {
      unsigned int num = 0;
      while (1) {
         for (int i = 0; i < PAGE_SIZE / sizeof(unsigned int); i++) {
            ptr[i] = num;
            if (num == UINT_MAX) {
               break;
            }
            num++;
            sleep(3);
         }
         if (num == UINT_MAX) {
            break;
         }
      }

   } else {
      unsigned int expected_num = ptr[0];
      while(1) {
         for (int i = 0; i < PAGE_SIZE / sizeof(unsigned int); i++) {
            if (expected_num == UINT_MAX) {
               break;
            }
            if (expected_num != ptr[i]) {
               printf("error in sequence\n");
               fflush(stdout);
            }
            expected_num++;
            sleep(3);
         }
         if (expected_num == UINT_MAX) {
            break;
         }
      }
   }

   munmap(ptr, 4096);
   return 0;
}
