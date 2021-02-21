
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

  // object_print_json(parent, 0, 0);

  printf("\n\n");
  char * mystr = "{ \"key\" : 12.1e2, \"key2\" : 11.1e3 }";

  struct scan_json_object_result * res = scan_json_object(mystr, 0);

  struct dynobj * reso = res->value;

  object_print_json(reso, 0, 0);

  printf("\n\ndone\n");

  return 0;
}
