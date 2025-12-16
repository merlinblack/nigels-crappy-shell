#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int pipe_children(const char* first, const char* second, const char* third)
{
  int pipe_fds[2];
  int pipe_a_read, pipe_a_write;
  int pipe_b_read, pipe_b_write;

  if (pipe(pipe_fds)) {
    return -1;
  }

  pipe_a_read = pipe_fds[0];
  pipe_a_write = pipe_fds[1];

  if (pipe(pipe_fds)) {
    return -1;
  }

  pipe_b_read = pipe_fds[0];
  pipe_b_write = pipe_fds[1];

  // First child
  int first_pid = fork();

  switch (first_pid) {
    case -1:
      return -1;
    case 0:  // child
      puts("---- First child");
      dup2(pipe_a_write, STDOUT_FILENO);
      close(pipe_a_read);
      close(pipe_a_write);
      close(pipe_b_read);
      close(pipe_b_write);
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
      dup2(pipe_a_read, STDIN_FILENO);
      dup2(pipe_b_write, STDOUT_FILENO);
      close(pipe_a_read);
      close(pipe_a_write);
      close(pipe_b_read);
      close(pipe_b_write);
      execlp(second, second, nullptr);
      // If we are here, something went wrong.
      perror("---- Problem with second child");
      exit(EXIT_FAILURE);

    default:  // parent
      break;
  }

  // Third child
  int third_pid = fork();

  switch (third_pid) {
    case -1:
      return -1;
    case 0:  // child
      puts("---- Second child");
      dup2(pipe_b_read, STDIN_FILENO);
      close(pipe_a_read);
      close(pipe_a_write);
      close(pipe_b_read);
      close(pipe_b_write);
      execlp(third, third, nullptr);
      // If we are here, something went wrong.
      perror("---- Problem with second child");
      exit(EXIT_FAILURE);

    default:  // parent
      break;
  }

  close(pipe_a_read);
  close(pipe_a_write);
  close(pipe_b_read);
  close(pipe_b_write);

  puts("---- Waiting for children");
  waitpid(first_pid, nullptr, 0);
  puts("---- First done");
  waitpid(second_pid, nullptr, 0);
  puts("---- Second done");
  waitpid(third_pid, nullptr, 0);
  puts("---- Third done");

  return 0;
}

int main(int argc, char* argv[])
{
  if (pipe_children("ls", "tac", "lolcat")) {
    perror("uh oh");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
