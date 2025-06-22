#ifndef SET_H
#define SET_H

#include <functional>

#include <QDebug>

#include "setnode.h"
#include "collection.h"

template <class C, class Compare = std::equal_to<C>>
class Set : public Collection<C, Compare>
{
private:
    SetNode<C> *h; // Head pointer
    SetNode<C> *t; // Tail pointer

public:
    // Constructors and destructor
    Set() : Collection<C, Compare>()
    {
        this->h = nullptr;
        this->t = nullptr;
    }

    Set(const Compare &comp) : Collection<C, Compare>(comp)
    {
        this->h = nullptr;
        this->t = nullptr;
    }

    ~Set() override
    {
        SetNode<C> *current = this->h;
        SetNode<C> *next;

        // Clean up all nodes but not the data
        // The Collection base class will handle data deletion if it's owned
        while (current != nullptr)
        {
            next = current->next();
            delete current;
            current = next;
        }

        this->h = nullptr;
        this->t = nullptr;
    }

    // Get the size of the set
    int getSize() const
    {
        return this->size;
    }

    // Get the head node
    SetNode<C> *head() const
    {
        return this->h;
    }

    // Get the tail node
    SetNode<C> *tail() const
    {
        return this->t;
    }

    // Set the head node
    void setHead(SetNode<C> *node)
    {
        this->h = node;
    }

    // Set the tail node
    void setTail(SetNode<C> *node)
    {
        this->t = node;
    }

    // Check if an element is in the set
    bool isMember(const C *data) const
    {
        SetNode<C> *current;

        for (current = this->head(); current != nullptr; current = current->next())
        {
            if (data != nullptr && current->data() != nullptr) {
                if (this->equalTo(*data, *(current->data()))) {
                    return true;
                }
            }
            else
            {
                if (data == current->data())
                {
                    return true;
                }
            }
        }

        return false;
    }

    // Insert an element into the set if it doesn't already exist
    void insert(C *data, bool takeOwnership = false)
    {
        // Only insert if the element isn't already in the set
        if (!this->isMember(data))
        {
            SetNode<C> *newNode = new SetNode<C>();
            newNode->setData(data);
            newNode->setNext(nullptr);

            if (this->size == 0)
            {
                // First node in the set
                this->h = newNode;
                this->t = newNode;
            }
            else
            {
                // Add to the end of the set
                this->t->setNext(newNode);
                this->t = newNode;
            }

            // Take ownership if requested
            if (takeOwnership && data != nullptr)
            {
                this->takeOwnership(data);
            }

            this->size++;
        }
    }

    // Remove an element from the set
    bool remove(C *data)
    {
        SetNode<C> *current;
        SetNode<C> *previous = nullptr;

        // Find the node containing data
        for (current = this->h; current != nullptr; current = current->next())
        {
            if (this->equalTo(*data, *(current->data()))) {
                break;
            }
            previous = current;
        }

        // Return false if not found
        if (current == nullptr)
        {
            return false;
        }

        // Remove the node
        if (previous == nullptr)
        {
            // Removing the head node
            this->h = current->next();
        }
        else
        {
            // Removing an internal or tail node
            previous->setNext(current->next());
        }

        // Update tail if removing the last node
        if (current->next() == nullptr)
        {
            this->t = previous;
        }

        // Release ownership if we owned the data
        if (this->hasOwnership(current->data()))
        {
            this->releaseOwnership(current->data());
        }

        // Clean up and update size
        delete current;
        this->size--;

        return true;
    }

    // Set operations
    static void unionSet(Set<C, Compare> *result, const Set<C, Compare> *set1, const Set<C, Compare> *set2)
    {
        SetNode<C> *current;

        // Copy all elements from set1
        for (current = set1->h; current != nullptr; current = current->next())
        {
            result->insert(current->data());
        }

        // Copy elements from set2 that aren't in set1
        for (current = set2->h; current != nullptr; current = current->next())
        {
            if (!set1->isMember(current->data()))
            {
                result->insert(current->data());
            }
        }
    }

    static void intersectionSet(Set<C, Compare> *result, const Set<C, Compare> *set1, const Set<C, Compare> *set2)
    {
        SetNode<C> *current;

        // Find elements that are in both sets
        for (current = set1->h; current != nullptr; current = current->next())
        {
            if (set2->isMember(current->data()))
            {
                result->insert(current->data());
            }
        }
    }

    static void differenceSet(Set<C, Compare> *result, const Set<C, Compare> *set1, const Set<C, Compare> *set2)
    {
        SetNode<C> *current;

        // Find elements in set1 that aren't in set2
        for (current = set1->h; current != nullptr; current = current->next())
        {
            if (!set2->isMember(current->data()))
            {
                result->insert(current->data());
            }
        }
    }

    static bool isSubset(const Set<C, Compare> *set1, const Set<C, Compare> *set2)
    {
        SetNode<C> *current;

        // If set1 is larger than set2, it can't be a subset
        if (set1->size > set2->size)
        {
            return false;
        }

        // Check if every element in set1 is also in set2
        for (current = set1->h; current != nullptr; current = current->next())
        {
            if (!set2->isMember(current->data()))
            {
                return false;
            }
        }

        return true;
    }

    static bool isEqualSet(const Set<C, Compare> *set1, const Set<C, Compare> *set2)
    {
        // Sets are equal if they have the same size and set1 is a subset of set2
        return (set1->size == set2->size) && isSubset(set1, set2);
    }
};

#endif // SET_H
