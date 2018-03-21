/*
 * DeltaList.cpp
 *
 *  Created on: Feb 2, 2018
 *      Author: c1moore
 */

#ifndef _SL_SCHEDULER_DELTALIST_IMPLEMENTATION
  #define _SL_SCHEDULER_DELTALIST_IMPLEMENTATION

  #include "../scheduler/DeltaList.h"

  #ifndef NULL
    #define NULL nullptr
  #endif
  
  template<class T>
  class DeltaList<T>::Implementation {
    public:
      class DeltaNode;

      DeltaNode *HEAD;  // The head of the DeltaList.

      Implementation() {
        HEAD = new DeltaNode(NULL);
      }

      /**
       * DeltaNode is a node within the DeltaList that holds the item
       * as well as a pointer to the next node in the list. Creating a
       * new class, DeltaNode, instead of adding the `next` pointer to
       * the DeltaItem class was chosen for better encapsulation of
       * the DeltaList implementation.
       */
      class DeltaNode {
        public:
          DeltaNode *next;    // Next DeltaNode in the DeltaList.
          DeltaItem<T> *item; // The item stored in the list.

          /**
           * Creates a new DeltaNode with `item` initialized with
           * the specified DeltaItem and `next` set to NULL.
           *
           * @param item (DeltaItem<T> *) - a pointer to the DeltaItem
           */
          DeltaNode(DeltaItem<T> *item): item(item) {
            next = NULL;
          }
      };
  };

  template<class T>
  DeltaList<T>::DeltaList() {
    implementation = new Implementation();
  }

  template<class T>
  DeltaList<T>::~DeltaList() {
    delete implementation;
  }

  template<class T>
  void DeltaList<T>::insert(const T item, int delta0) {
    typename Implementation::DeltaNode *node = new typename Implementation::DeltaNode(new DeltaItem<T>(item));
    typename Implementation::DeltaNode *current = implementation->HEAD;

    int delta = delta0;

    while(current->next && current->item->delta <= delta) {
      delta -= current->item->delta;

      current = current->next;
    }

    node->next = current->next;
    current->next = node;
    node->item->delta = delta;

    if(node->next) {
      node->next->item->delta -= delta;
    }
  }

  template<class T>
  void DeltaList<T>::decrement(const int value) {
    implementation->HEAD->next->item->delta--;
  }

  template<class T>
  const DeltaItem<T> DeltaList<T>::peek() const {
    if(!implementation->HEAD->next) {
      return DeltaItem<T>(T(), -1);
    }

    return *implementation->HEAD->next->item;
  }

  template<class T>
  const T DeltaList<T>::remove() {
    typename Implementation::DeltaNode *first = implementation->HEAD->next;

    implementation->HEAD->next = first->next;

    T value = first->item->value;

    delete first->item;
    delete first;

    return value;
  }

  template<class T>
  bool DeltaList<T>::isEmpty() const {
    return (implementation->HEAD->next != NULL);
  }
#endif /* _SL_SCHEDULER_DELTALIST_IMPLEMENTATION */
