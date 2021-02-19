
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string.h>

#include "./keytable.c"
#include "./dynobj.c"

int main(int argc, const char **argv) {

  printf("Hello world\n\n\n");

  //dynobj uses hashes of keys to save memory and complexity
  //keytable makes it easy to calculate, cache, and reverse lookup hashes from keys
  
  //a table to store object keys and their hashes
  struct keytable * kt = keytable_create();

  struct dynobj * parent = object_create(kt);
  object_set_property(parent, "name", "the parent", type_cstr, true, kt);

  struct dynobj * child = object_create(kt);
  object_set_property(child, "name", "the child", type_cstr, true, kt);
  object_set_property(child, "parent", parent, type_pointer_dynobj, true, kt);

  object_set_property(parent, "child", child, type_pointer_dynobj, true, kt);

  object_print_json(kt, parent, 0, 0);
  printf("\n\ndone\n");

  return 0;
}
