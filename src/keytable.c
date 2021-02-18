
/**Dynamic object keys tend to have lots of repeats
 * Why store all of those strings?
 * 
 * Key table matches hashes with strings
 * When a new key is created, its hash is calculated and stored in the table
 * 
 * The hashes are sorted, so looking them up for creation/addition is efficient
 * They are stored in a linked list, so it is dynamic
 */

#ifndef KEYTABLE_C
#define KEYTABLE_C
#include <stdbool.h>
#include <string.h>
#include "./murmurhash/murmurhash.c"

struct keynode {
  int hash; //4 bytes
  char * key; //4 bytes
  struct keynode * next; //4 bytes
};

struct keytable {
  int keyCount; //4 bytes
  bool isDirty; //4 bytes?
  struct keynode * first; //4 bytes
};

struct keytable * keytable_create () {
  struct keytable * result = malloc(sizeof(struct keytable));

  result->keyCount = 0;
  result->isDirty = true;
  result->first = 0;

  return result;
}

void _keytable_insert_keynode (struct keytable * table, struct keynode * kn) {
  //if the table is empty, just set the node as the first one
  if (table->first == 0) {
    table->first = kn;
    return;
  }

  struct keynode * current = table->first;

  //Find the node before our proposed one
	while (current->next != 0 && current->next->hash < kn->hash) {
		current = current->next;
  }

  //splice our node in
  //first, modify our node to point to the next
	kn->next = current->next;
  //then link our to the previous node
	current->next = kn;

  //mark for recalculating of keyCount
  table->isDirty = true;
  return;
}

//get the keynode before another
struct keynode * keytable_get_before (struct keytable * table, struct keynode * kn) {
  struct keynode * result = table->first;

  while (result != 0) {
    if (result->next == kn) return result;
    result = result->next;
  }
  return 0;
}

bool _keytable_remove_keynode (struct keytable * table, struct keynode * kn) {
  struct keynode * before = keytable_get_before(table, kn);
  struct keynode * after = kn->next;

  if (before != 0) {
    before->next = after;
    kn->next = 0;
    return true;
  }
  return false;
}

struct keynode * _keytable_get_by_index (struct keytable * table, int index) {
  struct keynode * result = table->first;

  int counter = 0;
  while (result != 0) {
    if (counter == index) return result;
    counter ++;
    result = result->next;
  }
  return 0;
}

/**Get or calculate keytable's key count
 * If calculated, will cache into table->keyCount
 */
int keytable_get_keycount (struct keytable * table) {
  int result = 0;

  if (table->isDirty) {
    struct keynode * current = table->first;

    while (current != 0) {
      result ++;
      current = current->next;
    }
    table->keyCount = result;
    table->isDirty = false;
  } else {
    result = table->keyCount;
  }
  return result;
}

/**Performs a binary search on the keytable to find a keynode with the given hash
 * This is how a feature such as JavaScript's Object.keys() could be implemented
 */
struct keynode * keytable_get_by_hash (struct keytable * table, int hash) {
  
  //TODO - fix binary search, its currently core dumping..
  
  // //table always maintains its key count
  // int size = keytable_get_keycount(table);

  // //--------BINARY SEARCH
  // int start = 0;
  // int end = size - 1;
  // int mid = 0;

  // struct keynode * current;

  // while(start <= end) {
  //   mid = (start + end) / 2;
  //   //test if the key is in the middle
  //   current = _keytable_get_by_index(table, mid);
  //   if (hash == current->hash) {
  //     return current;
  //   }
  //   if (hash < current->hash) {
  //     end = mid - 1;
  //   } else {
  //     start = mid + 1;
  //   }
  // }

  //slow but effective mode until its fixed
  struct keynode * current = table->first;
  while (current != 0) {
    if (current->hash == hash) return current;
    current = current->next;
  }

  //return null if couldn't find it after searching entire linked list
  return 0;
}

/**Internal function for generating the hash of a key
 * WARNING: This does not store the key or hash, only calculates the hash
 * This is meant for keytable.c internal use only
 */
int _keytable_hashkey (char* key) {
  int seed = 0;
  return murmurhash(key, (uint32_t) strlen(key), seed);
}

//if hash == 0 it will be calculated from the key
struct keynode * _keytable_create_keynode (struct keytable * table, char* key, int hash) {
  if (hash == 0) hash = _keytable_hashkey(key);
  struct keynode * result = malloc(sizeof(struct keynode));
  result->hash = hash;
  result->key = key;
  result->next = 0;
  _keytable_insert_keynode(table, result);
  return result;
}


/**Get the hash of a key
 * This will also store the key with its hash in the table
 * Looking up a key from a hash will use binary search as hashes are sorted
 */
struct keynode * keytable_get_hash (struct keytable * table, char* key) {
  //hash the key
  int hash = _keytable_hashkey(key);

  //try and see if the key has been used before
  struct keynode * result = keytable_get_by_hash(table, hash);

  //if not, lets stick it in the table
  if (result == 0) {
    result = _keytable_create_keynode(table, key, hash);
  }

  return result;
}

#endif
