

#ifndef DYNOBJ_C
#define DYNOBJ_C

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "./keytable.c"
#include "./linkedlist.c"
#include "./jsonlexer.c"

struct keytable * DYNOBJ_KT;

struct dynobj {
  int keyCount;
  bool isDirty;
  struct linkedkvpair * first;
};

struct linkedkvpair {
  //string key of the value (actually a hash of the key)
  int keyhash; //4 bytes

  //type of the value to expect
  char type; //1 byte

  //pointer to the value
  void* value; //4 bytes

  //link to next kv pair
  struct linkedkvpair * next;
};

struct linkedkvpair * object_create_kvpair (char* key, char type) {
  struct linkedkvpair * result = (struct linkedkvpair*)  malloc(sizeof(struct linkedkvpair));
  
  //get the hash from the keytable (makes key retrievable, and also efficient for reused keys)
  // result->keyhash = keytable_get_hash(kt, key)->hash;
  result->keyhash = keytable_get_hash(DYNOBJ_KT, key)->hash;
  result->type = type;
  result->next = 0x0; //init to 0 so we don't have weird results
  result->value = 0x0;
  return result;
}

enum dynobj_value_type {
  //a pointer to another dynobj
  type_pointer_dynobj,

  //a pointer to an unknown type, aka void *
  type_pointer_void,

  //a pointer to a C function
  type_pointer_function,
  type_uint8,
  type_uint16,
  type_uint32,
  type_int8,
  type_int16,
  type_int32,
  type_double,
  type_float,
  //c style string, ends in 0x00
  type_cstr
};

char* dynobj_value_type_to_string (char type) {
  switch (type) {
    case type_pointer_dynobj:
      return "dynobj";
    case type_pointer_function:
      return "function";
    case type_pointer_void:
      return "any";
    case type_uint8:
      return "uint8";
    case type_uint16:
      return "uint16";
    case type_uint32:
      return "uint32";
    case type_int8:
      return "int8";
    case type_int16:
      return "int16";
    case type_int32:
      return "int32";
    case type_cstr:
      return "string";
    case type_float:
      return "float";
    case type_double:
      return "double";
    default:
      return "unknown";
  }
}

char string_to_dynobj_value_type (char* typestr) {
  if (strcmp(typestr, "dynobj") == 0) return type_pointer_dynobj;
  if (strcmp(typestr, "function") == 0) return type_pointer_function;
  if (strcmp(typestr, "any") == 0) return type_pointer_void;
  if (strcmp(typestr, "uint8") == 0) return type_uint8;
  if (strcmp(typestr, "uint16") == 0) return type_uint16;
  if (strcmp(typestr, "uint32") == 0) return type_uint32;
  if (strcmp(typestr, "int8") == 0) return type_int8;
  if (strcmp(typestr, "int16") == 0) return type_int16;
  if (strcmp(typestr, "int32") == 0) return type_int32;
  if (strcmp(typestr, "string") == 0) return type_cstr;
  if (strcmp(typestr, "float") == 0) return type_float;
  if (strcmp(typestr, "double") == 0) return type_double;
  return -1;
}

struct linkedkvpair * object_get_property (struct dynobj * obj, char* key) {
  /**Using internal method to directly hash the key instead of caching it
   * This is less expensive, and its ok to use it here because:
   * 1. the key is likely already calculated / cached or
   * 2. the key shouldn't be cached because of runtime errors generating bad keys
   */
  int keyhash = _keytable_hashkey(key);

  struct linkedkvpair * cn = obj->first;

  //iterate the linked list of key values
  while (cn != 0x0) {
    if (cn->keyhash == keyhash) return cn;
    cn = cn->next;
  }

  return 0x0;
}

bool object_has_property (struct dynobj * obj, char* key) {
  struct linkedkvpair * n = object_get_property(obj, key);
  return n != 0x0;
}

