int bitwise_or(int a, int b) {
    int ok = a | b;
    return ok;
}

int main() {
  int got = 0;

  got = bitwise_or(-1, 1);
  assert(-1, got);

  got = bitwise_or(0, 1);
  assert(1, got);

  got = bitwise_or(1, 1);
  assert(1, got);

  got = bitwise_or(2, 1);
  assert(3, got);

  got = bitwise_or(3, 4);
  assert(7, got);

  got = bitwise_or(6, 1);
  assert(7, got);

  got = bitwise_or(6, 4);
  assert(6, got);

  got = bitwise_or(7, 4);
  assert(7, got);

  got = bitwise_or(7, 6);
  assert(7, got);

  return 0;
}
