
#ifndef LINKEDLIST_C
#define LINKEDLIST_C

struct linked_list_node {
  void * value;
  //the first node in the list (may be the same as current)
  struct linked_list_node * start;
  //the next node in the list
  struct linked_list_node * next;
};

struct linked_list_node * linked_list_create_node () {
  struct linked_list_node * link = (struct linked_list_node*) malloc(sizeof(struct linked_list_node));
  return link;
}

/**Iterate through the linked list with a callback function
 * node - a linked list node (usually the start aka tail)
 */
int linked_list_iterate ( struct linked_list_node * node, void (*cb)() ) {
  struct linked_list_node * cn = node->start;

  int counter = 0;

  while (&cn != 0x0) {
    (*cb)();
    counter++;
    cn = cn->next;
  }
  return counter;
}

struct linked_list_node * linked_list_get (struct linked_list_node * node, int index) {
  int counter = 0;

  struct linked_list_node * cn = node->start;

  for (int i=0; i<index; i++) {
    cn = cn->next;
  }

  return cn;
}

#endif
