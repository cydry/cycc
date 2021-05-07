int ifs_test(int a) {
  if (a<5) {
    if (a<3) {
      if (a<2) {
	return 1;
      }
    }
  }
  return 7;
}

int main() {
  int got = 0;

  got = ifs_test(1);
  assert(1, got);

  got = ifs_test(2);
  assert(7, got);

  return 0;
}
