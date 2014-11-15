#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../shared/mpc.h"

// Possible lval types
enum {
  LVAL_NUM,  // number
  LVAL_ERR,  // error
  LVAL_SYM,  // symbol
  LVAL_SEXPR // symbol
};

// Possible lval error types
enum {
  LERR_DIV_ZERO,
  LERR_BAD_OP,
  LERR_BAD_NUM
};

// lval stands for Lisp Value
typedef struct lval {
  int type;
  long num;

  // error and symbol types have string data
  char* err;
  char* sym;

  // list of lvals
  int count;
  struct lval** cell;

} lval;

lval* lval_num(long x);
lval* lval_err(char* x);
lval* lval_sym(char* x);
lval* lval_sexpr(void);
lval* lval_read(mpc_ast_t* t);
void lval_del(lval* v);
void lval_print(lval* v);
void lval_println(lval* v);

