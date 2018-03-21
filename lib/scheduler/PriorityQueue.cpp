/*
 * PriorityQueue.cpp
 *
 *      Author: c1moore
 */

#ifndef _SL_SCHEDULER_PRIORITYQUEUE_IMPLEMENTATION
  #define _SL_SCHEDULER_PRIORITYQUEUE_IMPLEMENTATION

  #include "../scheduler/PriorityQueue.h"

  #ifndef NULL
    #define NULL nullptr
  #endif
  
  /**
   * A PriorityNode represents a node within the PriorityQueue.
   */
  template<class T>
  class PriorityNode {
    public:
      PriorityNode *next; // The next item in the queue.
      int priority;       // The priority of this item.
      T item;             // The item.

      /**
       * Creates a new PriorityNode initialized with default values.
       */
      PriorityNode() {
        next = NULL;
        priority = 0;
        item = T();
      }

      /**
       * Creates a new PriorityNode for the specified item with the
       * specified priority.
       *
       * @param item (T) - the item that will be inserted in the
       *  PriorityQueue
       * @param priority (int) - the priority of this item
       */
      PriorityNode(T item, int priority): item(item), priority(priority) {
        next = NULL;
      }
  };

  /**
   * Implementation details for the PriorityQueue.
   */
  template<class T>
  class PriorityQueue<T>::Implementation {
    public:
      PriorityNode<T> *HEAD;  // The head of the PriorityQueue.
      int count;              // The total number of items in the PriorityQueue.

      Implementation() {
        HEAD = new PriorityNode<T>();
        count = 0;
      }

      ~Implementation() {
        delete HEAD;
      }
  };

  template<class T>
  PriorityQueue<T>::PriorityQueue() {
    implementation = new Implementation();
  }

  template<class T>
  PriorityQueue<T>::~PriorityQueue() {
    delete implementation;
  }

  template<class T>
  void PriorityQueue<T>::enqueue(const T item, const int priority) {
    PriorityNode<T> *node = new PriorityNode<T>(item, priority);
    PriorityNode<T> *current = implementation->HEAD;

    while(current->next && current->priority >= priority) {
      current = current->next;
    }

    node->next = current->next;
    current->next = node;

    implementation->count++;
  }

  template<class T>
  T PriorityQueue<T>::dequeue() {
    PriorityNode<T> *removed = implementation->HEAD->next;
    T item = removed->item;

    implementation->HEAD->next = removed->next;
    delete removed;

    implementation->count--;

    return item;
  }

  template<class T>
  void PriorityQueue<T>::remove(const T item) {
    PriorityNode<T> *current = implementation->HEAD;

    while(current->next != NULL) {
      if(current->next->item != item) {
        current = current->next;

        continue;
      }

      PriorityNode<T> *removed = current->next;

      current->next = removed->next;

      delete removed;
      implementation->count--;
    }
  }

  template<class T>
  T PriorityQueue<T>::peek() const {
    if(isEmpty()) {
      return T();
    }

    return implementation->HEAD->next->item;
  }

  template<class T>
  bool PriorityQueue<T>::isEmpty() const {
    return !implementation->HEAD->next;
  }

  template<class T>
  int PriorityQueue<T>::count() const {
    return implementation->count;
  }
#endif /* _SL_SCHEDULER_PRIORITYQUEUE_IMPLEMENTATION */
