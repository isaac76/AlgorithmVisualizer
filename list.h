#ifndef LIST_H
#define LIST_H

#include "listnode.h"
#include "collection.h"

template<class C, class Compare = std::equal_to<C>> class List : public Collection<C, Compare>
{
private:
    ListNode<C>* h;  // Head pointer
    ListNode<C>* t;  // Tail pointer
public:
    List() : Collection<C, Compare>() {
        this->h = nullptr;
        this->t = nullptr;
    }

    List(const Compare &comp) : Collection<C, Compare>(comp) {
        this->h = nullptr;
        this->t = nullptr;
    }

    ~List() override {
        C* data;
        while (this->size > 0) {
            // Remove from the head (passing nullptr removes from head)
            remove(nullptr, &data);
            
            // If we own the data, it'll be deleted by the Collection destructor
            // through the ownership tracking system
        }

        // Ensure head and tail are cleared
        this->h = nullptr;
        this->t = nullptr;
        
        // Base class destructor will clean up the owned data
    }

    void insert(ListNode<C>* node, C* data, bool takeOwnership = false) {
        ListNode<C>* newNode = new ListNode<C>();

        newNode->setData(data);

        if (node == nullptr) {
            // Insert at the head
            newNode->setNext(this->h);
            this->setHead(newNode);
            
            // If this is the first node, it's also the tail
            if (this->size == 0) {
                this->setTail(newNode);
            }
        } else {
            // Insert after the given node
            if (node->next() == nullptr) {
                this->setTail(newNode);
            }
            newNode->setNext(node->next());
            node->setNext(newNode);
        }
        
        // Take ownership of the data if requested
        if (takeOwnership && data != nullptr) {
            this->takeOwnership(data);
        }

        this->size++;
    }

    void remove(ListNode<C>* node, C** data) {
        ListNode<C>* oldNode;

        if (this->size == 0) {
            return;
        }

        if (node == nullptr) {
            *data = this->head()->data();
            oldNode = this->head();
            this->setHead(this->head()->next());
        } else {
            if (node->next() == nullptr) {
                return;
            }

            *data = node->next()->data();
            oldNode = node->next();
            node->setNext(node->next()->next());
        }

        // Release ownership of the data if we owned it
        // The data itself will be deleted by the caller if needed
        if (this->hasOwnership(*data)) {
            this->releaseOwnership(*data);
        }

        delete oldNode;
        this->size--;
    }

    void setHead(ListNode<C>* h) {
        this->h = h;
    }

    ListNode<C>* head() const {
        return this->h;
    }

    void setTail(ListNode<C>* t) {
        this->t = t;
    }
    
    ListNode<C>* tail() const {
        return this->t;
    }
    // Note: takeOwnership, releaseOwnership, and hasOwnership methods
    // are inherited from the Collection base class
};

#endif // LIST_H
