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
  }
}

void lval_println(lval* v) {
  lval_print(v);
  putchar('\n');
}
