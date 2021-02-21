
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./jsonlexer.c"
#include "./dynobj.c"
#include "./utils.c"

int main(int argc, char **argv) {
  char * mystr = string_concat(argv, 1, argc, " ");
  printf("parsing %s as json\n", mystr);
  // char * mystr = "{ \"key1\" : \"1e0\", \"key2\" : { \"key3\" : 3, \"key4\" : { \"key5\" : \"hello world\" } } }";

  struct dynobj * myobj = object_from_json(mystr);

  object_print_json(myobj, 0, 0);

  printf("\n\ndone\n");

  return 0;
}
