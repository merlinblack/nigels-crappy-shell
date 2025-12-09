#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_delim(char c, bool insideQuotes, char quoteType)
{
  if (c == 0) {
    return true;
  }

  if (insideQuotes) {
    if (c == quoteType) {
      return true;
    }
  }
  else {
    if (c == ' ') {
      return true;
    }
    if (c == '\'') {
      return true;
    }
    if (c == '"') {
      return true;
    }
  }

  return false;
}

void delimit(char* str)
{
  int pos = 0;
  bool insideQuotes = false;
  char quoteType;

  while (str[pos]) {
    while (is_delim(str[pos], insideQuotes, quoteType) == false) {
      pos++;
    }

    if (str[pos] == ' ') {
      str[pos++] = 0;
      continue;
    }

    // We have a quote char.
    if (insideQuotes) {
      str[pos++] = 0;
      insideQuotes = false;
      continue;
    }
    else {
      quoteType = str[pos];
      str[pos++] = 0;
      insideQuotes = true;
      continue;
    }
  }

  return;
}

#ifdef TEST_DELIMIT
int main(int argc, char* argv[])
{
  char* test[] = {
      "echo Hello world Bob was here",
      "echo 'Hello world' 'Bob was here'",
      "echo Hello world 'Bob was here'",
      "echo \"Hello 'world'\" Bob was here",
      "echo",
  };

  for (int i = 0; i < 5; i++) {
    printf("Test %d - <%s>\n", i, test[i]);

    char* str = malloc(strlen(test[i]) + 1);
    strcpy(str, test[i]);

    int length = strlen(str);

    delimit(str);

    int i = 0;
    while (i <= length) {
      puts(&str[i]);
      while (str[i] && i <= length) {
        i++;
      }
      while (!str[i] && i <= length) {
        i++;
      }
    }

    free(str);
  }

  return EXIT_SUCCESS;
}
#endif
