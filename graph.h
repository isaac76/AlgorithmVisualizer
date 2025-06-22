#ifndef GRAPH_H
#define GRAPH_H

#include "list.h"
#include "listnode.h"
#include "adjacentlist.h"
#include "bfsvertex.h"
#include "collection.h"

template <class C, class Compare = std::equal_to<C>> class Graph : public Collection<C, Compare>
{
private:
    int vcount;
    int ecount;
protected:
    List<AdjacentList<C, Compare>> adjacentLists;
public:
    Graph() : Collection<C, Compare>() {
        this->vcount = 0;
        this->ecount = 0;
    }

    Graph(const Compare &comp) : Collection<C, Compare>(comp) {
        this->vcount = 0;
        this->ecount = 0;
    }
    
    ~Graph() override {
        AdjacentList<C, Compare>* adjList;
        while(this->adjacentLists.getSize() > 0) {
            this->adjacentLists.remove(nullptr, &adjList);
            // The vertex pointer in the adjacentList will be deleted by Collection if we own it
            delete adjList;
        }
        // The Collection base class destructor will clean up any owned vertices
    }

    int insertVertex(C* data, bool takeOwnership = false) {
        ListNode<AdjacentList<C, Compare>>* node;
        AdjacentList<C, Compare>* adjList = new AdjacentList<C, Compare>();

        // Check if the vertex already exists
        for (node = this->adjacentLists.head(); node != nullptr; node = node->next()) {
            if (data != nullptr && node->data() != nullptr) {
                if (this->equalTo(*data, *(node->data()->vertex))) {
                    return 1;
                }
            }
        }
        
        // Store the vertex in the adjacent list
        adjList->vertex = data;
        
        // Add the adjacent list to our collection
        this->adjacentLists.insert(this->adjacentLists.tail(), adjList);
        
        // Take ownership of the vertex data if requested
        if (takeOwnership && data != nullptr) {
            this->takeOwnership(data);
        }
        
        this->vcount++;
        return 0;
    }

    int insertEdge(C* data1, C* data2, bool takeOwnership = false) {
        ListNode<AdjacentList<C, Compare>>* node;

        // Find destination vertex
        for (node = this->adjacentLists.head(); node != nullptr; node = node->next()) {
            if (data2 != nullptr && node->data() != nullptr) {
                if (this->equalTo(*data2, *(node->data()->vertex))) {
                    break;
                }
            }
        }

        if (node == nullptr) {
            return -1;  // Destination vertex not found
        }

        // Find source vertex
        for (node = this->adjacentLists.head(); node != nullptr; node = node->next()) {
            if (data1 != nullptr && node->data() != nullptr) {
                if (this->equalTo(*data1, *(node->data()->vertex))) {
                    break;
                }
            }
        }

        if (node == nullptr) {
            return -1;  // Source vertex not found
        }
        
        // Insert the edge (create relationship in adjacent list)
        node->data()->adjacent.insert(data2, takeOwnership);
        
        this->ecount++;
        return 0;
    }

    int removeVertex(C** data) {
        if (data == nullptr || *data == nullptr) {
            return -1; // Invalid input
        }

        ListNode<AdjacentList<C, Compare>>* current = nullptr;
        ListNode<AdjacentList<C, Compare>>* prev = nullptr;
        AdjacentList<C, Compare>* adjList = nullptr;
        C* vertexToRemove = *data;
        bool found = false;

        // First, check if the vertex is used in any adjacency list (incoming edges)
        for (current = this->adjacentLists.head(); current != nullptr; current = current->next()) {
            if (current->data()->adjacent.isMember(vertexToRemove)) {
                return -1;  // Vertex is referenced by other vertices
            }
        }

        // Find the vertex to remove
        for (current = this->adjacentLists.head(); current != nullptr; current = current->next()) {
            if (vertexToRemove != nullptr && current->data() != nullptr) {
                if (this->equalTo(*vertexToRemove, *(current->data()->vertex))) {
                    found = true;
                    break;
                }
            }
            prev = current;
        }

        if (!found) {
            return -1;  // Vertex not found
        }

        // Check if the vertex has outgoing edges
        if (current->data()->adjacent.getSize() > 0) {
            return -1;  // Vertex has edges, can't remove
        }

        // Remove the adjacent list from our collection
        if (prev == nullptr) {
            // It's the head node
            this->adjacentLists.remove(nullptr, &adjList);
        } else {
            // It's not the head node
            this->adjacentLists.remove(prev, &adjList);
        }

        // Store the vertex data for the caller
        *data = adjList->vertex;
        
        // Release ownership of the vertex data if we owned it
        if (this->hasOwnership(*data)) {
            this->releaseOwnership(*data);
        }
        
        delete adjList;

        this->vcount--;
        return 0;
    }

    int removeEdge(C* data1, C** data2) {
        if (data1 == nullptr || data2 == nullptr || *data2 == nullptr) {
            return -1; // Invalid input
        }

        ListNode<AdjacentList<C, Compare>>* node;

        // Find the source vertex
        for (node = this->adjacentLists.head(); node != nullptr; node = node->next()) {
            if (data1 != nullptr && node->data() != nullptr) {
                if (this->equalTo(*data1, *(node->data()->vertex))) {
                    break;
                }
            }
        }

        if (node == nullptr) {
            return -1;  // Source vertex not found
        }

        // The Set::remove method returns true if the element was found and removed
        if (node->data()->adjacent.remove(*data2)) {
            // Successfully removed
            this->ecount--;
            return 0;
        } else {
            // Edge not found
            return -1;
        }
    }

    int buildAdjacentList(C* data, AdjacentList<C, Compare>** adjList) {
        ListNode<AdjacentList<C, Compare>>* node;
        ListNode<AdjacentList<C, Compare>>* prev = nullptr;

        for (node = this->adjacentLists.head(); node != nullptr; node = node->next()) {
            if (data != nullptr && node->data() != nullptr) {
                if (this->equalTo(*data, *(node->data()->vertex))) {
                    break;
                }
            }
            prev = node;
        }

        if (node == nullptr) {
            return -1;
        }

        *adjList = node->data();
        return 0;
    }

    bool isAdjacentGraph(C* data1, C* data2) {
        ListNode<AdjacentList<C, Compare>>* node;
        ListNode<AdjacentList<C, Compare>>* prev = nullptr;

        for (node = this->adjacentLists.head(); node != nullptr; node = node->next()) {
            if (data1 != nullptr && node->data() != nullptr) {
                if (this->equalTo(*data1, *(node->data()->vertex))) {
                    break;
                }
            }
            prev = node;
        }

        if (node == nullptr) {
            return false;
        }

        return node->data()->adjacent.isMember(data2);
    }
    
    int getVertexCount() const {
        return this->vcount;
    }

    int getEdgeCount() const {
        return this->ecount;
    }

    ListNode<AdjacentList<C, Compare>>* findNodeByVertex(C* data) {
        ListNode<AdjacentList<C, Compare>>* node;
        
        for (node = this->adjacentLists.head(); node != nullptr; node = node->next()) {
            if (data != nullptr && node->data() != nullptr) {
                if (this->equalTo(*data, *(node->data()->vertex))) {
                    return node;
                }
            }
        }
        
        return nullptr;
    }
    
    // Getter for adjacentLists, returns the head node of the list
    ListNode<AdjacentList<C, Compare>>* getAdjacencyListHead() const {
        return this->adjacentLists.head();
    }
    
    // Getter to check if the vertex list is empty
    bool isAdjacencyListEmpty() const {
        return this->adjacentLists.getSize() == 0;
    }
};

#endif // GRAPH_H
