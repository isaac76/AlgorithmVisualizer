#ifndef GRAPHVISUALIZER_H
#define GRAPHVISUALIZER_H

#include <QObject>
#include <QWidget>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QColor>
#include <QLabel>
#include <QVBoxLayout>
#include "graph.h"
#include "circle.h"
#include "line.h"
#include "bfsvertex.h"
#include "bfs.h"
#include "dfsvertex.h"
#include "dfs.h"

// Utility function to convert VertexColor enum to QColor
inline QColor vertexColorToQColor(VertexColor color) {
    switch (color) {
        case white:
            return Qt::white;
        case gray:
            return Qt::gray;
        case black:
            return Qt::black;
        default:
            return Qt::white;
    }
}

class VisualVertexNotifier : public QObject {
    Q_OBJECT
signals:
    void colorChanged(VertexColor newColor);
    void hopsChanged(int newHops);
};

// Class to hold vertex data and its visual representation
class VisualVertex : public BfsVertex<VisualVertex> {
public:
    int value;
    Circle* circle;
    VisualVertexNotifier notifier;
    
    VisualVertex(int v, Circle* c) : value(v), circle(c) {
        setColor(white);
        setHops(-1);
    }

    // Override setColor to update the visual representation
    void setColor(VertexColor newColor) override {
        BfsVertex<VisualVertex>::setColor(newColor);
        if (circle) {
            circle->setColor(vertexColorToQColor(newColor));
            emit notifier.colorChanged(newColor);
        }
    }
    
    // Override setHops to update the circle's hop count
    void setHops(int newHops) override {
        BfsVertex<VisualVertex>::setHops(newHops);
        if (circle) {
            circle->setHopCount(newHops);
            emit notifier.hopsChanged(newHops);
        }
    }
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
    
    // Start BFS animation from the given start vertex
    void startBfsAnimation(int startValue);
    
    // Stop any ongoing BFS animation
    void stopBfsAnimation();
    
    // Reset all vertices to their initial state (white color, -1 hops)
    void resetBfsColors();
    
    // Start DFS animation
    void startDfsAnimation();
    
    // Stop any ongoing DFS animation
    void stopDfsAnimation();
    
    // Reset all vertices to their initial state for DFS (white color)
    void resetDfsColors();
    
    // Set the animation delay in milliseconds
    void setAnimationDelay(int delay);
    
    // Reposition the DFS order label (call this when parent widget is resized)
    void repositionDfsLabel();
    
    // Show the DFS order label
    void showDfsLabel();
    
    // Hide the DFS order label
    void hideDfsLabel();

private slots:
    // Handle vertex color change
    void onVertexColorChanged(VertexColor newColor);
    
    // Handle vertex hop count change
    void onVertexHopChanged(int newHops);
    
private:
    QWidget* area;
    Graph<VisualVertex, VisualVertexCompare> graph;
    QList<VisualVertex*> vertices;
    QList<Line*> lines;
    
    // BFS animation state
    enum BfsAnimationStep {
        NotRunning,
        Running,
        Completed
    };
    
    BfsAnimationStep animationStep = NotRunning;
    VisualVertex* startVertex = nullptr;
    int animationDelay = 200; // milliseconds between steps
    
    // DFS animation state
    enum DfsAnimationStep {
        DfsNotRunning,
        DfsRunning,
        DfsCompleted
    };
    
    DfsAnimationStep dfsAnimationStep = DfsNotRunning;
    QList<int> dfsOrderedList; // To store the order of DFS visit completion
    QLabel* dfsOrderLabel = nullptr; // To display the ordered list
    
    QPoint findNonOverlappingPosition(int w, int h) const;
    VisualVertex* findVertexByValue(int value);
};

#endif // GRAPHVISUALIZER_H
