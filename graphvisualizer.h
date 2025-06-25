#ifndef GRAPHVISUALIZER_H
#define GRAPHVISUALIZER_H

#include <QObject>
#include <QWidget>
#include <QList>
#include <QMap>
#include "graph.h"
#include "circle.h"
#include "line.h"
#include "bfsvertex.h"

// Simple struct to hold vertex data and its visual representation
struct VisualVertex : public BfsVertex<VisualVertex> {
    int value;
    Circle* circle;
    VisualVertex(int v, Circle* c) : value(v), circle(c) {}
};

// Comparator for VisualVertex (by value)
struct VisualVertexCompare {
    bool operator()(const VisualVertex& a, const VisualVertex& b) const {
        return a.value == b.value;
    }
};

class GraphVisualizer : public QObject {
    Q_OBJECT
public:
    explicit GraphVisualizer(QWidget* area, QObject* parent = nullptr);
    ~GraphVisualizer();

    // Add a vertex visually and to the graph
    VisualVertex* addVertex(int value);

    // Add an edge visually and to the graph
    void addEdge(VisualVertex* from, VisualVertex* to);

    // Remove a vertex and its visual representation
    void removeVertex(VisualVertex* vertex);

    // Remove an edge and its visual representation
    void removeEdge(VisualVertex* from, VisualVertex* to);

    // Clear all
    void clear();

    // Access to all vertices and edges
    QList<VisualVertex*> getVertices() const;
    QList<Line*> getLines() const;
    
    // Utility method to convert VertexColor enum to QColor
    static QColor vertexColorToQColor(VertexColor color);
    
private:
    QWidget* area;
    Graph<VisualVertex, VisualVertexCompare> graph;
    QList<VisualVertex*> vertices;
    QList<Line*> lines;

    QPoint findNonOverlappingPosition(int w, int h) const;
};

#endif // GRAPHVISUALIZER_H
