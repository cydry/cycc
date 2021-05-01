
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

int has_vec_break(Vec* elem) {
  if (!elem)
    return 0;

  if (!elem->next) {
    if (elem->node->kind == ND_BRK)
      return 1;
    return 0;
  }

  return has_vec_break(elem->next);
}

// For initialize with block having numbers.
int init_block_vec(Vec* elem, int acc) {
  if (!elem)
    return 0;
  acc = init_block_vec(elem->next, acc);
  acc++;

  printf("  .long %d\n", elem->node->val);
  return acc;
}

// For initialize with block having chars.
int init_block_vec_char(Vec* elem, int acc) {
  if (!elem)
    return 0;
  acc = init_block_vec_char(elem->next, acc);
  acc++;

  printf("  .byte %s\n", elem->node->call);
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
    if (node->rhs->ty->ptr_to->kind == CHAR) {
      printf("  pop rax\n");
      printf("  movzx rax, BYTE PTR[rax]\n");
      printf("  push rax\n");
    } else if (node->rhs->ty->ptr_to->kind == INT) {
      printf("  pop rax\n");
      printf("  mov eax, DWORD PTR[rax]\n");
      printf("  push rax\n");
    } else if (node->rhs->ty->ptr_to->kind == PTR) {
      printf("  pop rax\n");
      printf("  mov rax, QWORD PTR[rax]\n");
      printf("  push rax\n");

    } else if (node->rhs->ty->ptr_to->kind == ARRAY) {  // Dereferencing an array,
      if (node->rhs->ty->ptr_to->ptr_to->kind == CHAR) { // the unit is element's size.
	printf("  pop rax\n");
	printf("  movzx eax, BYTE PTR[rax]\n");
	printf("  push rax\n");

      } else if (node->rhs->ty->ptr_to->ptr_to->kind == INT) { // the unit is element's size.
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
    if (node->ty->kind == CHAR) {
      printf("  lea rax, BYTE PTR %s[rip]\n", node->call);

    } else if (node->ty->kind == INT) {
      printf("  lea rax, DWORD PTR %s[rip]\n", node->call);

    } else if (node->ty->kind == PTR) {
      if (node->ty->ptr_to->kind == ARRAY) {
	if (node->ty->ptr_to->ptr_to->kind == CHAR) {
	  printf("  lea rax, BYTE PTR %s[rip]\n", node->call);
	} else if (node->ty->ptr_to->ptr_to->kind == INT) {
	  printf("  lea rax, DWORD PTR %s[rip]\n", node->call);
	} else if(node->ty->ptr_to->ptr_to->kind == PTR) {
	  printf("  lea rax, QWORD PTR %s[rip]\n", node->call);
	} else {
	  error("Unsupported type of array at `gen_lval'");
	}
      } else {
	  printf("  lea rax, QWORD PTR %s[rip]\n", node->call);
      }
    } else if (node->ty->kind == ARRAY) {
      if (node->ty->ptr_to->kind == CHAR) {
	printf("  lea rax, BYTE PTR %s[rip]\n", node->call);
      } else if (node->ty->ptr_to->kind == INT) {
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
  case ND_LITER:
    printf("  lea rax, BYTE PTR %s[rip]\n", node->call);
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    if (node->lhs->kind == ND_DEREF)
      gen(node->lhs->rhs); // as rvalue
    else if (node->lhs->kind == ND_MEMB)
      gen(node->lhs->lhs); // as rvalue
    else
      gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    if (node->lhs && node->lhs->kind == ND_MEMB) {
      switch (type_size(node->lhs->rhs->ty)) {
      case 1:
	printf("  mov BYTE PTR  [rax], dil\n");
	break;
      case 4:
	printf("  mov DWORD PTR [rax], edi\n");
	break;
      case 8:
	printf("  mov QWORD PTR [rax], rdi\n");
	break;
      default:
	printf("  mov [rax], rdi\n");
	break;
      }
    } else {
	printf("  mov [rax], rdi\n"); // default
    }
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
    uniq = unique_num();
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lelse%d\n", uniq);
    gen(node->rhs);
    printf("  jmp .Lend%d\n", uniq);
    printf(".Lelse%d:\n", uniq);
    gen_vec_rev(node->block);
    printf(".Lend%d:\n", uniq);
    return;
  case ND_SWITCH:
    gen(node->lhs);
    printf("#SWITCH-COND-VALUE^\n");
    gen(node->rhs);
    return;
  case ND_CASE:
    uniq = unique_num();
    printf("#CASE-BEGIN %d\n", uniq);
    gen(node->lhs);
    printf("  pop rdi\n");
    printf("  pop rax  #SWITCH-VALUE\n");
    printf("  push rax #SWITCH-VALUE-REUSE\n");
    printf("  cmp rax, rdi\n");
    printf("  je .Lcase%d\n", uniq);
    printf("  jmp .Lcaend%d\n", uniq);
    printf(".Lcase%d:\n", uniq);
    gen(node->rhs);
    printf("  pop rax #DISCARD-STMT-VALUE\n");
    if (has_vec_break(node->block))
	printf("  jmp .Lend%d #JMP-TO-END\n", uniq);
    printf(".Lcaend%d:\n", uniq);
    gen_vec_rev(node->block);
    printf(".Lend%d: #CASE-END\n", uniq);
    return;
  case ND_BRK:
    return; // Nop, Flag. 'break' node is handled by ND_CASE.
  case ND_DEFAU:
    printf("#ND_DEFAU\n");
    gen(node->lhs);
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
    if (node->rhs) {
      if (node->rhs->kind == ND_NUM) {
	printf("  .long %d\n", node->rhs->val);

      } else if (node->rhs->kind == ND_LITER) {
	LVar* ivar = find_initials(node->rhs->call);
	if (ivar) {
	  char* p = ivar->lite;
	  p++;
	  while(*p != '"') {
	    printf("  .byte 0x%x\n", *p);
	    p++;
	  }
	  printf("  .byte '\\0'\n", *p);
	} else {
	  error("Not found a literal with initializer.");
	}
      } else if (node->rhs->kind == ND_ADDR) {
	printf("  .quad %s\n", node->rhs->rhs->call);
      } else if (node->rhs->kind == ND_ADD) {
	printf("  .quad %s + %d\n", node->rhs->lhs->call, node->rhs->rhs->val);
      } else if (node->rhs->kind == ND_PARAM) {
	int acc = 0;
	if (node->rhs->param->node->kind == ND_NUM)
	  acc = init_block_vec(node->rhs->param, 0);
	else if (node->rhs->param->node->kind == ND_CHAR)
	  acc = init_block_vec_char(node->rhs->param, 0);
	else
	  error("Unsupported type with block initializer.");
	if (node->ty->kind == ARRAY) {
	  acc = node->ty->array_size - acc;
	  for (; acc > 0; acc--) {
	    if (node->ty->ptr_to->kind == INT)
	      printf("  .long 0\n");
	    else if (node->ty->ptr_to->kind == CHAR)
	      printf("  .byte 0\n");
	    else if (node->ty->ptr_to->kind == PTR)
	      printf("  .quad 0\n");
	    else
	      error("Unsupported type with block initializer, remaining elements with zero.");
	  }
	}
      } else {
	error("Unsupported type with initializer.");
      }
    } else {
      printf("  .zero %d\n", node->offset);
    }
    return;
  case ND_DECL:
    if (node->rhs) {
      if (node->rhs->rhs && node->rhs->rhs->kind == ND_PARAM) {
	for (Vec* elem = node->rhs->rhs->param; elem; elem = elem->next) {
	  gen(elem->node);
	}
	gen_lval(node->rhs->lhs);
	for (Vec* elem = node->rhs->rhs->param; elem; elem = elem->next) {
	  printf("  pop rax\n");
	  printf("  pop rdi\n");
	  printf("  mov [rax], rdi\n");
	  if (node->rhs->lhs->ty->kind == ARRAY) {
	    if (node->rhs->lhs->ty->ptr_to->kind == INT)
	      printf("  add rax, 4\n");
	    else if (node->rhs->lhs->ty->ptr_to->kind == PTR)
	      printf("  add rax, 8\n");
	    else if (node->rhs->lhs->ty->ptr_to->kind == CHAR)
	      printf("  add rax, 1\n");
	    else
	      error("Unsupported type at block initialization of local variable.");
	  }
	  printf("  push rax\n");
	}
	printf("  pop rax\n"); // Drop address on top of stack.
      } else {
	gen(node->rhs);
      }
    }
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
  case ND_POST:
    gen(node->lhs);
    gen(node->rhs);
    printf("  pop rax\n");
    return;
  case ND_MEMB:
    gen(node->lhs);
    if (node->rhs) {
      switch (type_size(node->rhs->ty)) {
      case 1:
	printf("  pop rax\n");
	printf("  movzx rax, BYTE PTR[rax]\n");
	printf("  push rax\n");
	break;
      case 4:
	printf("  pop rax\n");
	printf("  mov eax, DWORD PTR[rax]\n");
	printf("  push rax\n");
	break;
      case 8:
	printf("  pop rax\n");
	printf("  mov rax, QWORD PTR[rax]\n");
	printf("  push rax\n");
	break;
      }
    } else {
      error("Failed at member accessing.\n");
    }
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
      if (node->lhs->ty->ptr_to->kind == CHAR)
	printf("  imul rdi, 1\n");

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
	if (node->lhs->ty->ptr_to->ptr_to->kind == CHAR)
	  printf("  imul rdi, 1\n");
      }
    }
    if (node->rhs && node->rhs->ty && node->rhs->ty->kind == PTR) {

      if (node->rhs->ty->ptr_to->kind == PTR)
	printf("  imul rax, 8\n");
      if (node->rhs->ty->ptr_to->kind == INT)
	printf("  imul rax, 4\n");
      if (node->rhs->ty->ptr_to->kind == CHAR)
	printf("  imul rax, 1\n");

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
	if (node->rhs->ty->ptr_to->ptr_to->kind == CHAR)
	  printf("  imul rax, 1\n");
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
      if (node->lhs->ty->ptr_to->kind == CHAR)
	printf("  imul rdi, 1\n");

      // Evaluating substraction of pointer to array, same as addition of it.
      if (node->lhs->ty->ptr_to->kind == ARRAY) {
	if (node->lhs->ty->ptr_to->ptr_to->kind == PTR)
	  printf("  imul rdi, 8\n");
	if (node->lhs->ty->ptr_to->ptr_to->kind == INT)
	  printf("  imul rdi, 4\n");
	if (node->lhs->ty->ptr_to->ptr_to->kind == CHAR)
	  printf("  imul rdi, 1\n");
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
  for (LVar* var = literals; var; var = literals->next) {
    printf("%s:\n", var->name);
    printf("  .string %s\n", var->lite);
  }

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
