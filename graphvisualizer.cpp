#include "graphvisualizer.h"
#include <QRandomGenerator>

GraphVisualizer::GraphVisualizer(QWidget* area, QObject* parent)
    : QObject(parent), area(area)
{}

GraphVisualizer::~GraphVisualizer() {
    clear();
}

VisualVertex* GraphVisualizer::addVertex(int value) {
    Circle* circle = new Circle(value, area);
    QPoint pos = findNonOverlappingPosition(circle->width(), circle->height());
    circle->setGeometry(pos.x(), pos.y(), circle->width(), circle->height());
    circle->show();

    VisualVertex* v = new VisualVertex(value, circle);
    vertices.append(v);
    graph.insertVertex(v, true);
    return v;
}

void GraphVisualizer::addEdge(VisualVertex* from, VisualVertex* to) {
    if (!from || !to) return;
    if (graph.isAdjacentGraph(from, to)) return;

    Line* line = new Line(area);
    line->connectWidgets(from->circle, to->circle);
    line->setGeometry(0, 0, area->width(), area->height());
    line->show();
    lines.append(line);

    graph.insertEdge(from, to);
}

void GraphVisualizer::removeVertex(VisualVertex* vertex) {
    if (!vertex) return;
    // Remove all lines connected to this vertex
    for (int i = lines.size() - 1; i >= 0; --i) {
        Line* line = lines[i];
        // You may want to store edge info to know which line connects which vertices
        // For now, just remove all lines (simplest)
        line->hide();
        delete line;
        lines.removeAt(i);
    }
    // Remove from graph and visual list
    graph.removeVertex(&vertex);
    vertices.removeOne(vertex);
    if (vertex->circle) {
        vertex->circle->hide();
        delete vertex->circle;
    }
    delete vertex;
}

void GraphVisualizer::removeEdge(VisualVertex* from, VisualVertex* to) {
    if (!from || !to) return;
    // Remove the visual line (simplest: remove all and redraw if needed)
    for (int i = lines.size() - 1; i >= 0; --i) {
        Line* line = lines[i];
        // No direct mapping, so just remove all lines between from and to
        // (You can improve this by storing edge info in a struct)
        line->hide();
        delete line;
        lines.removeAt(i);
    }
    graph.removeEdge(from, &to);
}

void GraphVisualizer::clear() {
    for (Line* line : lines) {
        line->hide();
        delete line;
    }
    lines.clear();
    for (VisualVertex* v : vertices) {
        if (v->circle) {
            v->circle->hide();
            delete v->circle;
        }
        delete v;
    }
    vertices.clear();
    // The graph will clean up owned vertices
    // (if you use ownership flags in insertVertex)
}

QList<VisualVertex*> GraphVisualizer::getVertices() const {
    return vertices;
}

QList<Line*> GraphVisualizer::getLines() const {
    return lines;
}

QPoint GraphVisualizer::findNonOverlappingPosition(int w, int h) const {
    int maxX = area->width() - w;
    int maxY = area->height() - h;
    maxX = qMax(maxX, 50);
    maxY = qMax(maxY, 50);
    for (int attempt = 0; attempt < 20; ++attempt) {
        int x = QRandomGenerator::global()->bounded(50, maxX);
        int y = QRandomGenerator::global()->bounded(50, maxY);
        bool overlap = false;
        for (VisualVertex* v : vertices) {
            if (v && v->circle) {
                int dx = v->circle->x() - x;
                int dy = v->circle->y() - y;
                int minDist = v->circle->width() + w;
                if (dx*dx + dy*dy < minDist*minDist/4) {
                    overlap = true;
                    break;
                }
            }
        }
        if (!overlap) return QPoint(x, y);
    }
    return QPoint(QRandomGenerator::global()->bounded(50, maxX),
                  QRandomGenerator::global()->bounded(50, maxY));
}
