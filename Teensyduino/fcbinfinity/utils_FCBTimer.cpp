#include <Wprogram.h>
#include "utils_FCBTimer.h"

// @TODO: add documentation and code cleanup

FCBTimer* FCBTimerManager::head = NULL;
FCBTimer* FCBTimerManager::last = NULL;

FCBTimer::FCBTimer(int interval, int numrepeats, void (*callback)(FCBTimer*)) {
  m_fpCallback = callback;
  m_iInterval = interval;
  m_iNumRepeats = numrepeats;
  m_lExpires = millis() + interval;
  next = NULL;
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
	if (head==NULL) {
		head = timer;
    last = timer;
  } else {
    last->next = timer;
  }
	last = timer;
}

//
// Loops through all the FCBTimer nodes and see if their timer has expired
// This can be optimized by either sorting Timers based on their expiration
// values and by keeping a reference to the earliest expiring timer, so
// we dont have to loop through all the objects, only to find out there are
// no expired timers. For now just keep it simple.
void FCBTimerManager::processTimers() {
  // Return if there are no active timers
  if (head == 0)
    return;

	FCBTimer * curr = head;
	FCBTimer * prev = NULL;

  // Loop through all the timer objects in the linked list
	do {
    // Has the current timer expired?
		if (curr->m_lExpires > millis()) {
      // The timer hasn't expired, just continue to the next entry
			prev = curr;
			curr = curr->next;
      continue;
    }

    // The timer expired, call the callback
    (*curr->m_fpCallback)(curr);

    // Check for repetitions
    if (curr->m_iNumRepeats > 0)
      curr->m_iNumRepeats--;

    if (curr->m_iNumRepeats==0) {
      // Timer has no more repetitions, delete it from the linkedlist

      // In case this isn't the head object, set the 'next' pointer
      // of the previous timer to the next timer of this object :P
      if (prev!=NULL)
        prev->next = curr->next;

      // In case the current timer entry is the head of the linkedlist
      // set the head to the next entry
      if (head == curr)
        head=curr->next;

      // Same goes for the last entry
      if (last == curr)
        last=prev;

      // Temporarely store the pointer to this timer,
      // so we can update the curr pointer and delete the
      // expired timer.
      FCBTimer * dTimer = curr;
      curr = curr->next;
      delete dTimer;
    }
    else {
      // Timer still has repetitions, update the new expiration
      curr->m_lExpires = millis() + curr->m_iInterval;
      prev = curr;
      curr = curr->next;
    }
	}
	while(curr>0);
}

//
// Removes a timer from the manager. It deletes all the timers with a reference
// to the specified callback
void FCBTimerManager::removeTimer(void (*func)(FCBTimer*)) {
  // Return if there are no active timers
  if (head == 0)
    return;

	FCBTimer * curr = head;
	FCBTimer * prev = NULL;

  // Loop through all the timer objects in the linked list
	do {
    // Function pointer doesnt match? continue
    if (curr->m_fpCallback != func) {
      prev = curr;
      curr = curr->next;
      continue;
    }

    // Function pointer does match
    // In case this isn't the head object, set the 'next' pointer
    // of the previous timer to the next timer of this object :P
    if (prev!=NULL)
      prev->next = curr->next;

    // In case the current timer entry is the head of the linkedlist
    // set the head to the next entry
    if (head == curr)
      head=curr->next;

    // Same goes for the last entry
    if (last == curr)
      last=prev;

    // Temporarely store the pointer to this timer,
    // so we can update the curr pointer and delete the
    // expired timer.
    FCBTimer * dTimer = curr;
    curr = curr->next;
    delete dTimer;
	}
	while(curr>0);
}

//
// Returns true if the manager still has active timers
bool FCBTimerManager::hasTimers() {
  return head>0;
}

// Marty McFly's secret function
void FCBTimerManager::backToTheFuture() {
  // If only this would work :P
  // If my calculations are correct, when this baby hits 88 miles per hour... you're gonna see some serious shit.
}