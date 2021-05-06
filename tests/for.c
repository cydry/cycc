int infinite_loop_test() {
  int n = 1;
  for (;;) {
    if (n>5)
      return n;
    n++;
  }
  return 1; // Unreachable.
}

int loop_scope_var_test() {
  int n = 5;
  int x = 0;
  for (int i=0; i<n; i++) {
    x = x + i;
  }
  return x;
}

int main() {
  int got = 0;

  got = infinite_loop_test();
  assert(6, got);

  got = loop_scope_var_test();
  assert(10, got);
  return 0;
}
