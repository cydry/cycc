
#include "cycc.h"

char* arg_to_reg[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen(Node *node);

int unique_number = 0;
int unique_num() {
  return unique_number++;
}

// Generates each node of Vecs by reverse order.
void gen_vec_rev(Vec* elem) {
  if (!elem)
    return;
  gen_vec_rev(elem->next);
  gen(elem->node);
}

void gen_vec(Vec* elem) {
  if (!elem)
    return;
  gen(elem->node);
  gen_vec(elem->next);
}

int vec_len(Vec* elem) {
  if (!elem)
    return 0;
  int acc = vec_len(elem->next);
  acc++;
  return acc;
}

void gen_deref(Node* node, int acc) {
  // Counts the number of dereferencing, using accumrator parameter.
  if (node->rhs->kind == ND_DEREF) {
    acc++;
    gen_deref(node->rhs, acc);
    return;
  }

  // Do dereferencing.
  gen(node->rhs);
  for (int i = 0; i < acc; i++) {
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
  }

  if (node->rhs) {
    // The type, be pointed to.
    if (node->rhs->ty->ptr_to->kind == INT) {
      printf("  pop rax\n");
      printf("  mov eax, DWORD PTR[rax]\n");
      printf("  push rax\n");
    } else if (node->rhs->ty->ptr_to->kind == PTR) {
      printf("  pop rax\n");
      printf("  mov rax, QWORD PTR[rax]\n");
      printf("  push rax\n");

    } else if (node->rhs->ty->ptr_to->kind == ARRAY) {  // Dereferencing an array,
      if (node->rhs->ty->ptr_to->ptr_to->kind == INT) { // the unit is element's size.
	printf("  pop rax\n");
	printf("  mov eax, DWORD PTR[rax]\n");
	printf("  push rax\n");
      } else if (node->rhs->ty->ptr_to->ptr_to->kind == PTR) {
	printf("  pop rax\n");
	printf("  mov rax, QWORD PTR[rax]\n");
	printf("  push rax\n");
      } else {
	error("Invalid type at dereferencing an array.\n");
      }
    } else {
      error("Invalid type at dereferencing.\n");
    }
  } else {
    error("Failed to find lvar node at dereferencing.\n");
  }
  return;
}

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR && node->kind != ND_GVAR)
    error("Not lvalue of the assignment");

  if (node->kind == ND_LVAR) {
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");

  } else  if (node->kind == ND_GVAR) {
    if (node->ty->kind == INT) {
      printf("  lea rax, DWORD PTR %s[rip]\n", node->call);
    } else if (node->ty->kind == PTR) {
      printf("  lea rax, QWORD PTR %s[rip]\n", node->call);

    } else if (node->ty->kind == ARRAY) {
      if (node->ty->ptr_to->kind == INT) {
	printf("  lea rax, DWORD PTR %s[rip]\n", node->call);
      } else if(node->ty->ptr_to->kind == PTR) {
	printf("  lea rax, QWORD PTR %s[rip]\n", node->call);
      } else {
	error("Unsupported type of array at `gen_lval'");
      }
    } else {
      error("Unsupported type of `gen_lval'");
    }
    printf("  push rax\n");
  }
}

