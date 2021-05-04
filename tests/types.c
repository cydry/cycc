typedef int   D;
typedef char  D1;
typedef int*  D2;
typedef char* D3;

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

  return 0;
}
