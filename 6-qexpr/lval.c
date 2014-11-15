#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"

static const int LVAL_SIZE = sizeof(lval);
static const int LVALP_SIZE = sizeof(lval*);

// create a new number lisp value
lval* lval_num(long x) {
  lval* v = malloc(LVAL_SIZE);
  v->type = LVAL_NUM;
  v->num = x;
  return v;
}

lval* lval_err(char* m) {
  lval* v = malloc(LVAL_SIZE);
  v->type = LVAL_ERR;
  v->err = malloc(strlen(m) + 1);
  strcpy(v->err, m);
  return v;
}

lval* lval_sym(char* s) {
  lval* v = malloc(LVAL_SIZE);
  v->type = LVAL_SYM;
  v->sym = malloc(strlen(s) + 1);
  strcpy(v->sym, s);
  return v;
}

lval* lval_sexpr(void) {
  lval* v = malloc(LVAL_SIZE);
  v->type = LVAL_SEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

lval* lval_qexpr(void) {
  lval* v = malloc(LVAL_SIZE);
  v->type = LVAL_QEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

void lval_del(lval* v) {
  switch (v->type) {

    // do nothing special for the number type
    case LVAL_NUM: break;

    // for errors and symbols, free the corresponding strings
    case LVAL_ERR:
      free(v->err);
    break;
    case LVAL_SYM:
      free(v->sym);
    break;

    case LVAL_QEXPR:
    case LVAL_SEXPR:
      for (int i = 0; i < v->count; i++) {
        lval_del(v->cell[i]);
      }
      free(v->cell);
    break;
  }

  free(v);
}

lval* lval_read_num(mpc_ast_t* t) {
  errno = 0;
  long num = strtol(t->contents, NULL, 10);
  return errno != ERANGE ? lval_num(num) : lval_err("Invalid number");
}

lval* lval_add(lval* parent, lval* addition) {
  // holy shit
  parent->count++;
  parent->cell = realloc(parent->cell, LVALP_SIZE * parent->count);
  // even more holy shit
  parent->cell[parent->count-1] = addition;
  return parent;
}

lval* lval_read(mpc_ast_t* t) {
  // convert numbers directly
  if (strstr(t->tag, "number")) return lval_read_num(t);
  if (strstr(t->tag, "symbol")) return lval_sym(t->contents);

  lval* x = NULL;
  if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
  if (strstr(t->tag, "sexpr")) { x = lval_sexpr(); }
  if (strstr(t->tag, "qexpr")) { x = lval_qexpr(); }

  for (int i = 0; i < t->children_num; i++) {
    mpc_ast_t* child = t->children[i];
    char* contents = child->contents;
    if (strcmp(contents, "(") == 0) continue;
    if (strcmp(contents, ")") == 0) continue;
    if (strcmp(contents, "{") == 0) continue;
    if (strcmp(contents, "}") == 0) continue;
    if (strcmp(child->tag, "regex") == 0) continue;
    x = lval_add(x, lval_read(child));
  }

  return x;
}

void lval_expr_print(lval* v, char open, char close) {
  putchar(open);
  for (int i = 0; i < v->count; i++) {
    // print value contained within
    lval_print(v->cell[i]);

    // do not print trailing space if last element
    if (i != v->count-1) {
      putchar(' ');
    }
  }
  putchar(close);
}

void lval_print(lval* v) {
  switch (v->type) {
    case LVAL_NUM: printf("%li", v->num); break;
    case LVAL_ERR: printf("Error %s", v->err); break;
    case LVAL_SYM: printf("%s", v->sym); break;
    case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
    case LVAL_QEXPR: lval_expr_print(v, '{', '}'); break;
  }
}

void lval_println(lval* v) {
  lval_print(v);
  putchar('\n');
}

lval* lval_eval_sexpr(lval* v) {
  // evaluate children
  for (int i = 0; i < v->count; i++) {
    v->cell[i] = lval_eval(v->cell[i]);
  }
  // at this point, the cell list is flat
  // error checking
  for (int i = 0; i < v->count; i++) {
    if (v->cell[i]->type == LVAL_ERR) return lval_take(v, i);
  }
  // empty expression
  if (v->count == 0) return v;
  // single expression
  if (v->count == 1) return lval_take(v, 0);
  // ensure first element is a symbol
  lval* f = lval_pop(v, 0);
  if (f->type != LVAL_SYM) {
    lval_del(f);
    lval_del(v);
    return lval_err("S-expression does not start with a symbol");
  }
  lval* result = builtin(v, f->sym);
  lval_del(f);
  return result;
}

lval* lval_eval(lval* v) {
  // evaluate s-expressions
  if (v->type == LVAL_SEXPR) return lval_eval_sexpr(v);
  // all other types return directly
  return v;
}

lval* lval_pop(lval* v, int i) {
  // find the item at i
  lval *x = v->cell[i];
  // holy mother of god
  memmove(&v->cell[i], &v->cell[i+1], LVALP_SIZE * (v->count-i-1));

  v->count--;

  v->cell = realloc(v->cell, LVALP_SIZE * v->count);
  return x;
}

lval* lval_take(lval* v, int i) {
  lval* x = lval_pop(v, i);
  lval_del(v);
  return x;
}

lval* builtin(lval* a, char* func) {
  if (strcmp("head", func) == 0) return builtin_head(a);
  if (strstr("-+/*", func)) return builtin_op(a, func);
  lval_del(a);
  return lval_err("Unknown function");
}

lval* builtin_op(lval* a, char* op) {

  // ensure all arguments are numbers
  for (int i = 0; i < a->count; i++) {
    if (a->cell[i]->type != LVAL_NUM) {
      // abort
      lval_del(a);
      return lval_err("Cannot operate on non numbers");
    }
  }

  lval* x = lval_pop(a, 0);

  // if no arguments and sub, perform unary negation
  if (strcmp(op, "-") == 0 && a->count == 0) {
    x->num = -x->num;
  }

  while (a->count > 0) {
    lval* y = lval_pop(a, 0);

    if (strcmp(op, "+") == 0) x->num += y->num;
    if (strcmp(op, "-") == 0) x->num -= y->num;
    if (strcmp(op, "*") == 0) x->num *= y->num;
    if (strcmp(op, "/") == 0) {
      if (y->num == 0) {
        lval_del(x);
        lval_del(y);
        x = lval_err("Division by zero");
        break;
      }
      x->num /= y->num;
    }

    lval_del(y);
  }
  lval_del(a);
  return x;
}


lval* builtin_head(lval* a) {
  // check error conditions
  if (a->count != 1) {
    lval_del(a);
    return lval_err("Function 'head' passed too many arguments");
  }

  if (a->cell[0]->type != LVAL_QEXPR) {
    lval_del(a);
    return lval_err("Function 'head' passed incorrect types");
  }

  if (a->cell[0]->count == 0) {
    lval_del(a);
    return lval_err("Function 'head' passed {}");
  }

  // if ok, take first argument, which is the qexpr
  lval* v = lval_take(a, 0);

  // delete all elements that are not head from the qexpr
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }

  return v;
}
