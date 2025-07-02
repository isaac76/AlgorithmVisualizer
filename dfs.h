#ifndef DFS_H
#define DFS_H

#include <functional>

#include "dfsvertex.h"
#include "list.h"
#include "listnode.h"
#include "setnode.h"
#include "adjacentlist.h"
#include "graph.h"

template<typename T, typename Compare = std::equal_to<T>>
int dfsMain(Graph<T, Compare>* graph, AdjacentList<T, Compare>* adjList, List<T, Compare>* ordered) {
    AdjacentList<T, Compare>* clrAdjList;
    T* clrVertex;
    T* adjVertex;
    SetNode<T>* member;

    adjList->vertex->setColor(gray);

    for (member = adjList->adjacent.head();member != nullptr;member = member->next()) {
        adjVertex = member->data();

        graph->buildAdjacentList(adjVertex, &clrAdjList);

        clrVertex = clrAdjList->vertex;

        if (clrVertex->getColor() == white) {
            if (dfsMain(graph, clrAdjList, ordered) != 0) {
                return -1;
            }
        }
    }

    adjList->vertex->setColor(black);

    ordered->insert(nullptr, adjList->vertex);

    return 0;
}

template<typename T, typename Compare = std::equal_to<T>>
int dfs(Graph<T, Compare>* graph, List<T, Compare>& ordered) {
    T* vertex;

    ListNode<AdjacentList<T, Compare>>* node;

    for (node = graph->getAdjacencyListHead();node != nullptr;node = node->next()) {
        vertex = node->data()->vertex;
        vertex->setColor(white);
    }

    ordered.clear();

    for (node = graph->getAdjacencyListHead();node != nullptr;node = node->next()) {
        vertex = node->data()->vertex;

        if (vertex->getColor() == white) {
            if (dfsMain(graph, node->data(), &ordered) != 0) {
                return -1;
            }
        }
    }

    return 0;
}

#endif // DFS_H
