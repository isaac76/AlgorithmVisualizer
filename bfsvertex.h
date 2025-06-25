#ifndef BFSVERTEX_H
#define BFSVERTEX_H

typedef enum VertexColor_ {white, gray, black} VertexColor;

template<class C> class BfsVertex
{
protected:
    VertexColor color;
    int hops;

public:
    C* data;
    
    BfsVertex() {
        this->data = nullptr;
        this->color = white;
        this->hops = -1;
    }
    
    BfsVertex(C* data) {
        this->data = data;
        this->color = white;
        this->hops = -1;
    }
    
    // Method for comparing vertex data
    static int compare(const BfsVertex<C>* v1, const BfsVertex<C>* v2) {
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

#endif // BFSVERTEX_H
