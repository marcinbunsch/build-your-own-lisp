#include <stdio.h>

#include "../shared/mpc.h"
#include "lval.h"
#include "eval.h"

lval eval_op(lval x, char *operator, lval y) {

  if (x.type == LVAL_ERR) return x;
  if (y.type == LVAL_ERR) return y;
  if (strcmp(operator, "+") == 0) return lval_num(x.num + y.num);
  if (strcmp(operator, "-") == 0) return lval_num(x.num - y.num);
  if (strcmp(operator, "*") == 0) return lval_num(x.num * y.num);
  if (strcmp(operator, "/") == 0) {
    return y.num == 0
      ? lval_err(LERR_DIV_ZERO)
      : lval_num(x.num / y.num);
  }
  return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t *t) {

  // if tagged as number, return directly
  if (strstr(t->tag, "number")) {
    // wow, I had no idea about errno. Apparently it is thread local, but still creepy
    errno = 0;
    long num = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(num) : lval_err(LERR_BAD_NUM);
  }

  // the operator is always the second child
  char *operator = t->children[1]->contents;

  // store the third child in x
  lval x = eval(t->children[2]);

  // iterate the remaining children and combining
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, operator, eval(t->children[i]));
    i++;
  }

  return x;
}
