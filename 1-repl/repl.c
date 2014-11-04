#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>

int main(int argc, char** argv) {

  char *input;
  // Print version and exit information
  puts("Lispy version 0.0.0.0.1");
  puts("Press ctrl-c to exit\n");

  // In a never ending loop
  while(1) {
    // output our prompt
    input = readline("lispy> ");

    add_history(input);

    printf("No, you're a %s\n", input);

    free(input);

  }
  return 0;
}
