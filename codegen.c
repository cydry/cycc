#include "cycc.h"

int unique_number = 0;
int unique_num() {
  return unique_number++;
}

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error("Not lvalue of the assignment");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  int uniq; // Make a label to have a unique label.

  switch (node->kind) {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  case ND_RETURN:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_IF:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    if (node->rhs->kind == ND_ELSE)
      return gen(node->rhs);
    uniq = unique_num();
    printf("  je .Lend%d\n", uniq);
    gen(node->rhs);
    printf(".Lend%d:\n", uniq);
    return;
  case ND_ELSE:
    uniq = unique_num();
    printf("  je  .Lelse%d\n", uniq);
    gen(node->lhs);
    printf("  jmp .Lend%d\n", uniq);
    printf(".Lelse%d:\n", uniq);
    gen(node->rhs);
    printf(".Lend%d:\n", uniq);
    return;
  case ND_WHILE:
    uniq = unique_num();
    printf(".Lbegin%d:\n", uniq);
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lend%d\n", uniq);
    gen(node->rhs);
    printf("  jmp .Lbegin%d\n", uniq);
    printf(".Lend%d:\n", uniq);
    return;
  case ND_FOR:
    uniq = unique_num();
    gen(node->lhs->lhs);
    printf(".Lbegin%d:\n", uniq);
    gen(node->lhs->rhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lend%d\n", uniq);
    gen(node->rhs->rhs);
    gen(node->rhs->lhs);
    printf("  jmp .Lbegin%d\n", uniq);
    printf(".Lend%d:\n", uniq);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQL:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NEQ:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LEQ:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LES:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  }

  printf("  push rax\n");
}

void genasm(Node* node) {
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // Prologue
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  int i = 0;
  while (code[i]) {
    gen(code[i++]);
    printf("  pop rax\n");
  }

  // Epilogue
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}
