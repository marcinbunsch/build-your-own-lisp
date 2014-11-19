#include "lenv.h"

lenv* lenv_new(void) {
  lenv* env = malloc(LENV_SIZE);
  env->count = 0;
  env->syms = NULL;
  env->vals = NULL;
  return env;
}

void lenv_del(lenv* env) {
  for (int i = 0; i < env->count; i++) {
    free(env->syms[i]);
    lval_del(env->vals[i]);
  }
  free(env->syms);
  free(env->vals);
  free(env);
}

lval* lenv_get(lenv* env, lval* key) {
  for (int i = 0; i < env->count; i++) {
    // if the symbol matches, return the copy of the value
    if (strcmp(env->syms[i], key->sym) == 0) {
      return lval_copy(env->vals[i]);
    }
  }
  // if no symbol was found, return error
  return lval_err("unbound symbol");
}

void lenv_put(lenv* env, lval* key, lval* value) {
  for (int i = 0; i < env->count; i++) {
    if (strcmp(env->syms[i], key->sym) == 0) {
      lval_del(env->vals[i]);
      env->vals[i] = lval_copy(value);
      return; // we're done, update complete
    }
  }

  // we found nothing, make space for new entry
  env->count++;
  env->vals = realloc(env->vals, sizeof(lval*) * env->count);
  env->syms = realloc(env->syms, sizeof(char*) * env->count);

  // copy contents of lval and symbol string into new location
  env->vals[env->count-1] = lval_copy(value);
  env->syms[env->count-1] = malloc(strlen(key->sym)+1);
  strcpy(env->syms[env->count-1], key->sym);
}

void lenv_add_builtin(lenv* env, char* name, lbuiltin func) {
  lval* key = lval_sym(name);
  lval* value = lval_fun(func);
  lenv_put(env, key, value);
  lval_del(key);
  lval_del(value);
}

void lenv_add_builtins(lenv* env) {
  // list functions
  lenv_add_builtin(env, "list", builtin_list);
  lenv_add_builtin(env, "head", builtin_head);
  lenv_add_builtin(env, "tail", builtin_tail);
  lenv_add_builtin(env, "eval", builtin_eval);
  lenv_add_builtin(env, "join", builtin_join);


  // mathematical functions
  lenv_add_builtin(env, "+", builtin_add);
  lenv_add_builtin(env, "-", builtin_subtract);
  lenv_add_builtin(env, "*", builtin_multiply);
  lenv_add_builtin(env, "/", builtin_divide);

}

