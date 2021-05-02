int bitwise_or(int a, int b) {
    int ok = a | b;
    return ok;
}

int bitwise_xor(int a, int b) {
    int ok = a ^ b;
    return ok;
}

int bitwise_and(int a, int b) {
    int ok = a & b;
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



  got = bitwise_xor(-1, 1);
  assert(-2, got);

  got = bitwise_xor(0, 1);
  assert(1, got);

  got = bitwise_xor(1, 1);
  assert(0, got);

  got = bitwise_xor(2, 1);
  assert(3, got);

  got = bitwise_xor(3, 1);
  assert(2, got);

  got = bitwise_xor(4, 1);
  assert(5, got);

  got = bitwise_xor(5, 1);
  assert(4, got);

  got = bitwise_xor(6, 1);
  assert(7, got);

  got = bitwise_xor(7, 1);
  assert(6, got);



  got = bitwise_and(-1, 1);
  assert(1, got);

  got = bitwise_and(1, 1);
  assert(1, got);

  got = bitwise_and(2, 1);
  assert(0, got);

  got = bitwise_and(3, 1);
  assert(1, got);

  got = bitwise_and(3, 4);
  assert(0, got);

  got = bitwise_and(6, 4);
  assert(4, got);

  got = bitwise_and(7, 4);
  assert(4, got);

  got = bitwise_and(7, 6);
  assert(6, got);

  got = bitwise_and(7, 7);
  assert(7, got);

  return 0;
}
