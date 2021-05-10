#include <stdio.h>
#include <stdlib.h>

void foo() { printf("2"); }
void bar(int a, int b) { printf("%d", a + b); }

void buzz(int a, int b, int c, int d, int e, int f) {
  printf("%d", a + b + c + d + e + f);
}

int alloc4(int **pp, int a, int b, int c, int d) {
  int* p = malloc(sizeof(int)*4);
  *pp = p;

  *p = a;
  p++;
  *p = b;
  p++;
  *p = c;
  p++;
  *p = d;

  return 0;
}

void assert(int expected, int got) {
  if (expected != got) {
    printf("expected=%d, got=%d\n", expected, got);
    exit(1);
  }
}
