#ifndef BFS_H
#define BFS_H

#include "list.h"
#include "listnode.h"
#include "queue.h"
#include "graph.h"
#include "bfsvertex.h"
#include "set.h"

template<typename T> int bfs(Graph<BfsVertex<T>>* graph, BfsVertex<T>* start, List<BfsVertex<T>>* hops) {
    if (!graph || !start) {
        return -1; // Invalid parameters
    }

    Queue<AdjacentList<BfsVertex<T>>> queue;

    AdjacentList<BfsVertex<T>>* adjList = nullptr;
    AdjacentList<BfsVertex<T>>* clrAdjList;

    BfsVertex<T>* clrVertex;
    BfsVertex<T>* adjVertex;

    ListNode<AdjacentList<BfsVertex<T>>>* node;

    // Initialize all vertices
    for (node = graph->getAdjacencyListHead(); node != nullptr; node = node->next()) {
        clrVertex = node->data()->vertex;

        if (BfsVertex<T>::compare(clrVertex, start)) {
            clrVertex->color = gray;
            clrVertex->hops = 0;
        } else {
            clrVertex->color = white;
            clrVertex->hops = -1;
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
            adjVertex = static_cast<BfsVertex<T>*>(member->data());

            // Get the adjacent list for this vertex
            if (graph->buildAdjacentList(adjVertex, &clrAdjList) == 0) {
                clrVertex = clrAdjList->vertex;

                if (clrVertex->color == white) {
                    clrVertex->color = gray;
                    clrVertex->hops = adjList->vertex->hops + 1;

                    queue.enqueue(clrAdjList);
                }
            }
        }

        queue.dequeue(&adjList);
        adjList->vertex->color = black;
    }

    // Clear the hop list if it exists; otherwise initialize it
    if (hops == nullptr) {
        // If hops is null, create a new list
        hops = new List<BfsVertex<T>>();
    } else {
        // If hops already exists, clear it by removing all items
        BfsVertex<T>* temp;
        while (hops->getSize() > 0) {
            hops->remove(nullptr, &temp);
        }
    }

    // Add all reachable vertices to the hop list
    for (node = graph->getAdjacencyListHead(); node != nullptr; node = node->next()) {
        clrVertex = node->data()->vertex;

        if (clrVertex->hops != -1) {
            hops->insert(hops->tail(), clrVertex);
        }
    }

    return 0;
}

#endif // BFS_H
