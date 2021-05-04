struct T {
  int a;
};

char struct_test1() {
  struct T x;
  x.a = 2;
  return x.a;
}

char struct_test2() {
  struct T x;
  struct T* p;
  p = &x;
  x.a = 2;
  return p->a;
}

int main() {
  int got = 0;

  got = struct_test1();
  assert(2, got);

  got = struct_test2();
  assert(2, got);
  return 0; 
}
