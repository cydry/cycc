int negate_test(int a) {
    return !a;
}

int main() {
  int got = 2;

  got = negate_test(0);
  assert(1, got);
  
  got = negate_test(1);
  assert(0, got);

  got = negate_test(3);
  assert(0, got);

  return 0;
}
