#ifndef QUEUE_H
#define QUEUE_H

#include "list.h"

template <class C> class Queue : public List<C>
{
public:
    Queue() {
        // Already initialized properly by List constructor
    }
    
    // Enqueue: adds element to the back of the queue
    void enqueue(C* data) {
        // Since we're now checking for null data in List::insert,
        // no additional null check is needed here
        this->List<C>::insert(this->List<C>::tail(), data);
    }
    
    // Dequeue: removes element from the front of the queue
    void dequeue(C** data) {
        // List::remove has been improved with robust error handling
        this->remove(nullptr, data);
    }
    
    // Peek: look at front element without removing
    C* peek() {
        if (this->List<C>::head() == nullptr) {
            return nullptr;
        }
        return this->List<C>::head()->data();
    }
    
    // Additional useful methods
    
    // Check if queue is empty
    bool isEmpty() const {
        return this->getSize() == 0;
    }
    
    // Clear the entire queue
    void clear() {
        C* data;
        while (!this->isEmpty()) {
            this->dequeue(&data);
        }
    }
};

#endif // QUEUE_H
