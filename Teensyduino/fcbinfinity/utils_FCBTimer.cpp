#include <Wprogram.h>
#include "utils_FCBLinkedList.h"
#include "utils_FCBTimer.h"

// Initialize the static linked list
FCBLinkedList<FCBTimer> * FCBTimerManager::list = new FCBLinkedList<FCBTimer>;

// Initialization of the FCBTimer linkedlist entry
FCBTimer::FCBTimer(int interval, int numrepeats, void (*callback)(FCBTimer*)) {
  m_fpCallback = callback;
  m_iInterval = interval;
  m_iNumRepeats = numrepeats;
  m_lExpires = millis() + interval;
}

//
// Add a new timer that calls the callback function once and then stops
void FCBTimerManager::addTimeout(int timeout, void (*func)(FCBTimer*)) {
  addInterval(timeout, 1, func);
}
//
// Add a new timer that calls the callback function infinitely
void FCBTimerManager::addInterval(int interval, void (*func)(FCBTimer*)) {
  addInterval(interval, -1, func);
}
//
// Add a new timer that calls the callback function for numRepeats times
void FCBTimerManager::addInterval(int interval, int numRepeats, void (*func)(FCBTimer*)) {
  FCBTimer * timer = new FCBTimer(interval, numRepeats, func);
  list->addItem(timer);
}

//
// Loops through all the FCBTimer nodes and see if their timer has expired
// This can be optimized by either sorting Timers based on their expiration
// values and by keeping a reference to the earliest expiring timer, so
// we dont have to loop through all the objects, only to find out there are
// no expired timers. For now just keep it simple.
void FCBTimerManager::processTimers() {
  // Return if there are no active timers
  if (list->isEmpty())
    return;

  // Loop through all the timer objects in the linked list
  FCBTimer * curr;
  list->reset();
  while((curr = list->current()) != NULL) {
    // Has the current timer expired?
    if (curr->m_lExpires > millis()) {
      // The timer hasn't expired, just continue to the next entry
      list->next();
      continue;
    }

    // The timer expired, call the callback
    (*curr->m_fpCallback)(curr);

    // Check for repetitions
    if (curr->m_iNumRepeats > 0)
      curr->m_iNumRepeats--;

    if (curr->m_iNumRepeats==0) {
      // Timer has no more repetitions, delete it from the linkedlist
      // Internally the linked list will clean up the linked object
      // and moves the pointer to the next entry, so no need
      // to delete and call next()
      list->removeCurrent();
      continue;
    }

    // Timer still has repetitions, update the new expiration
    // and move on to the next entry in the linkedlist
    curr->m_lExpires = millis() + curr->m_iInterval;
    list->next();
  }
}

//
// Removes a timer from the manager. It deletes all the timers with a reference
// to the specified callback
void FCBTimerManager::removeTimer(void (*func)(FCBTimer*)) {
  // Return if there are no active timers
  if (list->isEmpty())
    return;

  // Loop through all the timer objects in the linked list
  FCBTimer * curr;
  list->reset();
  while((curr = list->current()) != NULL) {
    // Function pointer doesnt match? continue to next entry
    if (curr->m_fpCallback != func) {
      list->next();
      continue;
    }

    // Function pointer does match, remove the entry
    list->removeCurrent();
  }
}

// Marty McFly's secret function
void FCBTimerManager::backToTheFuture() {
  // If only this would work :P
  // If my calculations are correct, when this baby hits 88 miles per hour... you're gonna see some serious shit.
}