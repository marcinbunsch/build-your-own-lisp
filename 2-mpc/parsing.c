#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../shared/mpc.h"

#include <editline/readline.h>

int main(int argc, char **argv) {

  char *input;
  int exit;
  mpc_result_t ast;

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
      // On success, print the AST
      mpc_ast_print(ast.output);
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
