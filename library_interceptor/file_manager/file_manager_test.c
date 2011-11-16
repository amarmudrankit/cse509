#include <stdio.h>
#include "file_manager.h"

int main(int argc, char *argv[]) {
  fm_get_alt_file("/tmp/1/2");
  fm_get_alt_file("/tmp/1/2");
  fm_get_alt_file("/tmp/1/3");
  return 0;
}
