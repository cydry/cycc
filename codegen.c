
#include "cycc.h"

char* arg_to_reg[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen(Node *node);

int unique_number = 0;
int unique_num() {
  return unique_number++;
}

// Uniqueness for Jump statements in a switch, break loops.
int switch_uniq_number  = 0;
int switch_uniq_current = 0;

int uniq_stack[64];
int uniq_stack_i = 0;

void push_uniq(int uniq) {
  uniq_stack_i++;
  uniq_stack[uniq_stack_i] = uniq;
}

int pop_uniq() {
  int uniq;
  uniq = uniq_stack[uniq_stack_i];
  uniq_stack_i--;
  return uniq;
}

int top_uniq() {
  return uniq_stack[uniq_stack_i];
}

// Issue an unique number for jump stmt.
int switch_uniq_num() {
  int u = switch_uniq_number;
  switch_uniq_current = u;

  switch_uniq_number++;
  return u;
}

// Query current number of jump stmt.
int switch_uniq_has() {
  return switch_uniq_current;
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

Vec* vec_last(Vec* elem) {
  if (!elem)
    return NULL;
  if (!elem->next) {
    return elem;
  }
  return vec_last(elem->next);
}

int vec_len(Vec* elem) {
  if (!elem)
    return 0;
  int acc = vec_len(elem->next);
  acc++;
  return acc;
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

  printf("  .byte %d\n", elem->node->val);
  return acc;
}

// For initializing with block having literals.
int init_block_vec_liter(Vec* elem, int acc) {
  if (!elem)
    return 0;
  acc = init_block_vec_liter(elem->next, acc);
  acc++;

  printf("  .quad %s\n", elem->node->call);
  return acc;
}

bool has_va(Vec* elem) {
  if (!elem)
    return false;
  if (elem->node->kind == ND_PARAM && elem->node->lhs->ty && elem->node->lhs->ty->kind == VA ||
      elem->node->kind == ND_VA)
    return true;
  return has_va(elem->next);
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
    } else if (node->rhs->ty->ptr_to->kind == STRUCT) {
      printf("  pop rax\n");
      printf("  mov rax, QWORD PTR[rax]\n");
      printf("  push rax\n");
    } else {
      error("Invalid type at dereferencing.\n");
    }
  } else {
    error("Failed to find lvar node at dereferencing.\n");
  }
  return;
}

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR && node->kind != ND_GVAR &&
      node->kind != ND_VA) {
    error("Not lvalue of the assignment");
  }

  if (node->kind == ND_LVAR || node->kind == ND_VA) {
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
  int uniq = 0; // Make a label to have a unique label.
  int  sw_uniq = 0;           // Have a label to end in ND_SWITCH, ND_CONTIN.
  Vec* sw_default = NULL; // Have a temprarily node to end in ND_SWITCH.
  Vec* sw_elem = NULL;    // Have a temprarily node to be handled in ND_SWITCH.

  switch (node->kind) {
  case ND_NUM:
  case ND_CHAR:
    printf("  push %d\n", node->val);
    return;
  case ND_NULL:
    printf("  xor rax, rax\n");
    printf("  push rax\n");
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
  case ND_VA:
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
      switch (type_size(node->lhs->ty)) {
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
    } else if (node->lhs && node->lhs->kind == ND_DEREF) {
      switch (type_size(node->lhs->rhs->ty->ptr_to)) {
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
    if (node->lhs) {
      gen(node->lhs);
      printf("  pop rax\n");
    } else {
      printf("  mov rax, 0\n");
    }
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
    sw_uniq = switch_uniq_num();
    push_uniq(sw_uniq);
    printf("#SW-BEGIN %d\n", sw_uniq);
    gen(node->lhs);
    printf("#SWITCH-COND-VALUE^\n");
    for (sw_elem = node->block; sw_elem; sw_elem = sw_elem->next) {
      if (sw_elem->node->kind == ND_DEFAU) {
	sw_default = sw_elem;
      } else if (sw_elem->node->kind == ND_CASE) {
	uniq = unique_num();
	printf("#CASE-BEGIN %d\n", uniq);
	gen(sw_elem->node->lhs);
	printf("  pop rdi\n");
	printf("  pop rax  #SWITCH-VALUE\n");
	printf("  push rax #SWITCH-VALUE-REUSE\n");
	printf("  cmp rax, rdi\n");
	if (sw_elem->node->rhs->block) {
	  printf("  je .Lcase%d\n", uniq);
	} else {
	  printf("  je .Lcase%d\n", uniq+1);
	}
	printf("  jmp .Lcaend%d\n", uniq);
	printf(".Lcase%d:\n", uniq);
	gen(sw_elem->node->rhs);
	printf("  pop rax #DISCARD-STMT-VALUE\n");
	printf(".Lcaend%d:\n", uniq);
      } else {
	error("Unsupportd node in switch.");
      }
    }
    if (sw_default)
      gen(sw_default->node);
    printf(".Lbrk%d: #SW-BREAK\n", sw_uniq);
    pop_uniq();
    return;
  case ND_DEFAU:
    printf("#ND_DEFAU\n");
    gen(node->lhs);
    return;
  case ND_BRK:
    sw_uniq = top_uniq();
    printf("  jmp .Lbrk%d\n", sw_uniq);
    return;
  case ND_WHILE:
    uniq = unique_num();
    sw_uniq = switch_uniq_num();
    push_uniq(sw_uniq);
    printf(".Lbegin%d:\n", uniq);
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lend%d\n", uniq);

    if (node->rhs->kind == ND_BLOCK) {
      gen_vec_rev(node->rhs->block);
    } else {
      gen(node->rhs);
    }
    printf(".Lcontin%d:\n", sw_uniq);
    printf("  jmp .Lbegin%d\n", uniq);
    printf(".Lend%d:\n", uniq);
    printf(".Lbrk%d: #WHILE-END\n", sw_uniq);
    pop_uniq();
    return;
  case ND_CONTIN:
    printf("  jmp .Lcontin%d\n", top_uniq());
    return;
  case ND_FOR:
    uniq = unique_num();
    sw_uniq = switch_uniq_num();
    push_uniq(sw_uniq);
    if (node->lhs->lhs)     // Clause-1
      gen(node->lhs->lhs);
    printf(".Lbegin%d:\n", uniq);
    gen(node->lhs->rhs);    // Clause-2
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lend%d\n", uniq);
    if (node->rhs->rhs)     // Loop-Body
      gen(node->rhs->rhs);
    if (node->rhs->lhs)     // Clause-3
      gen(node->rhs->lhs);
    printf(".Lcontin%d:\n", sw_uniq);
    printf("  jmp .Lbegin%d\n", uniq);
    printf(".Lbrk%d: #FOR-BREAK\n", sw_uniq);
    printf(".Lend%d:\n", uniq);
    pop_uniq();
    return;
  case ND_BLOCK:
    gen_vec_rev(node->block);
    return;
  case ND_CALL:
    if (has_va(node->param)) {
      gen_vec(node->param);
      int i;
      for (i = 0; i < vec_len(node->param); i++)
	printf("  pop %s\n", arg_to_reg[i]);

      printf("  mov rax, %s\n", arg_to_reg[i-1]); // start position on ap, address.
      printf("  push [rax]\n");
      printf("  pop %s\n", arg_to_reg[i-1]);

      for (i = vec_len(node->param); i < 6; i++) {
	printf("  add rax, 8\n");
        printf("  push [rax]\n");
	printf("  pop %s\n", arg_to_reg[i]);
      }

    } else {
      gen_vec(node->param);
      for (int i = 0; i < vec_len(node->param); i++)
	printf("  pop %s\n", arg_to_reg[i]);
    }
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
    if (has_va(node->param)) {
      gen_vec(node->param);
      for (int i = 0; i < vec_len(node->param); i++) {
	printf("  pop rax\n");
	printf("  mov [rax], %s\n", arg_to_reg[i]);
      }
      for (int i = vec_len(node->param); i < 6; i++) {
	printf("  add rax, 8\n");
	printf("  mov [rax], %s\n", arg_to_reg[i]);
      }
    } else {
      gen_vec(node->param);
      for (int i = 0; i < vec_len(node->param); i++) {
	printf("  pop rax\n");
	printf("  mov [rax], %s\n", arg_to_reg[i]);
      }
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
      } else if (node->rhs->kind == ND_NULL) {
        printf("  .quad 0\n");
      } else if (node->rhs->kind == ND_LITER) {
	LVar* ivar = find_initials(node->rhs->call);
	if (ivar) {
	  char* p = ivar->lite;
	  p++;
	  while(*p != '"') {
	    printf("  .byte 0x%x\n", *p);
	    p++;
	  }
	  printf("  .byte '\\0'\n");
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
	else if (node->rhs->param->node->kind == ND_LITER)
	  acc = init_block_vec_liter(node->rhs->param, 0);
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
    switch (type_size(node->ty)) {
    case 1:
      printf("  pop rax\n");
      printf("  movzx rax, BYTE PTR[rax] #ND_MEMB\n");
      printf("  push rax\n");
      break;
    case 4:
      printf("  pop rax\n");
      printf("  mov eax, DWORD PTR[rax] #ND_MEMB\n");
      printf("  push rax\n");
      break;
    case 8:
      printf("  pop rax\n");
      printf("  mov rax, QWORD PTR[rax] #ND_MEMB\n");
      printf("  push rax\n");
      break;
    }
    return;
  case ND_CONDOR:
    if (!node->lhs || node->lhs->kind != ND_COND)
      error("Should have conditional node.");
    uniq = unique_num();
    gen(node->lhs->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  jne .Lok%d\n", uniq);
    gen(node->rhs);
    printf("  jmp .Lend%d\n", uniq);
    printf(".Lok%d:\n", uniq);
    gen(node->lhs->rhs);
    printf(".Lend%d:\n", uniq);
    return;
  case ND_AND:
    uniq = unique_num();
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  xor rsi, rsi\n");
    printf("  cmp rax, rsi\n");
    printf("  je .Lante%d\n", uniq);
    gen(node->rhs);
    printf("  xor rsi, rsi\n");
    printf("  pop rdi\n");
    printf("  cmp rdi, rsi\n");
    printf("  je .Ldesc%d\n", uniq);
    printf("  mov rax, 1\n");
    printf("  jmp .Lend%d\n", uniq);
    printf(".Lante%d:\n", uniq);
    printf(".Ldesc%d:\n", uniq);
    printf("  mov rax, 0\n");
    printf(".Lend%d:\n", uniq);
    printf("  push rax\n");
    return;
  case ND_OR:
    uniq = unique_num();
    gen(node->lhs);
    printf("  xor rsi, rsi\n");
    printf("  pop rax\n");
    printf("  cmp rax, rsi\n");
    printf("  jne .Lante%d\n", uniq);
    gen(node->rhs);
    printf("  xor rsi, rsi\n");
    printf("  pop rdi\n");
    printf("  cmp rdi, rsi\n");
    printf("  jne .Ldesc%d\n", uniq);
    printf("  mov rax, 0\n");
    printf("  jmp .Lend%d\n", uniq);
    printf(".Lante%d:\n", uniq);
    printf(".Ldesc%d:\n", uniq);
    printf("  mov rax, 1\n");
    printf(".Lend%d:\n", uniq);
    printf("  push rax\n");
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
  case ND_REM:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    printf("  mov rax, rdx\n");
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
  case ND_BITOR:
    printf("  or rax, rdi\n");
    break;
  case ND_BITXOR:
    printf("  xor rax, rdi\n");
    break;
  case ND_BITAND:
    printf("  and eax, edi\n");
    break;
  }

  printf("  push rax\n");
}

void genasm() {
  printf(".intel_syntax noprefix\n");

  Node* node;
  int i = 0;
  while (code[i]) {
    node = code[i];
    if (node->kind == ND_FUNC || node->kind == ND_GDECL) {
      if (strcmp(node->call, ""))
        printf(".globl %s\n", node->call);
    }
    i++;
  }

  printf(".data\n");
  for (LVar* var = literals; var; var = var->next) {
    printf("%s:\n", var->name);
    printf("  .string %s\n", var->lite);
  }

  int text = 0;
  i = 0;
  while (code[i]) {
    node = code[i];
    if (!text && node->kind == ND_FUNC) {
      printf(".text\n");
      text = 1;
    } else if (text && node->kind != ND_FUNC) {
      printf(".data\n");
      text = 0;
    }

    if (node->is_ext) {
      i++;
      continue;
    }

    gen(code[i++]);
  }
}
