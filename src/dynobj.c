
#ifndef DYNOBJ_C
#define DYNOBJ_C

//imports HashedKeyStore
#include "./hashedkeystore.c"

#include "./murmurhash/murmurhash.c"

//imports lln
#include "./linkedlist.c"

//imports KeyValuePair
#include "./kvpair.c"

//stdbool.h is nice, but lets try and use 0 dependencies
#include "./boolean.h"

//A key store for DynObj - may be null
//Use DynObj_GetHashedKeyStore () unless you really know
//what you're doing (I don't, and I wrote this stuff)
HashedKeyStoreP DynObj_HashedKeyStore;

//typing "struct DynObj *" all the time gets old
#define DynObjP struct DynObj *

struct DynObj {
  //linked list of KeyValuePair - may be null
  llnp properties;
  //a cached value so we don't iterate the linked list constantly
  int propertyCount;
  //this needs to be updated when properties are modified
  bool propertyCountDirty;

  //set a value by its key (capable of runtime append)
  bool (*set)(DynObjP obj, char * key, void * value);
  //get a value by its key
  void * (*get)(DynObjP obj, char * key);

  void * (*getByHash)(DynObjP obj, int keyhash);
};

/**Tests if obj is null
 * 
 * Yes its wordy, but its explicit
 * Same as obj == 0
 */
bool DynObj_isNull (DynObjP obj) {
  return obj == 0;
}

/**Mark an object as needing property count to be recalculated and cached
 * 
 * This should be called automatically by built in functions for DynObj
 * However, if your code modifies properties, its a good idea to use this
 * 
 * Returns true aka 1 if success, 0 if obj is null aka 0
 */
bool DynObj_markPropertyCountDirty (DynObjP obj) {
  if (DynObj_isNull(obj)) return false;
  obj->propertyCountDirty = true;
  return true;
}

/**Get a count of properties in the object
 * 
 * If obj is null aka 0, result is -1
 * If obj->properties is null aka 0, result is 0
 * 
 * If obj->propertyCountDirty then its cached value will be recalculated
 */
int DynObj_getPropertyCount (DynObjP obj) {
  if (DynObj_isNull(obj)) return -1;

  if (obj->propertyCountDirty) {
    llnp current = obj->properties;
    
    int result = 0;
    while (current != 0) {
      result ++;
    }
    obj->propertyCount = result;
    obj->propertyCountDirty = false;
    return result;
  } else {
    return obj->propertyCount;
  }
}

/**Check that a property index is valid
 * 
 * Validity is such that index is greater than -1 and less than propertyCount
 */
bool DynObj_getPropertyIndexValid (DynObjP obj, int index) {
  //null check isn't necessary since index cannot be smaller and greater than -1
  //see getPropertyCount
  // if (DynObj_isNull(obj)) return false;
  return index < DynObj_getPropertyCount(obj) && index > -1;
}

llnp DynObj_getPropertyLinkAtIndex (DynObjP obj, int index) {
  //Null check isn't necessary, see getPropertyIndexValid
  if (!DynObj_getPropertyIndexValid(obj, index)) return 0;

  int indexer = 0;
  llnp current = obj->properties;
  while (current != 0) {
    if (indexer == index) return current;
    indexer ++;
    current = current->next;
  }

  //This should happen..
  return 0;
}

/**Gets the property at an index in obj's properties linked list
 * 
 * If the index is not valid (beyond length of list or bellow 0)
 * null aka 0 is returned
 * 
 * Warning, value may still be null, as properties are allowed to have null values
 */
KeyValuePairP DynObj_getPropertyAtIndex (DynObjP obj, int index) {
  //no need to do bounds checks since LinkAtIndex does this
  llnp link = DynObj_getPropertyLinkAtIndex(obj, index);

  if (link == 0) return 0;
  //value may legally be null
  return link->value;
}

/**Get a KeyValuePair pointer by its keyHash
 * 
 * Returns null aka 0 if not found
 */
