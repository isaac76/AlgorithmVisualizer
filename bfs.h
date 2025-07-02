#ifndef BFS_H
#define BFS_H

#include "list.h"
#include "listnode.h"
#include "queue.h"
#include "graph.h"
#include "bfsvertex.h"
#include "set.h"
#include <type_traits>

template<typename T, typename Compare = std::equal_to<T>> 
int bfs(Graph<T, Compare>* graph, 
        T* start, 
        List<T>& hops) {
    // No static assertion - we allow both BfsVertex<T> directly and classes derived from BfsVertex<T>
                  
    if (!graph || !start) {
        return -1; // Invalid parameters
    }

    Queue<AdjacentList<T, Compare>> queue;

    AdjacentList<T, Compare>* adjList = nullptr;
    AdjacentList<T, Compare>* clrAdjList;

    T* clrVertex;
    T* adjVertex;

    ListNode<AdjacentList<T, Compare>>* node;

    // Initialize all vertices
    for (node = graph->getAdjacencyListHead(); node != nullptr; node = node->next()) {
        clrVertex = node->data()->vertex;

        // Use pointer equality for now since we can't guarantee BfsVertex<T>::compare will work with all T types
        if (clrVertex == start) {
            clrVertex->setColor(gray);
            clrVertex->setHops(0);
        } else {
            clrVertex->setColor(white);
            clrVertex->setHops(-1);
        }
    }

    // Get the adjacent list of the starting vertex
    if (graph->buildAdjacentList(start, &clrAdjList) != 0) {
        return -1;  // Starting vertex not found
    }

    // Start BFS with the starting vertex
    queue.enqueue(clrAdjList);

    while(queue.getSize() > 0) {
        adjList = queue.peek();

        // Process all adjacent vertices
        for (auto member = adjList->adjacent.head(); member != nullptr; member = member->next()) {
            adjVertex = static_cast<T*>(member->data());

            // Get the adjacent list for this vertex
            if (graph->buildAdjacentList(adjVertex, &clrAdjList) == 0) {
                clrVertex = clrAdjList->vertex;

                if (clrVertex->getColor() == white) {
                    clrVertex->setColor(gray);
                    clrVertex->setHops(adjList->vertex->getHops() + 1);

                    queue.enqueue(clrAdjList);
                }
            }
        }

        queue.dequeue(&adjList);
        adjList->vertex->setColor(black);
    }

    hops.clear();

    // Add all reachable vertices to the hop list
    for (node = graph->getAdjacencyListHead(); node != nullptr; node = node->next()) {
        clrVertex = node->data()->vertex;

        if (clrVertex->getHops() != -1) {
            hops.insert(hops.tail(), clrVertex);
        }
    }

    return 0;
}

#endif // BFS_H
