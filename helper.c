#include <stdio.h>
#include <stdlib.h>

int foo() { printf("2"); }
int bar(int a, int b) { printf("%d", a + b); }

int buzz(int a, int b, int c, int d, int e, int f) {
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

int assert(int expected, int got) {
  if (expected != got) {
    printf("expected=%d, got=%d\n", expected, got);
    exit(got);
  }
}
