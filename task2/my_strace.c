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


int main() {   
   pid_t child;
   long orig_eax;
   int status;
   child = fork();
   if(child == 0) {
      ptrace(PTRACE_TRACEME, 0, NULL, NULL);
      execl("./hello_world", "hello_world", NULL);
   } else {
      waitpid(child, &status, 0);
      while(WIFSTOPPED(status)) {
         ptrace(PTRACE_SYSCALL, child, 0, 0);
         waitpid(child, &status, 0);
         orig_eax = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL);
         printf("child made syscall: %s\n", table[orig_eax].name);
         ptrace(PTRACE_SYSCALL, child, 0, 0);
         waitpid(child, &status, 0);
      }
   }
   return 0;
}

