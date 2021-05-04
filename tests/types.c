typedef int   D;
typedef char  D1;
typedef int*  D2;
typedef char* D3;

typedef struct T DT;

struct T {
  int a;
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

  return 0;
}
