#include <kernel/types.h>
#include <user/user.h>

void sieve(int fd_filterd_primes);

int main(int argc, char *argv[])
{
  if (argc > 1) {
    fprintf(2, "primes: this program doesn't receive arguments or flags\n");
    exit(1);
  }

  int fd_primes[2];
  int pid;

  if (pipe(fd_primes) < 0) {
    fprintf(2, "primes: couldn't create first pipe\n");
    exit(1);
  }

  pid = fork();

  switch(pid) {
  case -1:
    fprintf(2, "primes: couldn't create first right neighbor\n");
    exit(1);
    break;

  case 0:
    close(fd_primes[1]);
    sieve(fd_primes[0]);
    close(fd_primes[0]);
    exit(0);
    break;

  default:
    close(fd_primes[0]);
    for (uint i = 2; i <= 35; i++) {
      write(fd_primes[1], &i, sizeof(uint));
    }
    close(fd_primes[1]);
    wait(0);
    exit(0);
  }
}

void sieve(int fd_filtered)
{
  uint p;
  if (read(fd_filtered, &p, sizeof(uint)) <= 0) return;
  printf("prime %d\n", p);

  int fd_filtered_new[2];
  int pid;

  if (pipe(fd_filtered_new) < 0) {
    fprintf(2, "primes: couldn't create pipe at left neighbor {%d}\n", p);
    exit(1);
  }

  pid = fork();

  switch(pid) {
  case -1:
    fprintf(2, "primes: couldn't create new left neighbor from {%d}\n", p);
    exit(1);
    break;

  case 0:
    close(fd_filtered);
    close(fd_filtered_new[1]);
    sieve(fd_filtered_new[0]);
    close(fd_filtered_new[0]);
    exit(0);
    break;

  default:
    close(fd_filtered_new[0]);
    uint n;
    while(read(fd_filtered, &n, sizeof(uint)) > 0) {
      if (n % p > 0) {
        write(fd_filtered_new[1], &n, sizeof(uint));
      }
    }
    close(fd_filtered);
    close(fd_filtered_new[1]);
    wait(0);
    exit(0);
  }
}
