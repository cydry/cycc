struct T {
  int a;
};

struct T1 {
  int b;
  struct T* c;
};

struct T2 {
  char* name;
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

int struct_test4() {
  struct T2  x;
  struct T2* y;
  char name[10];

  y = &x;
  x.name = name;

  name[0] = 'm';

  return y->name[0];
}

int struct_test5() {
  struct T2  x;
  struct T2* y;
  char name[10];

  y = &x;
  x.name = name;

  name[1] = 'n';

  return y->name[1];
}

int main() {
  int got = 0;

  got = struct_test1();
  assert(2, got);

  got = struct_test2();
  assert(2, got);

  got = struct_test3();
  assert(2, got);

  got = struct_test4();
  assert(109, got);

  got = struct_test5();
  assert(110, got);
  return 0; 
}
