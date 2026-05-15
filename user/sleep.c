#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(2, "usage: sleep [delay]\n");
    exit(1);
  }

  int delay = atoi(argv[1]);

  if (delay == 0) {
    fprintf(2, "sleep: %s is not a valid input\n", argv[1]);
    exit(1);
  }

  sleep(delay);
  exit(0);
}
