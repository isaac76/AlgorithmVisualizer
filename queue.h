#ifndef QUEUE_H
#define QUEUE_H

#include "list.h"

template <class C> class Queue : public List<C>
{
public:
    Queue() {

    }

    void enqueue(C* data) {
        this->List<C>::insert(this->List<C>::tail(), data);
    }

    void dequeue(C** data) {
        this->remove(nullptr, data);
    }

    C* peek() {
        if (this->List<C>::head() == nullptr) {
            return nullptr;
        }
        return this->List<C>::head()->data();
    }
};

#endif // QUEUE_H
