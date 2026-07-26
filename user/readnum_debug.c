#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  char c;
  int r;

  uint x1 = readnum();
  uint x2 = readnum();

  printf("Press a key: ");
  r = read(0, &c, 1);
  printf("read() returned %d, char='%c'\n", r, c);

  uint x3 = readnum();

  printf("Press 10 keys:\n");
  for (int i = 0; i < 10; i++) {
    r = read(0, &c, 1);
    printf("[%d] read() returned %d, char='%c'\n", i, r, c);
  }

  uint x4 = readnum();

  printf("\n");
  printf("READNUM_TEST_OUTPUT\n");
  printf("x1 = %u (expected 0)\n", x1);
  printf("x2 = %u (expected 0)\n", x2);
  printf("x3 = %u (expected 1)\n", x3);
  printf("x4 = %u (expected 11)\n", x4);

  exit(0);
}