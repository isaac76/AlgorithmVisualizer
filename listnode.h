#ifndef LISTNODE_H
#define LISTNODE_H

#include "node.h"

template<class C> class ListNode : public Node<C>
{
private:
    ListNode* n;
public:
    ListNode() {
        n = nullptr;
    }
    ListNode<C>* next() {
        return n;
    }
    void setNext(ListNode<C>* n) {
        this->n = n;
    }
};

#endif // LISTNODE_H
