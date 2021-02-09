#include <stdio.h>

int foo() { printf("2"); }
int bar(int a, int b) { printf("%d", a + b); }

int buzz(int a, int b, int c, int d, int e, int f) {
  printf("%d", a + b + c + d + e + f);
}
