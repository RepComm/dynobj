
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynobj.c"
#include "dynobj.json.c"

void test_object_program () {
  printf("running test_object_program\n");

  //Create an object
  DynObjP myobj = DynObj_create();

  //set a property
  myobj->set(myobj, "name", "hello world", DynObj_Json_String);

  //get the property again
  KeyValuePairP pair = myobj->get(myobj, "name");

  //Always check the type of value
  //Which seems annoying
  //but you probably do this in JS too if your code doesn't suck
  if (pair->valueType == DynObj_Json_String) {

    //DynObj_Json_String is implemented as `char *` c-string
    //So we can just print it with %s
    printf("key 'name' had value '%s' \n", (char *)pair->valuePointer);

  }
}

void test_json_parse_program () {
  printf("running test_json_parse_program\n");

  char * jsonSource = "{ \"mykey\" : \"some value\", \"otherkey\" : 12, \"child\": { \"name\" : \"some dude\" } }";
  DynObjP jsonObj = DynObj_fromJson(jsonSource);

  printf("Object from json %p\n", jsonObj);
}

int main(int argc, char **argv) {
  
  test_object_program();

  printf("\n");

  test_json_parse_program();

  return 0;
}
