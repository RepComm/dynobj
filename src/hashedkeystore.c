
#ifndef HASHEDKEYSTORE_C
#define HASHEDKEYSTORE_C

#include <stdlib.h>

#include "./linkedlist.c"

#include "./boolean.h"

#define HashedKeyStoreP struct HashedKeyStore *

struct HashedKeyStore {
  int (*hashString)(char * str);
  //linked list of HashKeyPair
  llnp keys;
  int (*getHash)(HashedKeyStoreP store, char * key);
  char * (*lookupKey)(HashedKeyStoreP store, int hash);
};

struct HashKeyPair {
  int hash;
  char * key;
};
#define HashKeyPairP struct HashKeyPair *

HashKeyPairP HashKeyPair_create(int hash, char * key) {
  HashKeyPairP result = malloc (sizeof(struct HashKeyPair));
  result->hash = hash;
  result->key = key;
  return result;
}

llnp HashedKeyStore_getLinkByHash (HashedKeyStoreP store, int hash) {
  if (store == 0) return false;

  llnp current = store->keys;
  HashKeyPairP currentPair;

  while (current != 0) {
    currentPair = current->value;

    //null check and compare
    if (currentPair != 0 && currentPair->hash == hash) return current;

    current = current->next;
  }
  return 0;
}

HashKeyPairP HashedKeyStore_getPairByHash (HashedKeyStoreP store, int hash) {
  llnp link = HashedKeyStore_getLinkByHash(store, hash);
  if (link == 0 || link->value == 0) return 0;
  return link->value;
}

bool HashedKeyStore_hasHash (HashedKeyStoreP store, int hash) {
  return HashedKeyStore_getLinkByHash(store, hash) != 0;
}

/**This method is safe even if store has no keys linked list
 */
bool HashedKeyStore_setHash (HashedKeyStoreP store, int hash, char * key) {
  llnp link = HashedKeyStore_getLinkByHash(store, hash);
  HashKeyPairP pair;

  //Handle all edge cases here
  //if no pairs at all, there are no links in store
  if (link == 0) {
    //create the first link
    link = lln_create();
    //create the hash key pair
    pair = HashKeyPair_create(hash, key);
    //set pair as value of link
    link->value = pair;
    //set first key to be the link
    store->keys = link;
  } else {
    //if link exists the pair also exists because of getLinkByHash checking

    pair = link->value;
    pair->key = key;
  }
  return true;
}

/**Same as hashString, but will cache values for reverse lookup later
 * This is the correct way to get a hash from HashedKeyStore
 */
int HashedKeyStore_getHash (HashedKeyStoreP store, char * key) {
  int result = store->hashString(key);

  //cache the hash for later reverse lookup of key
  if (!HashedKeyStore_hasHash(store, result)) {
    HashedKeyStore_setHash(store, result, key);
  }
  return result;
}

/**Reverse lookup a key by its hash
 * This operation is somewhat expensive, so use it sparingly
 * 
 * Granted its probably still pretty fast by most standards
 * 
 * If the hash hasn't been recorded before, null aka 0 is returned
 */
char * HashedKeyStore_lookupKey (HashedKeyStoreP store, int hash) {
  HashKeyPairP pair = HashedKeyStore_getPairByHash(store, hash);
  if (pair == 0) return 0;
  return pair->key;
}

HashedKeyStoreP HashedKeyStore_create (int (*hashCallback)(char * str)) {
  HashedKeyStoreP result = malloc(sizeof(struct HashedKeyStore));
  result->hashString = hashCallback;
  result->getHash = &HashedKeyStore_getHash;
  result->lookupKey = &HashedKeyStore_lookupKey;
  return result;
}

#endif
