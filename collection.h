#ifndef COLLECTION_H
#define COLLECTION_H

#include <functional>
#include "owneddatanode.h"

/**
 * @class Collection
 * @brief Base class for data structure collections that manages common functionality
 * including memory ownership
 * 
 * This class provides basic memory management functionality that child classes
 * like List and Set can inherit. It manages a simple ownership tracking system
 * that allows collection classes to take ownership of elements and ensure proper
 * cleanup when the collection is destroyed.
 */
template<class C, class Compare = std::equal_to<C>> class Collection
{
protected:
    OwnedDataNode<C>* ownedHead;  // Head of the ownership tracking list
    int size;                     // Number of elements in the collection
    Compare equalTo;
    
public:
    /**
     * @brief Default constructor
     */
    Collection() {
        this->size = 0;
        this->ownedHead = nullptr;
    }
    
    /**
     * @brief Constructor with Compare functor
     * @param comp Compare functor for elements
     */
    Collection(const Compare& comp) {
        this->size = 0;
        this->ownedHead = nullptr;
        this->equalTo = comp;
    }
    
    /**
     * @brief Virtual destructor to ensure proper cleanup in derived classes
     */
    virtual ~Collection() {
        // Clean up owned data
        OwnedDataNode<C>* current = this->ownedHead;
        while (current != nullptr) {
            OwnedDataNode<C>* next = current->next();
            
            // Delete the owned data
            if (current->data() != nullptr) {
                delete current->data();
            }
            
            // Delete the node
            delete current;
            current = next;
        }
        
        this->ownedHead = nullptr;
    }
    
    /**
     * @brief Take ownership of a data element
     * @param data The element to take ownership of
     */
    void takeOwnership(C* data) {
        if (data == nullptr || hasOwnership(data)) {
            return;  // Don't take ownership of nullptr or already owned data
        }
        
        // Create a new ownership node
        OwnedDataNode<C>* newNode = new OwnedDataNode<C>(data);
        
        // Insert at head (O(1) operation)
        newNode->setNext(this->ownedHead);
        this->ownedHead = newNode;
    }
    
    /**
     * @brief Release ownership of a data element
     * @param data The element to release ownership of
     */
    void releaseOwnership(C* data) {
        if (data == nullptr || this->ownedHead == nullptr) {
            return;
        }
        
        // Special case for head
        if (this->ownedHead->data() == data) {
            OwnedDataNode<C>* oldHead = this->ownedHead;
            this->ownedHead = this->ownedHead->next();
            delete oldHead;
            return;
        }
        
        // Search for the data in our ownership list
        OwnedDataNode<C>* current = this->ownedHead;
        while (current->next() != nullptr) {
            if (current->next()->data() == data) {
                OwnedDataNode<C>* toDelete = current->next();
                current->setNext(toDelete->next());
                delete toDelete;
                return;
            }
            current = current->next();
        }
    }
    
    /**
     * @brief Check if the collection owns a data element
     * @param data The element to check ownership of
     * @return true if the collection owns the element, false otherwise
     */
    bool hasOwnership(C* data) const {
        if (data == nullptr) {
            return false;
        }
        
        OwnedDataNode<C>* current = this->ownedHead;
        while (current != nullptr) {
            if (current->data() == data) {
                return true;
            }
            current = current->next();
        }
        
        return false;
    }
    
    /**
     * @brief Get the size of the collection
     * @return The number of elements in the collection
     */
    int getSize() const {
        return this->size;
    }
};

#endif // COLLECTION_H
