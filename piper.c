#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct pipe_child {
  char path[PATH_MAX];
  char* arguments[256];
  __pid_t pid;
  struct pipe_child* next;
} pipe_child;

int pipe_children(pipe_child* first)
{
  // Make all required pipes before spwaning children incase of error.
  pipe_child* child = first;
  int num_children = 0;
  while (child) {
    num_children++;
    child = child->next;
  }

  int pipes[num_children - 1][2];

  int current_pipe;
  for (current_pipe = 0; current_pipe < num_children - 1; current_pipe++) {
    if (pipe(pipes[current_pipe])) {
      // Error,
      // Close previously made pipes
      while (current_pipe > 0) {
        current_pipe--;
        close(pipes[current_pipe][0]);
        close(pipes[current_pipe][1]);
      }
      return -1;
    }
  }

  // Spawn children and connect pipes
  child = first;
  current_pipe = 0;
  while (child) {
    printf("---- child: %s\n", child->path);

    child->pid = fork();

    switch (child->pid) {
      case -1:
        for (current_pipe = 0; current_pipe < num_children - 1; current_pipe++) {
          close(pipes[current_pipe][0]);
          close(pipes[current_pipe][1]);
        }
        return -1;
      case 0:  // child
        printf("---- %s - current_pipe %d\n", child->path, current_pipe);
        if (child == first) {
          dup2(pipes[current_pipe][1], STDOUT_FILENO);
        }
        else if (child->next == nullptr) {
          dup2(pipes[current_pipe - 1][0], STDIN_FILENO);
        }
        else {
          dup2(pipes[current_pipe - 1][0], STDIN_FILENO);
          dup2(pipes[current_pipe][1], STDOUT_FILENO);
        }

        // Close pipes in order to not block other children. (dup'd pipes will remain)
        for (current_pipe = 0; current_pipe < num_children - 1; current_pipe++) {
          close(pipes[current_pipe][0]);
          close(pipes[current_pipe][1]);
        }

        execvp(child->path, child->arguments);

        // If we are here, something went wrong.
        perror("---- Problem with child");
        exit(EXIT_FAILURE);

      default:  // parent
        current_pipe++;
        break;
    }
    child = child->next;
  }

  for (current_pipe = 0; current_pipe < num_children - 1; current_pipe++) {
    close(pipes[current_pipe][0]);
    close(pipes[current_pipe][1]);
  }

  puts("---- Waiting for children");
  child = first;
  while (child) {
    waitpid(child->pid, nullptr, 0);
    printf("---- Child %s finished\n", child->path);
    child = child->next;
  }

  return 0;
}

pipe_child* add_child(pipe_child* prev, const char* path, const char** arguments)
{
  pipe_child* new = malloc(sizeof(pipe_child));

  strncpy(new->path, path, PATH_MAX - 1);
  for (int i = 0; arguments[i]; i++) {
    new->arguments[i] = (char*)arguments[i];
  }
  new->next = nullptr;

  if (prev) {
    prev->next = new;
  }

  return new;
}

int main(int argc, char* argv[])
{
  pipe_child* children;
  pipe_child* next;

  const char* ls_args[] = {"ls", "-l", nullptr};
  children = add_child(nullptr, "ls", ls_args);

  const char* tac_args[] = {"tac", nullptr};
  next = add_child(children, "tac", tac_args);

  const char* tee_args[] = {"tee", "pipe-dump", nullptr};
  next = add_child(next, "tee", tee_args);

  const char* lolcat_args[] = {"lolcat", nullptr};
  next = add_child(next, "lolcat", lolcat_args);

  if (pipe_children(children)) {
    perror("uh oh");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
