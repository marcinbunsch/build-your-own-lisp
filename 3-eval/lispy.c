#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../shared/mpc.h"

#include <editline/readline.h>

long eval_op(long x, char *operator, long y) {
  if (strcmp(operator, "+") == 0) return x + y;
  if (strcmp(operator, "-") == 0) return x - y;
  if (strcmp(operator, "*") == 0) return x * y;
  if (strcmp(operator, "/") == 0) return x / y;
  return 0;
}

long eval(mpc_ast_t *t) {

  // if tagged as number, return directly
  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }

  // the operator is always the second child
  char *operator = t->children[1]->contents;

  // store the third child in x
  long x = eval(t->children[2]);

  // iterate the remaining children and combining
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, operator, eval(t->children[i]));
    i++;
  }

  return x;
}

int main(int argc, char **argv) {

  char *input;
  int exit;
  mpc_result_t ast;
  long result;

  // Print version and exit information
  puts("Lispy version 0.0.0.0.1");
  puts("Press ctrl-c to exit\n");

  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Operator = mpc_new("operator");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Lispy = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
    "number : /-?[0-9]+/ ; \
     operator : '+' | '-' | '*' | '/' ; \
     expr : <number> | '(' <operator> <expr>+ ')' ; \
     lispy : /^/ <operator> <expr>+ /$/ ; \
    ", Number, Operator, Expr, Lispy);

  // In a never ending loop
  while(1) {
    // output our prompt
    input = readline("lispy> ");

    exit = strcmp(input, "exit");

    if (exit == 0) {
      puts("Bye!");
      break;
    }

    add_history(input);

    if (mpc_parse("<stdin>", input, Lispy, &ast)) {
      result = eval(ast.output);
      printf("%li\n", result);
      mpc_ast_delete(ast.output);
    } else {
      // Otherwise, print the error
      mpc_err_print(ast.error);
      mpc_err_delete(ast.error);
    }

    free(input);

  }

  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  return 0;
}
