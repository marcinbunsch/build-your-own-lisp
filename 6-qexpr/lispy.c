#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <editline/readline.h>

#include "../shared/mpc.h"
#include "lval.h"

int main(int argc, char **argv) {

  char *input;
  int exit;
  mpc_result_t ast;

  // Print version and exit information
  puts("Lispy version 0.0.0.0.1");
  puts("Press ctrl-c to exit\n");

  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Symbol = mpc_new("symbol");
  mpc_parser_t *Sexpr = mpc_new("sexpr");
  mpc_parser_t *Qexpr = mpc_new("qexpr");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Lispy = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
    "number : /-?[0-9]+/ ; \
     symbol : \"head\" | \"tail\" | '+' | '-' | '*' | '/' ; \
     sexpr : '(' <expr>* ')' ; \
     qexpr : '{' <expr>* '}' ; \
     expr : <number> | <symbol> | <sexpr> | <qexpr> ; \
     lispy : /^/ <expr>* /$/ ; \
    ", Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

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
      // result = eval(ast.output);
      lval* result = lval_eval(lval_read(ast.output));
      lval_println(result);
      lval_del(result);
      mpc_ast_delete(ast.output);
    } else {
      // Otherwise, print the error
      mpc_err_print(ast.error);
      mpc_err_delete(ast.error);
    }

    free(input);

  }

  mpc_cleanup(5, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
  return 0;
}
