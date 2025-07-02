#ifndef DFSVERTEX_H
#define DFSVERTEX_H

#include "bfsvertex.h"

template<class C> class DfsVertex
{
protected:
    VertexColor color;
    int hops;

public:
    C* data;
    
    DfsVertex() {
        this->data = nullptr;
        this->color = white;
        this->hops = -1;
    }
    
    DfsVertex(C* data) {
        this->data = data;
        this->color = white;
        this->hops = -1;
    }
    
    // Method for comparing vertex data
    static int compare(const DfsVertex<C>* v1, const DfsVertex<C>* v2) {
        // Implement comparison logic based on your data type
        // For pointer comparison:
        return (v1->data == v2->data) ? 1 : 0;
    }
    
    // Getter for color
    virtual VertexColor getColor() const {
        return this->color;
    }
    
    // Setter for color
    virtual void setColor(VertexColor newColor) {
        this->color = newColor;
    }
    
    // Getter for hops
    virtual int getHops() const {
        return this->hops;
    }
    
    // Setter for hops
    virtual void setHops(int newHops) {
        this->hops = newHops;
    }
};

#endif // DFSVERTEX_H
