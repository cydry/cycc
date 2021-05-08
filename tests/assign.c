int assign_test() {
  int a = 1;
  a += 2;
  return a;
}

int assign_test1() {
  int a = 1;
  a += 2 + 2;
  return a;
}

int assign_test2() {
  int a = 2;
  a -= 1;
  return a;
}

int assign_test3() {
  int a = 5;
  a -= 1 + 2;
  return a;
}

int assign_test4() {
  int a = 5;
  a *= 2;
  return a;
}

int assign_test5() {
  int a = 5;
  a /= 2;
  return a;
}

int assign_test6() {
  int a = 5;
  a %= 2;
  return a;
}

int main() {
  int got = 0;

  got = assign_test();
  assert(3, got);

  got = assign_test1();
  assert(5, got);

  got = assign_test2();
  assert(1, got);

  got = assign_test3();
  assert(2, got);

  got = assign_test4();
  assert(10, got);

  got = assign_test5();
  assert(2, got);

  got = assign_test6();
  assert(1, got);

  return 0;
}