struct linkedkvpair * object_create_property (struct dynobj * obj, char* key, void* value, char type) {
  struct linkedkvpair * n = object_create_kvpair(key, type);
  // struct linkedkvpair * n = object_create_kvpair(kt, key, type);
  n->value = value;
  
  //If object has no property, set the new one as the first one
  if (obj->first == 0) {
    obj->first = n;
    obj->keyCount = 1;
    obj->isDirty = false;
    return n;
  }

  //while we're here, might as well recalculate the prop count!
  int count = 1;

  //otherwise stick it onto the end
  struct linkedkvpair * last = obj->first;
  while (last->next != 0) {
    last = last->next;
    count ++;
  }

  obj->keyCount = count;
  obj->isDirty = false;

  last->next = n;
  n->next = 0;
  return n;
}

/**Set a property on the object obj
 * If the property given by key doesn't exist:
 * createIfNull == true will create the property (NOTE: keytable kt must not be null in this case)
 * otherwise return false, no property is set
 * 
 * If createIfNull == false, keytable kt can be set to null / 0 / 0x0 safely
 */
bool object_set_property (struct dynobj * obj, char* key, void* value, char type) {
  struct linkedkvpair * n = object_get_property(obj, key);

  if (n == 0) {
    n = object_create_property(obj, key, value, type);
    return true;
  }

  n->value = value;
  n->type = type;

  return true;
}

void object_set_dirty (struct dynobj * obj, bool dirty) {
  obj->isDirty = dirty;
}

//gets or calculates object prop count
int object_get_property_count (struct dynobj * obj) {
  int result = 0;

  if (obj->isDirty) {
    struct linkedkvpair * cn = obj->first;

    int counter = 0;
    //iterate the linked list of key values
    while (cn != 0x0) {
      counter ++;
      cn = cn->next;
    }
    result = counter;
    obj->keyCount = result;
    obj->isDirty = false;
  } else {
    result = obj->keyCount;
  }

  return result;
}

// char* object_to_jsonstr (struct dynobj * obj) {
//   int count = object_get_property_count(obj);
  
// }

void object_print_json (struct dynobj * obj, int level, struct lln * visited) {
  level ++;
  struct linkedkvpair * current = obj->first;

  struct keynode * currentKey;

  if (visited == 0) {
    // printf("visited was 0, initializing\n");
    visited = lln_create();
    // printf("initialized lln as %p", visited);
  }

  printf("{\n");

  while (current != 0) {
    lln_add_value(visited, current);

    currentKey = keytable_get_by_hash(DYNOBJ_KT, current->keyhash);
    // currentKey = keytable_get_by_hash(kt, current->keyhash);

    for (int i=0; i<level; i++) {
      printf("  ");
    }
    printf("\"%s\"", currentKey->key);
    printf(" : ");

    if (current->type == type_pointer_dynobj) {
      if (lln_has_value(visited, current->value)) {
        printf("\"[Object cyclic]\"");
      } else {
        lln_add_value(visited, current->value);
        if (current->value == current) {
          printf("\"[Object self]\"");
        } else {
          object_print_json((struct dynobj *) current->value, level, visited);
        }
      }
    } else if (current->type == type_cstr) {
      printf("\"%.*s\"", 45, (char *)current->value);
    } else if (current->type == type_pointer_function) {
      printf("[function]");
    }

    if (current->next != 0) printf(",\n");
    current = current->next;
  }

  printf("\n");
  for (int i=0; i<level-1; i++) {
    printf("  ");
  }

  printf("}");
}

struct dynobj * object_create () {
  if (DYNOBJ_KT == 0) DYNOBJ_KT = keytable_create();

  struct dynobj * result = malloc(sizeof(struct dynobj));

  result->keyCount = 0;
  result->isDirty = false;

  result->isDirty = true;

  return result;
}

// struct dynobj * object_from_json (char* src) {
//   int offset = 0;

//   scan_stringliteral(src, offset);

// }

#endif
