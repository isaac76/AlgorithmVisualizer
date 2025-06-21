#ifndef ADJACENTLIST_H
#define ADJACENTLIST_H

#include "set.h"

template<class C, class Compare = std::equal_to<C>> class AdjacentList
{
public:
    C* vertex;
    Set<C, Compare> adjacent;

    AdjacentList(const Compare& comp) : vertex(nullptr), adjacent(comp) {
        this->vertex = nullptr;
    }
    
    ~AdjacentList() {
        // The vertex is not owned by the adjacent list
        // and will be deleted by whoever owns it
    }
};

#endif // ADJACENTLIST_H
