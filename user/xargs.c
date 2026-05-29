#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"


int main(int argc, char *argv[])
{
  if (argc == 1) {
    fprintf(2, "usage: xargs <command> <...args>\n");
    exit(1);
  }

  char *args[MAXARG];
  char buf[512], *p;
  int n = 0;

  // read 'xargs' args first
  for (int i = 1; i < argc; i++) {
    args[n++] = argv[i];
  }

  // read input from the pipe line by line
  while (gets(buf, sizeof(buf)) != 0) {
    if (buf[0] == '\0')
      break;

    int i = n;
    p = buf;

    while(*p != 0) {
      args[i++] = p;

      while(*p != '\0' && *p != '\n' &&  *p != ' ')
        p++;

      *p++ = 0;
    }

    // now have {argv..., args...}
    args[i] = 0;

    // now run the command for each line
    int pid = fork();

    if (pid < 0) {
      fprintf(2, "xargs: failed to run %s\n", argv[1]);
      exit(1);
    }

    if (pid == 0) {
      exec(argv[1], args);
      fprintf(2, "exec %s failed\n", argv[1]);
    }

    wait(0);
  }

  exit(0);
}
