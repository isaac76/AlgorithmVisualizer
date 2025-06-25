#include "graphvisualizer.h"
#include <QRandomGenerator>
#include <QtMath>

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

// Helper to check if a line segment (p1-p2) intersects a circle (center, radius)
static bool lineIntersectsCircle(const QPointF& p1, const QPointF& p2, const QPointF& center, double radius) {
    // Vector from p1 to p2
    QPointF d = p2 - p1;
    QPointF f = p1 - center;
    double a = QPointF::dotProduct(d, d);
    double b = 2 * QPointF::dotProduct(f, d);
    double c = QPointF::dotProduct(f, f) - radius * radius;
    double discriminant = b*b - 4*a*c;
    if (discriminant < 0) return false;
    discriminant = sqrt(discriminant);
    double t1 = (-b - discriminant) / (2*a);
    double t2 = (-b + discriminant) / (2*a);
    return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
}

void GraphVisualizer::addEdge(VisualVertex* from, VisualVertex* to)
{
    if (!from || !to || !from->circle || !to->circle) return;
    if (graph.isAdjacentGraph(from, to)) return;

    QPointF start = from->circle->geometry().center();
    QPointF end = to->circle->geometry().center();

    // Check for intersection with other circles
    bool needsBend = false;
    QPointF avoidCenter;
    double avoidRadius = 0;
    for (VisualVertex* v : vertices) {
        if (v == from || v == to) continue;
        QPointF center = v->circle->geometry().center();
        double radius = v->circle->width() / 2.0;
        if (lineIntersectsCircle(start, end, center, radius)) {
            needsBend = true;
            avoidCenter = center;
            avoidRadius = radius;
            break;
        }
    }

    Line* line = new Line(area);
    line->connectWidgets(from->circle, to->circle);

    if (needsBend) {
        // Bend: set control point perpendicular to the line, offset by the circle's radius + margin
        QPointF mid = (start + end) / 2.0;
        QPointF dir = end - start;
        QPointF perp(-dir.y(), dir.x());
        perp /= std::hypot(perp.x(), perp.y());
        double margin = avoidRadius + 20;
        // Choose bend direction based on which side is farther from the avoidCenter
        QPointF candidate1 = mid + perp * margin;
        QPointF candidate2 = mid - perp * margin;
        double dist1 = QLineF(candidate1, avoidCenter).length();
        double dist2 = QLineF(candidate2, avoidCenter).length();
        QPointF control = (dist1 > dist2) ? candidate1 : candidate2;
        line->setControlPoint(control); // Only set control point if needed
    } else {
        // For a straight line, set control point to the midpoint (default for quadratic Bezier)
        QPointF mid = (start + end) / 2.0;
        line->setControlPoint(mid);
    }

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

    // Only remove the line that connects 'from' and 'to'
    for (int i = lines.size() - 1; i >= 0; --i) {
        Line* line = lines[i];
        // Check if this line connects the correct widgets
        if ((line->getStartWidget() == from->circle && line->getEndWidget() == to->circle) ||
            (line->getStartWidget() == to->circle && line->getEndWidget() == from->circle)) {
            line->hide();
            delete line;
            lines.removeAt(i);
            break; // Remove only one matching edge
        }
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

QColor GraphVisualizer::vertexColorToQColor(VertexColor color) {
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