void gen(Node *node) {
  int uniq; // Make a label to have a unique label.

  switch (node->kind) {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lval(node);
    if (node->ty->kind == PTR &&
	node->ty->ptr_to &&
	node->ty->ptr_to->kind == ARRAY)
      return;
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_GVAR:
    gen_lval(node);
    if (node->ty->kind == PTR &&
	node->ty->ptr_to &&
	node->ty->ptr_to->kind == ARRAY)
      return;
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    if (node->lhs->kind == ND_DEREF)
      gen(node->lhs->rhs); // as rvalue
    else
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
  case ND_BLOCK:
    gen_vec_rev(node->block);
    return;
  case ND_CALL:
    gen_vec(node->param);
    for (int i = 0; i < vec_len(node->param); i++)
      printf("  pop %s\n", arg_to_reg[i]);
    uniq = unique_num();
    printf("  mov rax, rsp\n");
    printf("  mov r10b, 15\n");
    printf("  movzb r10, r10b\n");
    printf("  and rax, r10\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lelse%d\n", uniq);
    printf("  push 1\n");
    printf("  jmp  .Lend%d\n", uniq);
    printf(".Lelse%d:\n", uniq);
    printf("  push 1\n");
    printf("  push 2\n");
    printf(".Lend%d:\n", uniq);
    printf("  mov rax, %d\n", vec_len(node->param));
    printf("  call %s\n", node->call);

    uniq = unique_num();
    printf("  pop rdi\n");
    printf("  cmp rdi, 1\n");
    printf("  je  .Lend%d\n", uniq);
    printf("  pop rdi\n");
    printf(".Lend%d:\n", uniq);
    printf("  push rax\n");
    return;
  case ND_FUNC:
    printf("%s:\n", node->call);
    // Prologue
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", node->locals);
    gen_vec(node->param);
    for (int i = 0; i < vec_len(node->param); i++) {
      printf(" pop rax\n");
      printf(" mov [rax], %s\n", arg_to_reg[i]);
    }

    // statements of the function.
    gen(node->rhs);
    printf("  pop rax\n");

    // Epilogue
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_PARAM:
    gen_lval(node->lhs);
    return;
  case ND_GDECL:
    printf("%s:\n", node->call);
    printf("  .zero %d\n", node->offset);
    return;
  case ND_DECL:
    return;
  case ND_ADDR:
    gen_lval(node->rhs);
    return;
  case ND_DEREF:
    gen_deref(node, 0);
    return;
  case ND_SIZE:
    gen(node->rhs);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    if (node->lhs && node->lhs->ty && node->lhs->ty->kind == PTR){

      if (node->lhs->ty->ptr_to->kind == PTR)
	printf("  imul rdi, 8\n");
      if (node->lhs->ty->ptr_to->kind == INT)
	printf("  imul rdi, 4\n");

      // Evaluating addition of pointer to array, the unit is determined by the element's size.
      // Array's data structure is...
      //
      // PTR -> ARRAY -> (Element's type)
      //
      if (node->lhs->ty->ptr_to->kind == ARRAY) {
	if (node->lhs->ty->ptr_to->ptr_to->kind == PTR) // elements of the array..
	  printf("  imul rdi, 8\n");
	if (node->lhs->ty->ptr_to->ptr_to->kind == INT)
	  printf("  imul rdi, 4\n");
      }
    }
    if (node->rhs && node->rhs->ty && node->rhs->ty->kind == PTR) {

      if (node->rhs->ty->ptr_to->kind == PTR)
	printf("  imul rax, 8\n");
      if (node->rhs->ty->ptr_to->kind == INT)
	printf("  imul rax, 4\n");

      // Evaluating addition of pointer to array, the unit is determined by the element's size.
      // Array's data structure is...
      //
      // PTR -> ARRAY -> (Element's type)
      //
      if (node->rhs->ty->ptr_to->kind == ARRAY) {
	if (node->rhs->ty->ptr_to->ptr_to->kind == PTR) // elements of the array..
	  printf("  imul rax, 8\n");
	if (node->rhs->ty->ptr_to->ptr_to->kind == INT)
	  printf("  imul rax, 4\n");
      }
    }
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    if (node->lhs && node->lhs->ty && node->lhs->ty->kind == PTR) {

      if (node->lhs->ty->ptr_to->kind == PTR)
	printf("  imul rdi, 8\n");
      if (node->lhs->ty->ptr_to->kind == INT)
	printf("  imul rdi, 4\n");

      // Evaluating substraction of pointer to array, same as addition of it.
      if (node->lhs->ty->ptr_to->kind == ARRAY) {
	if (node->lhs->ty->ptr_to->ptr_to->kind == PTR)
	  printf("  imul rdi, 8\n");
	if (node->lhs->ty->ptr_to->ptr_to->kind == INT)
	  printf("  imul rdi, 4\n");
      }
    }
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

void genasm() {
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");

  printf(".data\n");
  Node* node;
  int text = 0;
  int i = 0;
  while (code[i]) {
    node = code[i];
    if (node->kind == ND_FUNC && !text) {
      printf(".text\n");
      text = 1;
    }
    gen(code[i++]);
  }
}
