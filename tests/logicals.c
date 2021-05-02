int logical_and(int a, int b) {
    int ok = a && b;
    return ok;
}

int main() {
  int got = 3;
  got = logical_and(1, 1);
  assert(1, got);

  got = logical_and(1, 0);
  assert(0, got);

  got = logical_and(0, 1);
  assert(0, got);

  got = logical_and(0, 0);
  assert(0, got);

  return 0;
}
