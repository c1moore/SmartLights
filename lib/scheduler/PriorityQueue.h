/*
 * PriorityQueue.h
 *
 *      Author: c1moore
 */

#ifndef _SL_SCHEDULER_PRIORITYQUEUE
  #define _SL_SCHEDULER_PRIORITYQUEUE

  /**
   * A PriorityQueue stores items in descending order of priority. Items
   * with the same priority are stored in FIFO order.
   */
  template<class T>
  class PriorityQueue {
    public:
      PriorityQueue();
      ~PriorityQueue();
  
      /**
       * Adds item to the queue with the given priority.  Higher priority items will be added to the queue ahead of lower
       * priority items.  Items with the same priority will be inserted/removed in FIFO order.
       * 
       * @param item (const T) - the item to add to the queue
       * @param priority (int) - the priority of the item to insert
       */
      void enqueue(const T item, const int priority);
  
      /**
       * Removes the first item from the queue and returns it.
       * 
       * @returns (T) the item removed from the queue
       */
      T dequeue();

      /**
       * Removes the first occurrence of item in the queue.  While this method is not normally included for queues, it is
       * required for the Scheduler to function properly.
       *
       * @param item (const T) - the item to remove from the queue
       */
      void remove(const T item);
  
      /**
       * Returns the first item in the queue without removing it.
       * 
       * @returns (T) the first item in the queue
       */
      T peek() const;
  
      /**
       * Checks if the queue is empty.
       * 
       * @returns (bool) true iff this queue is empty
       */
      bool isEmpty() const;
  
      /**
       * Counts the total number of items in the queue.
       * 
       * @returns (int) the total number of items in the queue.
       */
      int count() const;
      
    private:
      class Implementation;

      Implementation *implementation;
  };

  #include "../scheduler/PriorityQueue.cpp"

#endif /* _SL_SCHEDULER_PRIORITYQUEUE */

