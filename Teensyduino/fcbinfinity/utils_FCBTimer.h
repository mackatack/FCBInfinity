/**
 * FCBTimerManager is a tool that allows us to register functions
 * and have them called after a specified timeout or interval.
 * Since this uses function pointers, you can also unregister these
 * functions.
 * version 1.0 by mackatack@gmail.com
 * Released into the public domain.
 *
 * Use at own risk.
 */

 // @TODO: add documentation and code cleanup

#ifndef FCBInfinity_TimerH
#define FCBInfinity_TimerH

#include <Wprogram.h>
#include "utils_FCBLinkedList.h"

/**
 *  The timerEntry struct this is actually a node in a linked list.
 */
struct FCBTimer {
    FCBTimer(int interval, int numrepeats, void (*callback)(FCBTimer*));
    unsigned long m_lExpires;           // Tracks at what milis() this timer will expire
    int m_iInterval;                    // In case of a repeating timer, what is the interval
    int m_iNumRepeats;                  // In case of a repeating timer, how many times will it repeat
    void (*m_fpCallback)( FCBTimer* );  // The function pointer to the callback that needs to be called.
};

/**
 * A singleton object that keeps a linkedlist of FCBTimer objects.
 * This manager lets us create synchronised timers, this means they
 * run in the same thread as the current program and won't cause deadlocks
 * or race-conditions because of this.
 * It expects the developer to call it's processTimers() function as often as
 * possible. For example in the loop() function of a Arduino program.
 * Keep in mind that this will never call the callback before the timer has expired
 * but there's no guarantee that it will be called exactly on time, it all depends
 * when the processTimers() is called.
 * This library has been created in such a way that creating new timers from
 * timer callback functions is supported. Also because the callback receives
 * the timer struct you can modify the timer's repetitions or interval for
 * subsequent callbacks.
 *
 * @example
 * void callback() { printf("callback called!") }
 *
 * FCBTimerManager::addTimeout(1000, &callback);
 * // This will call the callback() function after 1000 msec.
 */
class FCBTimerManager  {

  public:
    /**
     * Loops through all the FCBTimer nodes and see if their timer has expired
     */
    static void processTimers();

    /**
     * Returns true if the manager still has active timers
     */
    static bool hasTimers() {
      return !list->isEmpty();
    }

    /**
     * Add a new timer that calls the callback function once and then stops
     */
    static void addTimeout(int timeout, void (*func)(FCBTimer*));

    /**
     * Add a new timer that calls the callback function infinitely
     */
    static void addInterval(int interval, void (*func)(FCBTimer*));

    /**
     * Add a new timer that calls the callback function for numRepeats times
     */
    static void addInterval(int interval, int numRepeats, void (*func)(FCBTimer*));

    /**
     * Deletes all timers with the corresponding function pointer.
     */
    static void removeTimer(void (*func)(FCBTimer*));

    /**
     * If only this would work! :(
     */
    static void backToTheFuture();

  protected:

    /**
     * Static linkedlist of FCBTimer structs
     */
    static FCBLinkedList<FCBTimer> * list;

    // Contructor is private so this is a singleton
    FCBTimerManager();

};

#endif
