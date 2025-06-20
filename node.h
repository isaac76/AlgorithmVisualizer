#ifndef NODE_H
#define NODE_H


template<class C> class Node
{
protected:
    C* d;
public:
    Node() {
        d = nullptr;
    }

    C* data() {
        return d;
    }

    void setData(C* d) {
        this->d = d;
    }
};

#endif // NODE_H
