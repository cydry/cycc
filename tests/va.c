
typedef __builtin_va_list __cycc_va_list;
typedef __cycc_va_list va_list;

void va_list_test(char* fmt, ...) {
  va_list ap;
  __builtin_va_start(ap, fmt);
  printf(fmt, ap);
}

int main() {
  va_list_test("ap:%d\n", 7);
  return 0;
}
