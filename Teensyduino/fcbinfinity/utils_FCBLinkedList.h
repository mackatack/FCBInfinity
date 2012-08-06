/**
 * FCBLinkedList is a template class for a linked list. I created a linked
 * list implementation myself because I didn't want to import the entire STL
 * version 1.0 by mackatack@gmail.com
 * Released into the public domain.
 *
 * Use at own risk.
 */

#ifndef FCBInfinity_LinkedList_H
#define FCBInfinity_LinkedList_H

/**
 * Template for our linkedlist, we use this template in other places as well.
 */

// Linked List entry node
template <class T>
struct FBCListItem {
  // Pointer to the user data
  T * data;

  // Pointer to the next entry in the LinkedList
  FBCListItem<T> * next;

  // Constructor that initializes the data
  FBCListItem(T * d) {
    data = d;
    next = NULL;
  }
};

// Linked List class
template <class T>
class FCBLinkedList {
  public:
    // Constructor
    FCBLinkedList() {
      // Set all the pointers to NULL;
      head = last = prev = curr = NULL;
      numItems = 0;
    }

    // Add a new item to the end
    T* addItem(T * itemData) {
      FBCListItem<T> * entry = new FBCListItem<T>(itemData);
      if (head==NULL)
        head = entry;
      else
        last->next = entry;
      last = entry;
      ++numItems;
      return itemData;
    }

    // Return the current data
    T* current() {
      if (curr == NULL)
        return NULL;
      return curr->data;
    }

    // Set the internal pointer to the head item
    // and return the head data.
    T* reset() {
      curr = head;
      prev = NULL;
      return current();
    }

    // Return the next item's data
    T* next() {
      if (curr == NULL)
        return NULL;
      prev = curr;
      curr = curr->next;
      return current();
    }

    // Returns whether or not this linkedlist is empty
    bool isEmpty() {
      return head == NULL;
    }

    // Removes the item that is on the curr pointer and jumps to the next item.
    // It also calls a delete to the data stored in the linked list. If you don't want
    // this, use removeCurrent(false)
    void removeCurrent() {
      removeCurrent(true);
    }

    // Removes the item that is on the curr pointer and jumps to the next item.
    // @param deleteReferencedData also call a 'delete' on the data stored in the linkedlist
    void removeCurrent(bool deleteReferencedData) {
      if (curr == NULL) return;

      // Item is head, next is the new head
      if (curr == head)
        head = curr->next;
      // item is last, prev is the new last
      if (curr == last)
        last = prev;
      // If prev exists link it to the new next
      if (prev != NULL)
      prev->next = curr->next;

      // Remember this item and set curr to the next item
      FBCListItem<T> * pItem = curr;
      curr = curr->next;

      // Delete the data in the entry if requested
      if (deleteReferencedData)
        delete pItem->data;

      // Clean up the entry
      delete pItem;
      --numItems;
    }

    // Returns the number of entries in the linked list.
    int getSize() {
      return numItems;
    }
  protected:
    int numItems;            // Holds the number of items in this list.
    FBCListItem<T> * head;   // The first entry in the linked list
    FBCListItem<T> * last;   // The last entry in the linked list
    FBCListItem<T> * prev;   // While moving through the linked list, this will keep the previous item
    FBCListItem<T> * curr;   // While moving through the linked list, this will have the current item
};

#endif