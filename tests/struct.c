struct T {
  int a;
};

struct T1 {
  int b;
  struct T* c;
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

int struct_test3() {
  struct T   x;
  struct T1  x1;
  struct T1* p1;

  x.a  = 2;
  x1.c = &x;
  p1   = &x1;

  return p1->c->a;
}

int main() {
  int got = 0;

  got = struct_test1();
  assert(2, got);

  got = struct_test2();
  assert(2, got);

  got = struct_test3();
  assert(2, got);
  return 0; 
}
