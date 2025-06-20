#ifndef SETNODE_H
#define SETNODE_H

template <class C> class SetNode
{
private:
    C* data_field;
    SetNode<C>* next_field;

public:
    SetNode() {
        this->data_field = nullptr;
        this->next_field = nullptr;
    }
    
    ~SetNode() {
        // Node destructor doesn't delete the data or next node
        // This is handled by the Set class
    }
    
    void setData(C* data) {
        this->data_field = data;
    }
    
    C* data() const {
        return this->data_field;
    }
    
    void setNext(SetNode<C>* next) {
        this->next_field = next;
    }
    
    SetNode<C>* next() const {
        return this->next_field;
    }
};

#endif // SETNODE_H
