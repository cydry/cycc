int ident_has_score() {
  int u_score = 2;
  return u_score;
}

int __initial_has_score() {
  int u_score = 3;
  return u_score;
}

int main() {
  int got = 0;
  got = ident_has_score();
  assert(2, got);

  got = __initial_has_score();
  assert(3, got);
  return 0;
}
