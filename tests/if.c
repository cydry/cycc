struct T {
  int a;
};

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

int if_null_test() {
  char* p = NULL;
  if (1==1 && !p) {
    return 2;
  }
  return 3;
}

int if_ptr_exist_test() {
  struct T x;
  struct T* p;
  p = &x;

  if (p && 1) {
    return 2;
  }

  return 3;
}

int if_ptr_exist_test1() {
  struct T x;
  struct T* p;
  p = NULL;

  if (p && 1) {
    return 2;
  }
  return 3;
}

int main() {
  int got = 0;

  got = ifs_test(1);
  assert(1, got);

  got = ifs_test(2);
  assert(7, got);

  got = if_null_test();
  assert(2, got);

  got = if_ptr_exist_test();
  assert(2, got);

  got = if_ptr_exist_test1();
  assert(3, got);

  return 0;
}
