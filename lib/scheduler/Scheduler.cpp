/*
 * Scheduler.cpp
 *
 *      Author: c1moore
 */

#include "../scheduler/Scheduler.h"

#include <stdint.h>

#include <Arduino.h>
#include "../scheduler/DeltaList.h"
#include "../scheduler/PriorityQueue.h"
#include "../scheduler/Runnable.h"

/**
 * ProcessData represents the structure of data stored about each process in the process table.
 */
struct ProcessData {
  Runnable *process;  // The Runnable that should be executed when the process is active
  ProcessState state; // The process's current state
  int priority;       // The process's priority
  int repetitions;    // If the process should execute at a specific interval, the total number of times the process should execute
  int interval;       // If the process should execute multiple times at a given interval, the interval at which the process should execute.
};

/**
 * Implementation details for the Scheduler.
 */
class Scheduler::SchedulerImplementation {
  public:
    ProcessData ptable[MAX_PROCESSES] = { { 0 } };  // The table of all processes managed by Scheduler.

    PriorityQueue<int> readyList; // The list of processes waiting to execute.
    DeltaList<int> sleepingList;  // The list of processes currently sleeping.

    int currentPid;               // The ID of the process currently executing.
    int nextValidPid;             // The next process ID to attempt when assigning a new process its ID.

    bool started;                 // true iff Scheduler has started; false otherwise

    SchedulerImplementation() {
      readyList = PriorityQueue<int>();
      sleepingList = DeltaList<int>();

      currentPid = -1;
      nextValidPid = 0;

      started = false;
    }

    /**
     * Returns the next available PID that can be used for a process being scheduled.  If MAX_PROCESSES has been reached, -1 is returned.
     * Since scheduling a new process is not expected to be common and since memory is likely limited, we'll trade time for memory when
     * searching the table for an empty entry.
     *
     * @returns (int) the PID to use for the process being scheduled or -1 if no more processes can be scheduled
     */
    int getNextAvailablePid() {
      // Scheduling a new thread is not expected to be common, so we'll trade time for memory.
      int nextPid = nextValidPid;
      int originalPid = nextPid;

      if(ptable[nextPid].state != DEAD) {
        nextPid = (nextPid + 1) % MAX_PROCESSES;

        if(nextPid == originalPid) {
          return -1;
        }
      }

      return nextPid;
    }

    /**
     * Determines the next process to execute and begin executing it.  The current process's next state should be specified before rescheduling.  If
     * the process wishes to remain eligible, it state should remain as EXECUTING.
     */
    void reschedule() {
      delay(0); // Give the underlying OS, if any, time to do any necessary processing.

      if(readyList.isEmpty()) {
        return;
      }

      int nextPid = readyList.peek();

      if(ptable[currentPid].state == EXECUTING) {
        if(ptable[nextPid].priority < ptable[currentPid].priority) {
          return;
        }

        ptable[currentPid].state = READY;
        readyList.enqueue(currentPid, ptable[currentPid].priority);
      }

      nextPid = readyList.dequeue();

      switchContext(nextPid);
    }

    /**
     * For processes that should be repeated, updates the process's state in the process table to reflect a completed iteration.
     *
     * @param pid (const int) - the ID of the process that just finished executing
     */
    void postExecute(const int pid) {
      ProcessData process = ptable[pid];

      if(process.repetitions > 0) {
        --process.repetitions;

        if(process.repetitions <= 0) {
          // The process has finished all its repetitions and can be removed from the process table.
          ptable[pid] = { 0 };
        } else {
          // The process needs to be repeated.
          repeatProcess(pid, process);
        }
      } else if(process.repetitions < 0) {
        // This process repeats indefinitely.  Make it sleep.
        repeatProcess(pid, process);
      }
    }

    /**
     * Starts executing the process specified by nextPid.
     *
     * @param nextPid (int) - the ID of the process that should be executed
     */
    void switchContext(int nextPid) {
      ProcessData nextProcess = ptable[nextPid];

      currentPid = nextPid;
      nextProcess.state = EXECUTING;

      nextProcess.process->run();
      postExecute(nextPid);
    }

  private:
    /**
     * Adds the back to the sleeping list to be executed again.
     *
     * @param pid (const int) - the ID of the process that should be repeated
     * @param process (ProcessData) - the process's entry in the ptable
     */
    void repeatProcess(const int pid, ProcessData process) {
      sleepingList.insert(pid, process.interval);

      process.state = SLEEPING;
    }
};

