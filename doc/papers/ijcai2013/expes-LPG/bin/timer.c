#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


void main(int argc, char **argv)
{
  int pid;
  if ((pid = fork()) == 0) {
    execv(argv[2], argv+2);
  } else {
    sleep(atoi(argv[1]));
    kill(pid, SIGKILL);
  }
  
}
