int infinite_loop_test() {
  int n = 1;
  for (;;) {
    if (n>5)
      return n;
    n++;
  }
  return 1; // Unreachable.
}

int main() {
  int got = 0;
  got = infinite_loop_test();
  assert(6, got);
  return 0;
}
