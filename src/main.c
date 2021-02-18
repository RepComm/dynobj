
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string.h>

#include "./keytable.c"
#include "./dynobj.c"

int main(int argc, const char **argv) {

  printf("Hello world\n");

  //dynobj uses hashes of keys to save memory and complexity
  //keytable makes it easy to calculate, cache, and reverse lookup hashes from keys
  
  //a table to store object keys and their hashes
  struct keytable * kt = keytable_create();

  struct dynobj * myobj = object_create(kt);

  object_set_property(myobj, "sup_dudes", "a string of text", type_nstr, true, kt);

  object_print_json(kt, myobj);

  return 0;
}
