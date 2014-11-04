#include <stdio.h>

// Declare a buffer of user input of size 2048

static char input[2048];

int main(int argc, char** argv) {

  // Print version and exit information
  puts("Lispy version 0.0.0.0.1");
  puts("Press ctrl-c to exit\n");

  // In a never ending loop
  while(1) {
    // output our prompt
    fputs("lispy> ", stdout);

    // Read a line of used input of maximum size 2048
    fgets(input, 2048, stdin);

    // echo input back to the user
    printf("No, you're a %s", input);

  }
  return 0;
}