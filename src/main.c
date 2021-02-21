
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./jsonlexer.c"
#include "./dynobj.c"

int main(int argc, const char **argv) {
  char * mystr = "{ \"key1\" : \"1e0\", \"key2\" : 2, \"key3\" : \"three\", \"key4\" : 4 }";

  struct scan_json_object_result * res = scan_json_object(mystr, 0);

  struct dynobj * reso = res->value;

  object_print_json(reso, 0, 0);

  printf("\n\ndone\n");

  return 0;
}
