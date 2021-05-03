int cond_test(int x) {
  return x ? 2 : 3;
}

int cond_test1(int x) {
  return x-1 ? 2 : 3;
}

int cond_test2(int a, int b) {
  return a ? 2 : b ? 3 : 5;
}

int main() {
  int got = 0;
  got = cond_test(1);
  assert(2, got);

  got = cond_test(0);
  assert(3, got);

  
  got = cond_test1(1);
  assert(3, got);

  got = cond_test1(0);
  assert(2, got);

  
  got = cond_test2(1, 0);
  assert(2, got);

  got = cond_test2(0, 1);
  assert(3, got);

  got = cond_test2(0, 0);
  assert(5, got);

  return 0;
}
