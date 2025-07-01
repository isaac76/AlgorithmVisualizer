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
        // Don't insert null data
        if (data == nullptr) {
            return;
        }
        
        // Create the new node
        ListNode<C>* newNode = new ListNode<C>();
        newNode->setData(data);
        
        // Check if node is a valid node in our list
        if (node != nullptr) {
            bool nodeFound = false;
            ListNode<C>* current = this->h;
            
            while (current != nullptr) {
                if (current == node) {
                    nodeFound = true;
                    break;
                }
                current = current->next();
            }
            
            // If node isn't in list, treat as null (insert at head)
            if (!nodeFound) {
                node = nullptr;
            }
        }
        
        // Now handle insertion
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
        if (takeOwnership) {
            this->takeOwnership(data);
        }

        this->size++;
    }

    void remove(ListNode<C>* node, C** data) {
        // Add null checks and empty list checks
        if (this->h == nullptr || this->size == 0) {
            if (data != nullptr) {
                *data = nullptr;
            }
            return;
        }
        
        // If removing a specific node, validate it exists in the list
        if (node != nullptr) {
            bool nodeFound = false;
            ListNode<C>* current = this->h;
            
            while (current != nullptr) {
                if (current == node) {
                    nodeFound = true;
                    break;
                }
                current = current->next();
            }
            
            if (!nodeFound) {
                // Node not in list - either handle as error or set data to null and return
                if (data != nullptr) {
                    *data = nullptr;
                }
                return;
            }
        }

        ListNode<C>* oldNode;
        C* removedData = nullptr;

        if (node == nullptr) {
            // Remove from head
            removedData = this->head()->data();
            oldNode = this->head();
            this->setHead(this->head()->next());
            
            // Update tail if we've removed the last node
            if (this->head() == nullptr) {
                this->setTail(nullptr);
            }
        } else {
            // Remove after the specified node
            if (node->next() == nullptr) {
                // Can't remove after tail
                if (data != nullptr) {
                    *data = nullptr;
                }
                return;
            }

            removedData = node->next()->data();
            oldNode = node->next();
            node->setNext(node->next()->next());
            
            // Update tail if we removed the last node
            if (node->next() == nullptr) {
                this->setTail(node);
            }
        }

        // Set output data pointer if provided
        if (data != nullptr) {
            *data = removedData;
        }

        // Release ownership of the data if we owned it
        if (removedData != nullptr && this->hasOwnership(removedData)) {
            this->releaseOwnership(removedData);
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
    
    void clear() {
        C* data;
        while (this->size > 0) {
            remove(nullptr, &data);
        }
    }
};

#endif // LIST_H
