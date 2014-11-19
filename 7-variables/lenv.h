#include "lval.h"

struct lenv {
  int count;
  char** syms;
  lval** vals;
};

static const int LENV_SIZE = sizeof(lenv);

lenv* lenv_new(void);
void lenv_del(lenv* env);
lval* lenv_get(lenv* env, lval* key);
void lenv_put(lenv* env, lval* key, lval* value);
void lenv_add_builtin(lenv* env, char* name, lbuiltin func);
void lenv_add_builtins(lenv* env);
