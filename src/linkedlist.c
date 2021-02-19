
#ifndef LINKEDLIST_C
#define LINKEDLIST_C

struct lln {
  void * value;
  //the first node in the list (may be the same as current)
  struct lln * start;
  //the next node in the list
  struct lln * next;
};

struct lln * lln_create () {
  struct lln * link = malloc(sizeof(struct lln));
  link->start = link;
  link->next = 0;
  link->value = 0;
  return link;
}

void lln_add (struct lln * parent, struct lln * child) {
  parent->next = child;
  child->start = parent->start;
}

struct lln * lln_get_tail (struct lln * parent) {
  struct lln * current = parent->start;
  while (current->next != 0) {
    current = current->next;
  }
  return current;
}

struct lln * lln_add_value (struct lln * parent, void * value) {
  struct lln * node = lln_create();
  node->value = value;

  lln_add(lln_get_tail(parent), node);

  return node;
}

bool lln_has (struct lln * list, struct lln * search) {
  struct lln * current = list->start;

  while (current != 0) {
    if (current == search) return true;
    current = current->next;
  }
  return false;
}

bool lln_has_value (struct lln * list, void * value) {
  struct lln * current = list->start;

  while (current != 0) {
    if (current->value == value) return true;
    current = current->next;
  }
  return false;
}

/**Iterate through the linked list with a callback function
 * node - a linked list node (usually the start aka tail)
 */
int lln_iterate ( struct lln * node, void (*cb)() ) {
  struct lln * cn = node->start;

  int counter = 0;

  while (cn != 0x0) {
    (*cb)();
    counter++;
    cn = cn->next;
  }
  return counter;
}

#endif
