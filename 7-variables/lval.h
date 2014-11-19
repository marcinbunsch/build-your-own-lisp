#ifndef LVAL_H_LOADED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../shared/mpc.h"

#define LVAL_H_LOADED

struct lval;
struct lenv;

typedef struct lval lval;
typedef struct lenv lenv;

typedef lval*(*lbuiltin)(lenv*, lval*);

// Possible lval types
enum {
  LVAL_NUM,  // number
  LVAL_ERR,  // error
  LVAL_SYM,  // symbol
  LVAL_SEXPR, // symbolic expression
  LVAL_QEXPR,  // quoted expression
  LVAL_FUN  // function
};

// Possible lval error types
enum {
  LERR_DIV_ZERO,
  LERR_BAD_OP,
  LERR_BAD_NUM
};

// lval stands for Lisp Value
struct lval {
  int type;
  long num;

  // error and symbol types have string data
  char* err;
  char* sym;
  lbuiltin fun;

  // list of lvals
  int count;
  struct lval** cell;

};

lval* lval_num(long x);
lval* lval_err(char* x);
lval* lval_sym(char* x);
lval* lval_sexpr(void);
lval* lval_fun(lbuiltin func);
lval* lval_read(mpc_ast_t* t);
void lval_del(lval* v);
lval* lval_copy(lval* v);
void lval_print(lval* v);
void lval_println(lval* v);
lval* lval_eval(lenv* env, lval* v);
lval* lval_eval_sexpr(lenv* env, lval* v);
lval* lval_pop(lval* v, int i);
lval* lval_take(lval* v, int i);
lval* builtin_op(lenv* env, lval* a, char* op);
lval* builtin_head(lenv* env, lval* a);
lval* builtin_tail(lenv* env, lval* a);
lval* builtin_list(lenv* env, lval* a);
lval* builtin_eval(lenv* env, lval* a);
lval* builtin_join(lenv* env, lval* a);
lval* builtin_add(lenv* env, lval* a);
lval* builtin_subtract(lenv* env, lval* a);
lval* builtin_multiply(lenv* env, lval* a);
lval* builtin_divide(lenv* env, lval* a);

#endif
