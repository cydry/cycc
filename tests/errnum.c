extern int* __errno_location ();

int main() {
  printf("%s\n", strerror(*__errno_location()));
  return 0;
}
