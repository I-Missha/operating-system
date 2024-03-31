#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <stdlib.h>

void reverseString(char from[], char to[]) {
   int len = strlen(from);
   for (int i = 0; i < len; i++) {
      to[len - 1 - i] = from[i];  
   }
   to[len] = '\0';
}


void constructFilePath(char dirName[256], char name[256], char filePath[512]) {
   strcpy(filePath, dirName);
   filePath[strlen(dirName)] = '/';
   strcpy(filePath + strlen(dirName) + 1, name);
   filePath[strlen(dirName) + strlen(name) + 1] = '\0';
}

void revCopyFile(int src, int dest) {
   unsigned char* buff = (unsigned char*)malloc(2048 * sizeof(unsigned char));
   unsigned char* buffRev = (unsigned char*)malloc(2048 * sizeof(unsigned char));
   int counter = read(src, buff, 2048 * sizeof(unsigned char));
   while (counter > 0) {
      for (int i = 0; i < counter; i++) {
         buffRev[i] = buff[counter - i - 1];
      }
      write(dest, buffRev, counter);
      counter = read(src, buff, 2048 * sizeof(unsigned char));
   }
   free(buffRev);
   free(buff);
}

void reverseDir(char path[]) {
   DIR* dir;
   struct dirent* de;

   dir = opendir(path);

   if (dir == NULL) {
      printf("coudn't open dir");
      return;
   }
   char revDirName[256];
   char ch;
   int len = strlen(path) - 1;
   int counter = 0;
   while (1) {
      ch = path[len - counter];
      if (ch == '/') {
         revDirName[counter] = '\0';
         break;
      }
      revDirName[counter] = ch;
      counter++;
   }

   mkdir(revDirName, 0777);
   char currDirPath[512];
   getcwd(currDirPath, 512);
   char revDirPath[512];
   constructFilePath(currDirPath, revDirName, revDirPath);
   de = readdir(dir);
   while (de != NULL) {
      if (de->d_type != DT_REG) {
         de = readdir(dir);
         continue;
      }
      char filePath[512];
      constructFilePath(path, de->d_name, filePath);
      struct stat st;
      stat(filePath, &st);
      int read = open(filePath, O_RDWR);
      if (read == -1) {
         printf("Cant open file: %s\n", de->d_name);
         de = readdir(dir);
         continue;
      }
      char revFilePath[512];
      char revFileName[512];
      reverseString(de->d_name, revFileName);
      constructFilePath(revDirPath, revFileName, revFilePath);
      int write = open(revFilePath, O_CREAT | O_RDWR | O_TRUNC , st.st_mode);
      if (write == -1){
         break;
      }
      revCopyFile(read, write);
      close(read);
      close(write);
      de = readdir(dir);
   }

}

int main(int argc, char** argv) {
   reverseDir(argv[1]);

   //   int read = open("/file1", O_RDONLY);
   // if (read == -1) {
   //  printf("here\n");
   //}
   return 0;
} 
