/**
 * FCBAssocLinkedList is a template class for a linked list. That also allows it to be
 * used as a map. I created a linked list implementation myself because I didn't want to
 * import the entire STL for linked list and map implementations.
 * This library expects the stored data to be pointers, keys pointing to the data are
 * probably often of integer type
 * version 1.0 by mackatack@gmail.com
 * Released into the public domain.
 *
 * Use at own risk.
 */

#ifndef FCBInfinity_AssocLinkedList_H
#define FCBInfinity_AssocLinkedList_H

/**
 * Template for our linkedlist, we use this template in other places as well.
 */

// Linked List entry node
template <class dT, typename keyT=int>
struct FBCAssocListItem {
  // Pointer to the user data
  dT * data;

  // The user key value, this allows a user
  // to set a value by key and use get by key functions
  keyT key;

  // Pointer to the next entry in the LinkedList
  FBCAssocListItem<dT, keyT> * next;

  // Constructor that initializes the data
  FBCAssocListItem(dT * d, keyT k) {
    data = d;
    key = k;
    next = NULL;
  }
  // Constructor that initializes the data
  FBCAssocListItem(dT * d) {
    data = d;
    next = NULL;
  }
};

// Linked List class
template <class T, typename keyT=int>
class FCBAssocLinkedList {
  public:
    // Constructor
    FCBAssocLinkedList() {
      // Set all the pointers to NULL;
      head = last = prev = curr = NULL;
      numItems = 0;
    }

    // Sets an associative value
    T* set(keyT key, T * data) {
      reset();
      FBCAssocListItem<T, keyT> * entry;
      while((entry=curr) != NULL) {
        if (entry->key == key) {
          entry->data = data;
          return data;
        }
        next();
      }
      addItem(data);
      last->key = key;
      return last->data;
    }

    // Return the data stored at key, or NULL if no data
    T* get(keyT key) {
      reset();
      FBCAssocListItem<T, keyT> * entry;
      while((entry=curr) != NULL) {
        if (entry->key == key) {
          return entry->data;
        }
        next();
      }
      return NULL;
    }

    // Remove the entry associated with key from the linked list
    void remove(keyT key) {
      reset();
      FBCAssocListItem<T, keyT> * entry;
      while((entry=curr) != NULL) {
        if (entry->key == key) {
          removeCurrent();
          return;
        }
        next();
      }
    }

    // This sweet little operator allows us to use the linked list
    // as a associative array:
    // FCBAssocLinkedList<byte *, int> list = FCBAssocLinkedList<byte *, int>();
    // list[123] = "test123";
    // Isn't that lovely?
    // It always creates the entry in the linkedlist for the requested key
    T* operator[](keyT key) {
      T* result = get(key);
      // Entry already on the linked list?
      if (curr!=NULL)
        return result;
      // Nope, make a new entry
      addItem(NULL);
      last->key = key;
      return last->data;
    }
    // Add a new item to the end
    T* addItem(T* itemData) {
      FBCAssocListItem<T, keyT> * entry = new FBCAssocListItem<T, keyT>(itemData);
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
        return (keyT)NULL;
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
    // @param deleteReferencedData also call a 'delete' on the data stored in the linkedlist (default)
    void removeCurrent(bool deleteReferencedData=true) {
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
      FBCAssocListItem<T, keyT> * pItem = curr;
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
    FBCAssocListItem<T, keyT> * head;   // The first entry in the linked list
    FBCAssocListItem<T, keyT> * last;   // The last entry in the linked list
    FBCAssocListItem<T, keyT> * prev;   // While moving through the linked list, this will keep the previous item
    FBCAssocListItem<T, keyT> * curr;   // While moving through the linked list, this will have the current item
};

#endif
