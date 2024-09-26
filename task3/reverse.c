#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

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

void printErr(char* funcName) {
   fprintf(stderr, "execute func %s with error %s\n", funcName, strerror(errno));
}

void revCopyFile(int src, int dest, int src_size) {
   unsigned char buff[2048];
   unsigned char buffRev[2048];
   lseek(src, src_size - src_size % 2048, SEEK_SET);
   int counter = 0;
   while (counter != src_size) {
      int temp = read(src, buff, 2048 * sizeof(unsigned char));
      counter += temp;
      for (int i = 0; i < counter; i++) {
         buffRev[i] = buff[counter - i - 1];
      }
      int counterBytes = 0; 
      while (counterBytes != temp) {
         int wrBytes = write(dest, buffRev, temp);
         if (wrBytes == -1) {
            printErr("write");
            return;
         }
         counterBytes += wrBytes;
      }

      if (counter + 2048 > src_size) {
         break;
      }
      lseek(src, src_size - (counter + 2048), SEEK_SET);
   }
}


void reverseDir(char path[]) {
   DIR* dir;
   struct dirent* de;

   dir = opendir(path);

   if (dir == NULL) {
      printErr("opendir"); 
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
      if (de->d_type != DT_REG && de->d_type != DT_DIR) {
         de = readdir(dir);
         continue;
      } 
      char filePath[512];
      constructFilePath(path, de->d_name, filePath);
      if (de->d_type == DT_DIR) {
         if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..")) {
            reverseDir(filePath);
         }
         de = readdir(dir);
         continue;
      } 
      struct stat st;
      stat(filePath, &st);
      int read = open(filePath, O_RDWR);
      if (read == -1) {
         fprintf(stderr, "Cant open file: %s\n", de->d_name);
         de = readdir(dir);
         continue;
      }
      char revFilePath[512];
      char revFileName[512];
      reverseString(de->d_name, revFileName);
      constructFilePath(revDirPath, revFileName, revFilePath);
      int write = open(revFilePath, O_CREAT | O_RDWR | O_TRUNC , st.st_mode);
      if (write == -1){
         fprintf(stderr, "Cant open/create file with path: %s\n", revDirPath);
         break;
      }
      revCopyFile(read, write, st.st_size);
      close(read);
      close(write);
      de = readdir(dir);
   }
}

int main(int argc, char** argv) {
   reverseDir(argv[1]);
   return 0;
}

