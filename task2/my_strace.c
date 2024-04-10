#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <asm/unistd_64.h>
#include "syscall_table.h"
#include <linux/ptrace.h>
#include <string.h>

void printStr(pid_t pid, long ptr, int numBytes, char* buff) {
   int counter = 0;
   while (counter < numBytes) {
      char ch = ptrace(PTRACE_PEEKDATA, pid, ptr + (counter*sizeof(char)) , NULL);
      buff[counter] = ch;
      counter++;
   }
}


void printArgs(pid_t pid, struct ptrace_syscall_info sys_info, int argc) {
   if (!strcmp("write", table[sys_info.entry.nr].name)) {
      printf("%llu ", sys_info.entry.args[0]);
      char buff[1024];
      printStr(pid, sys_info.entry.args[1], sys_info.entry.args[2], buff);
      printf("%s ", buff);
      printf("%llu ", sys_info.entry.args[2]);
   } else {
      for (int i = 0; i < argc; i++) {
         printf("%llu ", sys_info.entry.args[i]);
      }
   }
}



int main() {   
   pid_t child;
   long orig_eax;
   int status;
   struct ptrace_syscall_info sys_info;
   child = fork();
   if(child == 0) {
      ptrace(PTRACE_TRACEME, 0, NULL, NULL);
      execl("./hello_world", "hello_world", NULL);
   } else {
      waitpid(child, &status, 0);
      ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);
      while(WIFSTOPPED(status)) {
         ptrace(PTRACE_SYSCALL, child, 0, 0);
         waitpid(child, &status, 0);
         ptrace(PTRACE_GET_SYSCALL_INFO, child, sizeof(sys_info), &sys_info);
         printf("child made syscall %s with args: ", table[sys_info.entry.nr].name);
         printArgs(child, sys_info, table[sys_info.entry.nr].arc); 
         printf("ret = %lld", sys_info.exit.rval);
         printf("\n");
         ptrace(PTRACE_SYSCALL, child, 0, 0);
         waitpid(child, &status, 0);
      }
   }
   return 0;
}

