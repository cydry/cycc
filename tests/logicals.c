int logical_and_test(int a, int b) {
    int ok = a && b;
    return ok;
}

int logical_or_test(int a, int b) {
    int ok = a || b;
    return ok;
}

int main() {
  int got = 3;
  got = logical_and_test(1, 1);
  assert(1, got);

  got = logical_and_test(1, 0);
  assert(0, got);

  got = logical_and_test(0, 1);
  assert(0, got);

  got = logical_and_test(0, 0);
  assert(0, got);


  got = logical_or_test(1, 1);
  assert(1, got);

  got = logical_or_test(1, 0);
  assert(1, got);

  got = logical_or_test(0, 1);
  assert(1, got);

  got = logical_or_test(0, 0);
  assert(0, got);

  return 0;
}
