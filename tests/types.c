typedef int   D;
typedef char  D1;
typedef int*  D2;
typedef char* D3;

typedef struct T DT;
typedef enum E DE;

typedef struct {
  int a;
} DT2;

struct T {
  int a;
};

enum E {
  A,
  B,
  C,
};

int typedef_test() {
  D a;
  a = 2;
  return a;
}

char typedef_test1() {
  D1 a;
  a = 'a';
  return a;
}

int typedef_test2() {
  int a;
  D2  p;
  p = &a;
  a = 3;
  return *p;
}

char typedef_test3() {
  char a;
  D3   p;
  p = &a;
  a = 'a';
  return *p;
}

int typedef_test4() {
  DT x;
  x.a = 2;
  return x.a;
}

int typedef_test5() {
  DT  x;
  DT* p;
  x.a = 3;
  p = &x;
  return p->a;
}

int typedef_test6() {
  DT2 x;
  x.a = 2;
  return x.a;
}

int typedef_test7() {
  DT2  x;
  DT2* p;
  x.a = 3;
  p = &x;
  return p->a;
}

int typedef_test8() {
  DE x;
  x = A;
  return x;
}

int typedef_test9() {
  DE x;
  x = B;
  return x;
}

int typedef_test10() {
  DE x;
  x = C;
  return x;
}

int main() {
  int got = 0;

  got = typedef_test();
  assert(2, got);

  got = typedef_test1();
  assert(97, got);

  got = typedef_test2();
  assert(3, got);

  got = typedef_test3();
  assert(97, got);

  got = typedef_test4();
  assert(2, got);

  got = typedef_test5();
  assert(3, got);

  got = typedef_test6();
  assert(2, got);

  got = typedef_test7();
  assert(3, got);

  got = typedef_test8();
  assert(0, got);

  got = typedef_test8();
  assert(A, got);

  got = typedef_test9();
  assert(1, got);

  got = typedef_test9();
  assert(B, got);

  got = typedef_test10();
  assert(C, got);

  return 0;
}
