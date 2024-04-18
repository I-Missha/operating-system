#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <errno.h>

void printErr(char* funcName) {
   fprintf(stderr, "executed %s\n with error msg: %s\n", funcName, strerror(errno));
}

void myMkdir(char* dirPath) {
   if (mkdir(dirPath, 0777) == -1) {
      fprintf(stderr, "cant create dir by path: %s", dirPath);
      printErr("mkdir");
   }
}

void myLs(char* dirPath) {
   DIR* dir;
   struct dirent* de;

   dir = opendir(dirPath);

   if (dir == NULL) {
      printErr("opendir");
      return;
   }

   de = readdir(dir);
   while (de != NULL) {
      printf("%s\n", de->d_name);
      de = readdir(dir);
   }
}

void myRmDir(char* filePath) {
   if (rmdir(filePath) == -1) {
      fprintf(stderr, "cant remove folder\n");
      printErr("rmdir");
   }
}

void myTouch(char* filePath) {
   if (creat(filePath, 0666) == -1) {
      fprintf(stderr, "cant creat file\n");
      printErr("creat");
   }
}

int secureWrite(int numBytesToWr, int fd, char* buff) {
   int sumWrBytes = 0;
   while (sumWrBytes != numBytesToWr) {
      int wrBytes = write(fd, buff, numBytesToWr);
      if (wrBytes == -1) {
         printErr("write");
         return 0; 
      } 
      sumWrBytes += wrBytes;

   }
   return 1;
}

void myCat(char* filePath) {
   char buff[2048];
   int src = open(filePath, O_RDWR);
   if (src == -1) {
      fprintf(stderr, "Cant open file: %s\n", filePath);
      printErr("open"); 
      return;
   }

   int counter = 1; 
   while (counter > 0) {
      counter = read(src, buff, 2048 * sizeof(char));
      if (counter == -1) {
         fprintf(stderr, "Cant read from file: %s\n", filePath);
         printErr("read");
         break;
      }
      if (!secureWrite(counter, 1, buff)) {
         break;
      }
   }
   printf("\n");
   close(src);
}

void myRm(char* filePath) {
   if (unlink(filePath) == -1) {
      fprintf(stderr, "Cant delete file: %s\n", filePath);
      printErr("unlink");
   }
}

void mySymlink(char* target, char* linkPath) {
   if (symlink(target, linkPath) == -1) {
      fprintf(stderr, "cant creat symlink\ntarget: %s\nlinkpath: %s\n", target, linkPath);
      printErr("symlink"); 
   }
}

void myReadLink(char* filePath) {
   char buff[2048];
   int counter = readlink(filePath, buff, 2048 * sizeof(char));
   if (counter == -1) {
      fprintf(stderr, "cant readlink by path: %s\n", filePath);
      printErr("readlink");
      return;
   }
   buff[counter] = '\n';   
   if (!secureWrite(counter + 1, 1, buff)) {
      return;
   }
}

void myLink(char* oldPath, char* newPath) {
   if (link(oldPath, newPath) == -1) {
      fprintf(stderr, "cant creat hard link\noldPath: %s\nnewPath: %s\n", oldPath, newPath);
      printErr("link");
   }
}

void myReadFileByLink(char* filePath) {
   char buff[2048];
   int counter = 1;
   counter = readlink(filePath, buff, 2048 * sizeof(unsigned char));
   if (counter == -1) {
      printf("Cant read file by link: %s\n", filePath);
      return;
   }
   myCat(filePath);
}

void myStat(char* filePath) {
   struct stat st;
   if (stat(filePath, &st) == -1) {
      fprintf(stderr, "Cant stat file: %s\n", filePath);
      printErr("stat");
      return;
   }
   printf("file mode: 0%o\nnumber of hardlinks: %ld\n", st.st_mode&0777, st.st_nlink);
}

void myChmod(char* filePath, mode_t mode) {
   if (chmod(filePath, mode) == -1) {
      fprintf(stderr, "Cant chmod file: %s\n", filePath);
      printErr("chmod"); 
   }
}

int main(int argc, char** argv) {
   if (argc != 3 && argc != 2) {
      fprintf(stderr, "wrong argc\n");
      return 0;
   }

   if (argc == 3) {
      if (!strcmp(argv[0], "./symlink")) {
         mySymlink(argv[1], argv[2]);
         return 0;
      } else if (!strcmp(argv[0], "./link")) {
         myLink(argv[1], argv[2]);
         return 0;
      } else if (!strcmp(argv[0], "./chmod")) {
         myChmod(argv[1], strtol(argv[2], NULL, 8));
         return 0;
      }  
   }

   if (!strcmp(argv[0], "./mkdir")) {
      myMkdir(argv[1]); 
   } else if (!strcmp(argv[0], "./ls")) {
      myLs(argv[1]); 
   } else if (!strcmp(argv[0], "./rmdir")) {
      myRmDir(argv[1]);
   } else if (!strcmp(argv[0], "./touch")) {
      myTouch(argv[1]);
   } else if (!strcmp(argv[0], "./cat")) {
      myCat(argv[1]);
   } else if (!strcmp(argv[0], "./rm")) {
      myRm(argv[1]);
   } else if (!strcmp(argv[0], "./readlink")) {
      myReadLink(argv[1]);
   } else if (!strcmp(argv[0], "./readfbl")) {
      myReadFileByLink(argv[0]);
   } else if (!strcmp(argv[0], "./unsymunlink")) {
      myRm(argv[1]);
   } else if (!strcmp(argv[0], "./unhardlink")) {
      myRm(argv[1]);
   } else if (!strcmp(argv[0], "./stat")) {
      myStat(argv[1]);
   }

   return 0;
}


