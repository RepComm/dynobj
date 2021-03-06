
#ifndef KVPAIR_C
#define KVPAIR_C

struct KeyValuePair {
  //a hashed version of a string key
  int keyHash;
  //a pointer to the value
  void * valuePointer;
  //the type of value
  char valueType;
};

//pointer to a key value pair, same as "struct KeyValuePair *"
#define KeyValuePairP struct KeyValuePair *

/**Create a key value pair instance
 * Returns null if could not malloc
 */
KeyValuePairP KeyValuePair_create (int keyHash, void * valuePointer, char valueType) {
  KeyValuePairP result = malloc(sizeof(struct KeyValuePair));
  if (result == 0) return 0;
  result->keyHash = keyHash;
  result->valuePointer = valuePointer;
  result->valueType = valueType;
  return result;
}

#endif