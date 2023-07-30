#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[]) {
  printf(1, "%s", "CS350 proj0 print in user space: ");
  for(int i = 1; i < argc; i++)
    printf(1, "%s%s", argv[i], i+1 < argc ? " " : "\n");
  exit();
}
