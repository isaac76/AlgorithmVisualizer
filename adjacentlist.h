#ifndef ADJACENTLIST_H
#define ADJACENTLIST_H

#include "set.h"

template<class C, class Compare = std::equal_to<C>> class AdjacentList
{
public:
    C* vertex;
    Set<C, Compare> adjacent;
    
    // Default constructor
    AdjacentList() : vertex(nullptr), adjacent() {
        
    }
    
    // Constructor with Compare object
    AdjacentList(const Compare& comp) : vertex(nullptr), adjacent(comp) {
        
    }
    
    ~AdjacentList() {
        // The vertex is not owned by the adjacent list
        // and will be deleted by whoever owns it
    }
};

#endif // ADJACENTLIST_H
