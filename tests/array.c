int array_with_calucated_index_test() {
  int a[10];
  int i = 3;

  a[2] = 5;

  return a[i-1];
}

int array_with_calucated_index_test1() {
  int a[10];
  int i = 3;

  a[5] = 7;

  return a[i+2];
}

int main() {
  int got = 0;

  got = array_with_calucated_index_test();
  assert(5, got);

  got = array_with_calucated_index_test1();
  assert(7, got);
  return 0;
}

