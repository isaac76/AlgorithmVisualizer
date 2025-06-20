#ifndef BFSVERTEX_H
#define BFSVERTEX_H

typedef enum VertexColor_ {white, gray, black} VertexColor;

template<class C> class BfsVertex
{
public:
    C* data;
    VertexColor color;
    int hops;
    
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
};

#endif // BFSVERTEX_H
