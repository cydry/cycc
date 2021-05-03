int remainder_test(int a, int b) {
    return a % b;
}

int main() {
  int got = 0;

  got = remainder_test(0, 1);
  assert(0, got);

  got = remainder_test(1, 2);
  assert(1, got);

  got = remainder_test(3, 2);
  assert(1, got);

  got = remainder_test(2, 2);
  assert(0, got);

  return 0;
}
