#include "sketchybar.h"
#include <stdio.h>

int main (int argc, char *argv[]) {
  char* response = sketchybar("--query bar");
  printf("%s", response);
  return 0;
}
