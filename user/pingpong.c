// the parent sends byte to child
// the child prints <pid>: received ping
// the child sends byte to parent
// the parent prints <pid>: received pong

#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int fd_ping[2];
  int fd_pong[2];
  uint8 byte;
  int pid;

  if (pipe(fd_ping) < 0 || pipe(fd_pong) < 0 || (pid = fork()) < 0) {
    fprintf(2, "pingpong: something went wrong\n");
    exit(1);
  }

  if(pid == 0) {
    close(fd_ping[1]);
    close(fd_pong[0]);

    read(fd_ping[0], &byte, sizeof(uint8));
    printf("%d: received ping\n", getpid());
    write(fd_pong[1], &byte, sizeof(uint8));
    exit(0);
  }

  write(fd_ping[1], &byte, sizeof(uint8));
  read(fd_pong[0], &byte, sizeof(uint8));
  printf("%d: received pong\n", getpid());

  wait(0);
  close(fd_ping[0]);
  close(fd_ping[1]);
  close(fd_pong[0]);
  close(fd_pong[1]);
  exit(0);
}