KeyValuePairP DynObj_getPropertyByHash (DynObjP obj, int keyHash) {
  if (DynObj_isNull(obj)) return 0;
  
  llnp current = obj->properties;
  KeyValuePairP currentPair;

  while (current != 0) {
    currentPair = current->value;
    //null check and keyHash check
    if (currentPair != 0 && currentPair->keyHash == keyHash) return currentPair;

    current = current->next;
  }

  return 0;
}

//========Implement a callback for HashedKeyStore
//Which bridges that gaps between DynObj, HashedKeyStore, and murmurhash
#define DynObj_HashedKeyStore_seed 0

int impl_DynObj_HashedKeyStore_hashCallback (char * key) {
  //Simply call murmurhash in this case
  //This can be swapped out as long as we supply a char * and acquire an int
  //Also, we really don't want hash collisions
  //either swap the algorithm, or set the seed to something else
  return murmurhash(key, (uint32_t) strlen(key), DynObj_HashedKeyStore_seed);
}

//Access the key store, which will ensure it isn't null
HashedKeyStoreP DynObj_GetHashedKeyStore () {
  //If the key store for DynObj lib is null
  if (DynObj_HashedKeyStore == 0) {
    //Instantiate it
    DynObj_HashedKeyStore = HashedKeyStore_create(
      //Using a callback that implements the hashing math
      impl_DynObj_HashedKeyStore_hashCallback
    );
  }
  return DynObj_HashedKeyStore;
}

/**Get a hash of a string key
 */
int DynObj_getHashForKey (char * key) {
  HashedKeyStoreP store = DynObj_GetHashedKeyStore();
  return store->getHash(store, key);
}

KeyValuePairP DynObj_getPropertyByKey (DynObjP obj, char * key) {
  return DynObj_getPropertyByHash(
    obj,
    DynObj_getHashForKey(key)
  );
}

void * DynObj_getByHash (DynObjP obj, int keyhash) {
  KeyValuePairP pair = DynObj_getPropertyByHash(obj, keyhash);
  if (pair == 0) return 0;
  return pair->valuePointer;
}

void * DynObj_get (DynObjP obj, char * key) {
  KeyValuePairP pair = DynObj_getPropertyByKey(obj, key);
  if (pair == 0) return 0;
  return pair->valuePointer;
}

/**Set a property on an object
 * 
 * This works for keys that don't exist yet
 * 
 * Returns false aka 0 if memory could not allocate for any reason
 */
bool DynObj_set (DynObjP obj, char * key, void * value) {
  if (DynObj_isNull(obj)) return false;

  KeyValuePairP pair = DynObj_getPropertyByKey(obj, key);
  if (pair == 0) {
    pair = KeyValuePair_create(
      DynObj_getHashForKey(key),
      value
    );

    //its possible _create ran out of memory
    if (pair == 0) return 0;

    //If object has no properties, we need to create the linked list start
    if (obj->properties == 0) {
      //create the first link
      llnp first = lln_create();

      //if we couldn't allocate for the link return 0
      if (first == 0) return 0;

      //set the links value to the new key value pair
      first->value = pair;

      //set object's properties to the first link
      obj->properties = first;
    } else {
      //append the new key value pair to the properties linked list
      lln_add_value(obj->properties, pair);
    }

    //make sure property count knows to update
    DynObj_markPropertyCountDirty(obj);
  } else {

    //Modifying values doesn't change property count
    pair->valuePointer = value;
  }
  return true;
}

/**Create a dynamic object
 * 
 * Returns null aka 0 if object couldn't be allocated
 */
DynObjP DynObj_create () {
  DynObjP result = malloc(sizeof(struct DynObj));

  if (DynObj_isNull(result)) return 0;

  result->properties = 0;
  result->propertyCount = 0;
  result->propertyCountDirty = true;

  result->set = &DynObj_set;
  result->getByHash = &DynObj_getByHash;
  result->get = &DynObj_get;
  return result;
}

#endif
