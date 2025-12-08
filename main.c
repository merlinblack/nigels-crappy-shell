// turn off header re-ordering as stdio.h needs to be before readline. Ufff.
// clang-format off
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
// clang-format on

#define DELIM " "

void cd_home()
{
  char* home = getenv("HOME");

  if (!home) {
    puts("HOME not set!!");
    return;
  }

  chdir(home);
}

void cd(const char* path)
{
  if (chdir(path)) {
    perror("There was a bit of an issue:");
  }
}

int main(int argc, char* argv[])
{
  puts("Nigels crappy shell.");

  bool shouldStop = false;
  char cwd[PATH_MAX];
  char* arguments[255];

  while (!shouldStop) {
    getcwd(cwd, PATH_MAX);
    puts(cwd);
    char* input = readline("NCS> ");

    if (strcmp(input, "exit") == 0) {
      free(input);
      shouldStop = true;
      puts("Ok, bye!");
      continue;
    }

    arguments[0] = strtok(input, DELIM);

    int i = 0;

    while ((arguments[++i] = strtok(nullptr, DELIM)))
      ;

    for (i = 0; arguments[i]; i++) {
      puts(arguments[i]);
    }

    if (strcmp(arguments[0], "cd") == 0) {
      if (arguments[1] == nullptr) {
        // No arguments
        cd_home();
      }
      else if (strcmp(arguments[1], "~") == 0) {
        cd_home();
      }
      else {
        cd(arguments[1]);
      }

      free(input);
      continue;
    }

    __pid_t pid = fork();

    switch (pid) {
      case 0:  // Child
        execvp(arguments[0], arguments);
        perror("Could not execute: ");
        exit(EXIT_FAILURE);

      case -1:  // Error
        perror("Could not forking fork.");
        break;

      default:
        waitpid(pid, nullptr, 0);
        break;
    }

    free(input);
  }
}
