#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int pipe_children(const char* first, const char* second)
{
  int pipe_fds[2];

  if (pipe(pipe_fds)) {
    return -1;
  }

  // First child
  int first_pid = fork();

  switch (first_pid) {
    case -1:
      return -1;
    case 0:  // child
      puts("---- First child");
      dup2(pipe_fds[1], STDOUT_FILENO);
      close(pipe_fds[0]);
      execlp(first, first, nullptr);
      // If we are here, something went wrong.
      perror("---- Problem with first child");
      exit(EXIT_FAILURE);

    default:  // parent
      break;
  }

  // Second child
  int second_pid = fork();

  switch (second_pid) {
    case -1:
      return -1;
    case 0:  // child
      puts("---- Second child");
      dup2(pipe_fds[0], STDIN_FILENO);
      close(pipe_fds[1]);
      execlp(second, second, nullptr);
      // If we are here, something went wrong.
      perror("---- Problem with second child");
      exit(EXIT_FAILURE);

    default:  // parent
      break;
  }

  close(pipe_fds[0]);
  close(pipe_fds[1]);

  puts("---- Waiting for children");
  waitpid(first_pid, nullptr, 0);
  puts("---- First done");
  waitpid(second_pid, nullptr, 0);
  puts("---- Second done");

  return 0;
}

int main(int argc, char* argv[])
{
  if (pipe_children("ls", "lolcat")) {
    perror("uh oh");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