Scheduler::Scheduler() {
  implementation = new SchedulerImplementation();
}

Scheduler::~Scheduler() {
  delete implementation;
}

Scheduler &Scheduler::getInstance() {
  static Scheduler scheduler;

  return scheduler;
}

const int Scheduler::getCurrentPid() const {
  return implementation->currentPid;
}

int Scheduler::schedule(Runnable &process, const int priority) {
  ProcessData *ptable = implementation->ptable;
  int pid = implementation->getNextAvailablePid();

  if(pid < 0) {
    return pid;
  }

  ptable[pid].process = &process;
  ptable[pid].priority = priority;
  ptable[pid].state = READY;  // New processes must be READY.  If they aren't, they can be SUSPENDED immediately.

  // While pid + 1 may not be (currently) available, it may be available by the next time a process is scheduled and we want to
  // minimize the number of times a PID is reused.
  implementation->nextValidPid = pid + 1;

  implementation->readyList.enqueue(pid, priority);

  // If the Scheduler has already taken control, we need to give the new process a chance to be executed.
  if(implementation->started) {
    yield();
  }

  return pid;
}

int Scheduler::scheduleInterval(Runnable &process, const int interval, const int repetitions, const int priority) {
  #ifdef SCHEDULER_ENABLE_CLOCK
    int pid = implementation->getNextAvailablePid();
    ProcessData processData = implementation->ptable[pid];

    if(pid < 0) {
      return pid;
    }

    if(interval < 0) {
      return -2;
    }

    if(interval < MIN_INTERVAL) {
      interval = MIN_INTERVAL;
    }

    processData.process = &process;
    processData.priority = priority;
    processData.state = SLEEPING;
    processData.interval = interval;
    processData.repetitions = repetitions;

    // While pid + 1 may not be (currently) available, it may be available by the next time a process is scheduled and we want to
    // minimize the number of times a PID is reused.
    implementation->nextValidPid = pid + 1;

    implementation->sleepingList.insert(pid, interval, priority);

    // If the Scheduler has already taken control, we need to give the new process a chance to be executed.
    if(implementation->started) {
      yield();
    }

    return pid;
  #else
    return -1;
  #endif
}

void Scheduler::start() {
  implementation->started = true;

  // This will take the place of loop(), so we need to loop forever.
  while(true) {
    delay(0);   // First let any underlying OS or external services get a chance to execute.
    
    if(implementation->readyList.isEmpty()) {
      continue;
    }

    int nextPid = implementation->readyList.dequeue();

    implementation->ptable[nextPid].process->run();
    implementation->postExecute(nextPid);
  }
}

void Scheduler::yield() {
  implementation->reschedule();
}

int Scheduler::ready(const int pid) {
  ProcessData readyProcess = implementation->ptable[pid];

  if(readyProcess.state != SUSPENDED) {
    return -1;
  }

  readyProcess.state = READY;
  implementation->readyList.enqueue(pid, readyProcess.priority);

  if(readyProcess.priority > implementation->ptable[implementation->currentPid].priority) {
    implementation->reschedule();
  }

  return 0;
}

int Scheduler::sleep(const int delay) {
  #ifdef SCHEDULER_ENABLE_CLOCK
    implementation->sleepingList.insert(implementation->currentPid, delay);
    implementation->ptable[implementation->currentPid].state = SLEEPING;

    implementation->reschedule();
  #else
    return -1;
  #endif
}

int Scheduler::suspend(const int pid) {
  ProcessData suspendedProcess = implementation->ptable[pid];

  if(suspendedProcess.state != READY && suspendedProcess.state != EXECUTING) {
    return -1;
  }

  if(suspendedProcess.state == READY) {
    implementation->readyList.remove(pid);

    suspendedProcess.state = SUSPENDED;
  } else {
    suspendedProcess.state = SUSPENDED;

    implementation->reschedule();
  }

  return 0;
}

int Scheduler::kill() {
  implementation->ptable[implementation->currentPid] = { 0 };

  return 0;
}

void Scheduler::tick() {
  DeltaList<int> sleepingList = implementation->sleepingList;

  sleepingList.decrement();

  if(sleepingList.peek().delta > 0) {
    return;
  }

  ProcessData *ptable = implementation->ptable;
  PriorityQueue<int> readyList = implementation->readyList;

  while(sleepingList.peek().delta <= 0) {
    delay(0);

    int awokenPid = sleepingList.remove();

    readyList.enqueue(awokenPid, ptable[awokenPid].priority);
  }

  implementation->reschedule();
}
