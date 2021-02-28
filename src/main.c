
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynobj.c"

int main(int argc, char **argv) {
  
  //Create an object
  DynObjP myobj = DynObj_create();

  //set a property
  myobj->set(myobj, "name", "hello world");

  //get the property again
  void * value = myobj->get(myobj, "name");

  //get the value as text (because the value could be anything)
  char * valueAsText = (char *) value;

  //print text in terminal
  printf(" %s ", valueAsText);


  return 0;
}
