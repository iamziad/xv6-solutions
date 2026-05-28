#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

void find(int fd, char* start, char* target)
{
  struct stat st;
  struct dirent de;

  while(read(fd, &de, sizeof(de)) == sizeof(de)) {
    if(de.inum == 0)
      continue;

    if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;

    char path[512], *p;
    strcpy(path, start);
    p = path + strlen(path);
    *p++ = '/';
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    if (stat(path, &st) < 0) {
      printf("find: cannot stat %s\n", de.name);
      continue;
    }

    switch (st.type) {
    case T_FILE:
    case T_DEVICE:
      if (strcmp(de.name, target) == 0)
        printf("%s/%s\n", start, target);
      break;

    case T_DIR:
      int fd_new = open(path, 0);
      find(fd_new, path, target);
      close(fd_new);
      break;

    default:
      break;
    }
  }
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
    fprintf(2, "usage: find <start> <target>\n");
    exit(1);
  }

  int fd;
  struct stat st;

  if ((fd = open(argv[1], 0)) < 0) {
    fprintf(2, "find: couldn't search in %s\n", argv[1]);
    exit(1);
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: couldn't stat %s\n", argv[1]);
    exit(1);
  }

  switch (st.type) {
  case T_FILE:
  case T_DEVICE:
    printf("%s: cannot search in files or devices\n", argv[1]);
    exit(0);

  case T_DIR:
    find(fd, argv[1], argv[2]);

  default:
    exit(1);
  }

  close(fd);
  exit(0);
}
