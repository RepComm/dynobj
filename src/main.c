
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./jsonlexer.c"
#include "./dynobj.c"

int main(int argc, const char **argv) {
  printf("Hello world\n\n\n");
  
  struct dynobj * parent = object_create();
  object_set_property(parent, "name", "the parent", type_cstr);

  struct dynobj * child = object_create();
  object_set_property(child, "name", "the child", type_cstr);
  object_set_property(child, "parent", parent, type_pointer_dynobj);

  object_set_property(parent, "child", child, type_pointer_dynobj);

  object_print_json(parent, 0, 0);

  printf("\n\n");
  char * mystr = "12.1e2 \"hello world\" extra stuff";
  
  // struct scan_result * res = scan_stringliteral(mystr, 0);
  struct scan_result * res = scan_numberliteral(mystr, 0);
  if (res->success) {
    printf("extracted %s", res->value);
  } else {
    printf("couldn't extract token");
  }

  printf("\n\ndone\n");

  return 0;
}
