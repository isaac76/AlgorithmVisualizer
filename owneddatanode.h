#ifndef OWNEDDATANODE_H
#define OWNEDDATANODE_H

/**
 * @class OwnedDataNode
 * @brief Node class for tracking owned data pointers in Collections
 * 
 * This class is used by the Collection base class to implement ownership tracking
 * of data elements. Each node contains a pointer to data and a pointer to the next node.
 */
template<class C> class OwnedDataNode
{
private:
    C* data_field;              // Pointer to the data owned by this node
    OwnedDataNode<C>* next_field;  // Pointer to the next node

public:
    /**
     * @brief Default constructor
     */
    OwnedDataNode() {
        this->data_field = nullptr;
        this->next_field = nullptr;
    }
    
    /**
     * @brief Constructor with data
     * @param data Pointer to the data to be tracked
     */
    OwnedDataNode(C* data) {
        this->data_field = data;
        this->next_field = nullptr;
    }
    
    /**
     * @brief Destructor
     * 
     * This destructor does not delete the data or the next node.
     * Data deletion is handled by the Collection class.
     */
    ~OwnedDataNode() {
        // The node destructor doesn't delete the data or the next node
        // This is handled by the Collection class
    }
    
    /**
     * @brief Set the data pointer
     * @param data Pointer to the data
     */
    void setData(C* data) {
        this->data_field = data;
    }
    
    /**
     * @brief Get the data pointer
     * @return Pointer to the data
     */
    C* data() const {
        return this->data_field;
    }
    
    /**
     * @brief Set the next node
     * @param next Pointer to the next node
     */
    void setNext(OwnedDataNode<C>* next) {
        this->next_field = next;
    }
    
    /**
     * @brief Get the next node
     * @return Pointer to the next node
     */
    OwnedDataNode<C>* next() const {
        return this->next_field;
    }
};

#endif // OWNEDDATANODE_H
