/*
 * DeltaList.h
 *
 *  Created on: Feb 2, 2018
 *      Author: c1moore
 */

#ifndef _SL_SCHEDULER_DELTALIST
  #define _SL_SCHEDULER_DELTALIST

  /**
   * DeltaItem stores data about an item in the DeltaList, including
   * the value of the delta and the item tied to the delta.
   */
  template<class T>
  class DeltaItem {
    public:
      T value;    // The value of the item to store in the delta list.
      int delta;  // The delta value used to sort the item relative to other items in the list.

      /**
       * Creates a new DeltaItem with the specified item.  The delta
       * value will be initialized to 0.
       *
       * @param value (T) - the item to insert in the DeltaList
       */
      DeltaItem(T value): value(value) {
        delta = 0;
      }

      /**
       * Creates a new DeltaItem with the specified item and delta
       * value.
       *
       * @param value (T) - the item to insert in the DeltaList
       * @param delta (int) - the delta value used to sort `value`
       *  in the DeltaList
       */
      DeltaItem(T value, int delta): value(value), delta(delta) { }
  };

  /**
   * A DeltaList stores items in a list sorted by a relative value
   * known as the delta value.  When inserting an item in the
   * DeltaList, the delta value is calculated by taking the initial
   * delta value provided by the user and subtracting from it the delta
   * values already stored in the list.  The item is inserted in the
   * last position where its associated delta value has the smallest,
   * nonnegative value.  For example, given (D, 5), where D is the
   * item to store in the list and 5 is the initial delta value, and
   * the list
   *
   *  (A, 0), (B, 1), (C, 3), (E, 7), (F, 0)
   *
   * D would be inserted right before E as (D, 1).  Furthermore, since
   * D was inserted before the end of the list, the next item in the
   * list will have to be decremented by the delta value used for D.
   * In the example above, this would mean the delta value for E would
   * become 6.  The final list would look like the following
   *
   *  (A, 0), (B, 1), (C, 3), (D, 1), (E, 6), (F, 0)
   */
  template<class T>
  class DeltaList {
    public:
      DeltaList();
      ~DeltaList();

      /**
       * Inserts a new item in this DeltaList using delta as the
       * initial delta value.  See the class description for more
       * detail on how an item is inserted in a DeltaList.
       *
       * @param item (const T) - the item to insert
       * @param delta (int) - the initial delta value to assign the
       *  item
       */
      void insert(const T item, int delta);

      /**
       * Decrements the delta value of the first item in the
       * DeltaList.
       *
       * @param value (const int) _optional_ - the value by which the
       *  first delta value should be decremented.  Default: 1
       */
      void decrement(const int value = 1);

      /**
       * Returns the DeltaItem for the first item in the DeltaList.
       *
       * @return (const DeltaItem<T>) the DeltaItem representing the
       *  first item in the DeltaList and its current delta value
       */
      const DeltaItem<T> peek() const;

      /**
       * Removes the first item from the DeltaList and returns it.
       *
       * @return (const T) the item that was removed from the
       *  DeltaList
       */
      const T remove();

      /**
       * Returns a count of all the items stored in this DeltaList.
       *
       * @return (int) a count of the items stored in this DeltaList
       */
      int count() const;

      /**
       * Returns whether the DeltaList is empty.
       *
       * @return (bool) true iff this DeltaList is empty; false
       *  otherwise
       */
      bool isEmpty() const;
    private:
      class Implementation;

      const Implementation *implementation;
  };

  #include "../scheduler/DeltaList.cpp"

#endif /* _SL_SCHEDULER_DELTALIST */
