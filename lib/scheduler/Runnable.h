/*
 * Runnable.h
 *
 *      Author: c1moore
 */

#ifndef _ARDUINO_RUNNABLE
  #define _ARDUINO_RUNNABLE

  /**
   * Runnable defines an interface for instances of classes that can be
   * scheduled and executed by the Scheduler.
   *
   * This interface is meant to provide a common protocol for objects
   * that wish to execute code while they are active.
   */
  class Runnable {
    public:
      Runnable() {}
      virtual ~Runnable() {};

      /**
       * This operation can perform any action whatsoever.  If the Runnable is being used in a Thread, this method will be
       * executed when the Thread is ready to run.
       *
       * @returns (int) a success code.  A value of 0 means successful execution.  Any other value represents an unsuccessful
       *  execution; however, what a non-zero value signifies is specific to the Runnable.
       */
      virtual int run() = 0;
  };

#endif
