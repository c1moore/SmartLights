/*
 * Scheduler.h
 *
 *      Author: c1moore
 */

#ifndef _SL_SCHEDULER_SCHEDULER
  #define _SL_SCHEDULER_SCHEDULER

  #include "../scheduler/Runnable.h"

  // A semi-random maximum number of threads allowed to be scheduled at any given time.  If you need this many threads, you
  // may want to reconsider your design.
  #define MAX_PROCESSES   128

  // The minimum number of milliseconds for which a Thread can sleep.  This value can/should be modified for the MCU on which
  // this library is being executed.  For example, the ESP8266 will begin to malfunction with a value less than or equal to 2
  // milliseconds.
  #define MIN_INTERVAL    3

  /**
   * Represents the current state of a Process.
   */
  enum ProcessState {
    DEAD = 0,   /* The Thread is dead and cannot execute again. */
    READY,      /* The Thread is ready and waiting to execute. */
    EXECUTING,  /* The Thread is executing. */
    SLEEPING,   /* The Thread is currently waiting for a delay to expire before it should execute. */
    SUSPENDED   /* The Thread is not ready to execute, but still needs to execute. */
  };

  /**
   * The Scheduler takes the place of the normal loop() method.  Scheduler allows for better handling of multiple tasks
   * in a more encapsulated/decoupled fashion.  While true multithreading or even hyperthreading is not (currently)
   * supported on the Arduino MCU, the Scheduler can emulate such behavior or at least provide a better approach to organizing
   * several independent tasks at once.
   * 
   * To quote from "Operating System Design: The XINU Approach"
   * 
   *    An operating system achieves the illusion of concurrent execution by rapidly switching a processor among several 
   *    computations.  Because the speed of the computation is extremely fast compared to that of a human, the effect is 
   *    impressive - multiple activies appear to proceed simultaneously.
   *    
   * Even though the Scheduler cannot really support concurrent execution, the MCU can fake it and make us pitiful humans think
   * it is capable of executing multiple calculations at once.
   * 
   * The implementation of the Scheduler is not perfect.  It is meant to be an extremely simplified and dumbed down version of 
   * what might exist on a more powerful embedded system or even (dare I say) personal computer.  The Scheduler or any other
   * part of this library does not implement a fully functioning OS that allows for true system calls.  Therefore, it cannot
   * truly implement even the simplified process manager set forth by the XINU OS.  Implementing a true OS that can handle
   * scheduling completely automatically is another project completely.  If you're wondering why I wrote this, its moreso to
   * try to convince myself to hold off on writing an OS for the Arduino so I can complete this project than to convince
   * anybody else that the Scheduler is not full featured or perfect.
   * 
   * That being said, you can shoot yourself in the foot using the Scheduler.  If you're not careful, you can starve out your
   * other processes.  Choose priorities careful and be extremely conscious of how long each process holds control of the
   * MCU.  If your process is never suspended or never sleeps and will execute indefinitely, it should probably have a
   * priority of 1.  Also be conscious of how often you call yield().  It should be called often enough to give other
   * processes and the underlying OS, if any, a chance to perform other vital or time sensitive actions.  Chances are if you
   * are defining long running processes that never exit, this library is not for you as your stack could quickly run out.
   *
   * By default, the Scheduler does not support making processes sleep or scheduling processes that should be executed at
   * a regular interval.  To support such behavior,
   *    1. The MCU must support interrupts that occur at a regular basis or the external code must be able to spoof such
   *       behavior
   *    2. The macro SCHEDULER_ENABLE_CLOCK must be defined
   *    3. `Scheduler.getInstance().tick()` must be called every milliseconds
   * If the above conditions are not met, attempting to cause a process to sleep or scheduling a process to execute at a
   * regular interval will fail.
   */
  class Scheduler {
    public:
      Scheduler(Scheduler const &scheduler) = delete;
      void operator=(Scheduler const &scheduler) = delete;
    
      /**
       * Returns an instance of the Scheduler.  This instance may or may not be the only Scheduler available; however, it
       * can be assumed that the the instance returned will be coordinated with any other instances that may exist.
       * 
       * @return (Scheduler) an instance of Scheduler
       */
      static Scheduler &getInstance();

      /**
       * Returns the PID of the currently executing process.  If no process is currently executing, -1 will be returned.  This
       * can occur, for example, if the method is invoked outside of `start()`.
       *
       * @returns (const int) the currently executing TID
       */
      const int getCurrentPid() const;

      /**
       * Schedules a new process to be executed.  If MAX_PROCESSES processes have been scheduled, a nonzero value will be
       * returned and the process will not be scheduled to execute.
       *
       * Any given process should only be scheduled once.  The only exception is when a process is scheduled using
       * `scheduleInterval()` and a positive integer has been specified for `repetitions`.  In this case, the process can be
       * re`schedule`d once it has completed all repetitions.
       * 
       * @param process (Runnable &) - the process to add to the Scheduler
       * @param priority (const int) - the priority of the new process.  This value must be between 1 and 15.  A higher value
       *  priority means the process should take precedence over other priorities of lower value.  Default: 1
       *
       * @returns (int) a positive integer representing the new process's PID if the process was successfully scheduled;
       *  otherwise, a negative value will be returned
       */
      int schedule(Runnable &process, const int priority = 1);

      /**
       * Schedules a new process to be executed at a specific interval.  The Scheduler only guarantees that the process will be
       * executed within an interval at least as small as the interval provided.  If a value less than MIN_INTERVAL is
       * provided, it will be rounded to MIN_INTERVAL.  If your process should execute relative to the original time it was
       * scheduled, the process should be rescheduled after each execution.  This is because any delays that occur in executing
       * this process will be propagated and likely exaggerated for the remaining iterations.
       * 
       * When selecting a new process to execute, the Scheduler will prefer processes scheduled with an interval.  In other words,
       * processes with an interval naturally have a higher priority than processes without an interval.
       * 
       * If MAX_PROCESSES processes have been scheduled, a nonzero value will be returned and the process will not be scheduled to
       * execute.
       *
       * For this method to work, the embedded system must have a clock and support hardware interrupts for the clock.  If
       * both of these conditions are met the the user wishes to use sleeping capabilities, s/he must register an interrupt
       * for the clock that occurs every millisecond and call the Scheduler's `tick()` method.  Once these conditions are met,
       * SCHEDULER_ENABLE_CLOCK macro must be defined to enable process sleeping.
       *
       * @param process (Runnable &) - the process to add to the Scheduler
       * @param interval (int) - the interval at which the Thread should run, in milliseconds
       * @param repetitions (int) - the total number of times this Thread should be executed at the specified interval.  A
       *  negative value will result in the Thread executing indefinitely.  Default: 1
       * @param priority (const int) - the priority of the new process.  This value must be between 1 and 15.  A higher value
       *  priority means the process should take precedence over other priorities of lower value.  Default: 1
       *
       * @returns (int) a positive integer representing the new process's PID if the process was successfully scheduled;
       *  otherwise, a negative value will be returned
       */
      int scheduleInterval(Runnable &process, const int interval, const int repetitions = 1, const int priority = 1);

      /**
       * Starts the Scheduler.  This should be executed at the end of the `setup()` method.
       */
      void start();

      /**
       * Notifies the Scheduler that a millisecond has passed.  Any sleeping processes that should be awoken will be awoken
       * and the scheduler will reschedule.
       */
      void tick();

      /**
       * Marks the process identified by `pid` as ready to execute.
       *
       * @param pid (const int) - the PID of the process to mark as `READY`
       *
       * @returns (int) 0 iff the process was updated successfully; otherwise, a negative value
       */
      int ready(const int pid);

      /**
       * Pauses the current running process for a minimum of `interval` milliseconds.  The Scheduler cannot guarantee that the
       * process will begin execution immediately after the specified interval, but it will do its best to execute the process
       * as close to the specified time as possible.
       * 
       * For this method to work, the embedded system must have a clock and support hardware interrupts for the clock.  If
       * both of these conditions are met the the user wishes to use sleeping capabilities, s/he must register an interrupt
       * for the clock that occurs every millisecond and call the Scheduler's `tick()` method.  Once these conditions are met,
       * SCHEDULER_ENABLE_CLOCK macro must be defined to enable process sleeping.
       *
       * @param interval (int) - the minimum milliseconds to pause the process
       *
       * @returns (int) 0 iff the process was updated successfully; otherwise, a negative integer
       */
      int sleep(const int delay);

      /**
       * Suggests to the Scheduler that the currently executing process is willing to yield its control of the MCU.  If there
       * are no other process waiting to executing, the process may continue to execute.  The processes's next state should be
       * set before calling this method.
       */
      void yield();

      /**
       * Suspends the process identified by `pid`.  A suspended process will not be scheduled to execute until it is unsuspended.
       *
       * @param pid (const int) - the PID of the process to suspend
       *
       * @returns (int) 0 iff the process was successfully suspended; otherwise, a negative integer
       */
      int suspend(const int pid);
      
      /**
       * Kills the current process.
       *
       * Killing the currently executing process is more of a courtesy to the Scheduler.  Since the Scheduler does not attempt
       * to manage the heap (or any memory), it cannot completely exit a process.  Therefore, if the currently executing process
       * wants to exit, it should call `Scheduler.kill()` and expect to be able to continue to execute.  The proper way to handle
       * this is to immediately exit the process (e.g. through calling `return` until the process's call stack has been fully
       * unwound).  A process that has called this method should not attempt to perform any other actions.
       *
       * This behavior is necessary because the Scheduler cannot free the process's call stack on its own.  If the Scheduler
       * simply ignored the process and simply rescheduled without freeing its memory, it could easily cause a stack overflow.
       *
       * @returns (int) 0 iff the process was successfully killed; otherwise, a negative integer
       */
      int kill();

    private:
      class SchedulerImplementation;

      Scheduler();
      ~Scheduler();
      
      SchedulerImplementation *implementation;
  };

#endif  /* _SL_SCHEDULER_SCHEDULER */
