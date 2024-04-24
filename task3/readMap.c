#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define SIZE 2048
#define ull unsigned long long


void printErr(char* funcName, int lineNum) {
   fprintf(stderr, "executed func %s\nfunc numb: %d\nwith an error msg: %s\n", funcName, lineNum, strerror(errno));
}

// find the '\n' and add to offset 
int recalcOffset(int currOff, int fd, char* buff, int readCounter) {
   int ind = 0;
   int offsetCounter = 0;
   while (1) {
      if (readCounter == -1) {
         printErr("read", 0);
         return -1;
      }
      
      while (ind < readCounter && buff[ind] != '\n') {
         ind++;
      }
      
      if (buff[ind] == '\n') {
         return currOff + ind + offsetCounter + 1;
      }

      offsetCounter += ind;
      ind = 0;

      readCounter = read(fd, buff, 2048);
   }
}

ull convertAdress(char* buff) {
   ull res;
   sscanf(buff, "%llx", &res);
   return res;
}

int readAddress(ull address, int fd) {
   // page entry size is 8
   if (fd == -1) {
      printErr("open", 1);
      return -1;
   }

   ull pageSize = getpagesize();
   ull offsetInPagemap = address / pageSize * 8;
   if (lseek(fd, offsetInPagemap, SEEK_SET) == -1) {
      printErr("lseek", 2);
      return -1;
   }

   ull res;
   if (read(fd, &res, 8) == -1) {
      printErr("read", 3);
   }

   printf("result: %lld\n\
         some info from this res:\n\
         page frame num: %p\n\
         swap type: %lld\n\
         swap offset: %lld\n\
         soft-dirty: %lld\n\
         exclusively mapped: %lld\n\
         file-page/shared-anon: %lld\n\
         swapped: %lld\n\
         is in mem: %lld\n",
         res,
         (void *)(res & 0x7fffffffffffff), 
         res & 15, 
         res & (0x7fffffffffffff - 15), 
         (res >> 55) & 1,
         (res >> 56) & 1,
         (res >> 61) & 1, 
         (res >> 62) & 1, 
         (res >> 63) & 1);
   return 1;
}

void readPagemapByAddresses(char* pid) {
   char mapsPath[2048];
   sprintf(mapsPath, "/proc/%s/maps", pid);
   int fd = open(mapsPath, O_RDONLY);
   if (fd == -1) {
      printErr("open", 4);
      return;
   }
   
   char pagemapPath[2048];
   sprintf(pagemapPath, "/proc/%s/pagemap", pid);
   int fd_pagemap = open(pagemapPath, O_RDONLY);
   if (fd_pagemap == -1) {
      printErr("open", 7);
      return;
   }  

   if (lseek(fd, 0, SEEK_SET) == -1) {
      printErr("lseek", 5);
      close(fd);
      return;
   }

   int readBytes = 0;
   int offset = 0;
   char buff[2048];
   while(lseek(fd, offset,  SEEK_SET) != -1) {
      readBytes = read(fd, buff, 2048 * sizeof(char));
      if (readBytes == 0) {
         break;
      }
      char address[128]; 
      int ind = 0;
      while (buff[ind] != '-') {
         address[ind] = buff[ind];
         ind++;
      }
      address[ind] = '\000'; 
      if (readBytes == -1) {
         printErr("read", 6);
         break;
      }
      ull adr = convertAdress(address);
      readAddress(adr, fd_pagemap);

      offset = recalcOffset(offset, fd, buff, readBytes);
      if (offset == -1) {
         break;
      }
      memset(buff, 0, 2048);
   }
} 

int main(int argc, char** args) {
   readPagemapByAddresses(args[1]);
   return 0;
}
