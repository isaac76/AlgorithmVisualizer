#include "graphvisualizer.h"
#include <QRandomGenerator>
#include <QtMath>
#include <thread>

GraphVisualizer::GraphVisualizer(QWidget* area, QObject* parent)
    : QObject(parent), area(area), animationStepTimer(nullptr)
{}

GraphVisualizer::~GraphVisualizer() {
    clear();
    
    // Delete the animation timer if it was created
    if (animationStepTimer) {
        animationStepTimer->stop();
        delete animationStepTimer;
        animationStepTimer = nullptr;
    }
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

    // Check if there's already an edge in the opposite direction (to->from)
    // If so, we need to offset both lines to avoid overlap
    bool oppositeDirectionExists = graph.isAdjacentGraph(to, from);
    
    Line* line = new Line(area);
    line->connectWidgets(from->circle, to->circle);

    QPointF dir = end - start;
    QPointF perp(-dir.y(), dir.x()); // Perpendicular vector
    perp /= std::hypot(perp.x(), perp.y()); // Normalize it
    QPointF mid = (start + end) / 2.0;
    
    if (needsBend) {
        // Bend: set control point perpendicular to the line, offset by the circle's radius + margin
        double margin = avoidRadius + 20;
        // Choose bend direction based on which side is farther from the avoidCenter
        QPointF candidate1 = mid + perp * margin;
        QPointF candidate2 = mid - perp * margin;
        double dist1 = QLineF(candidate1, avoidCenter).length();
        double dist2 = QLineF(candidate2, avoidCenter).length();
        QPointF control = (dist1 > dist2) ? candidate1 : candidate2;
        line->setControlPoint(control);
    } 
    else if (oppositeDirectionExists) {
        // Offset this edge to one side to avoid overlap with the opposite direction
        double offsetAmount = 15.0; // Adjust as needed for visibility
        QPointF control = mid + perp * offsetAmount;
        line->setControlPoint(control);
        
        // Also find and adjust the opposite direction line if needed
        for (Line* existingLine : lines) {
            if (existingLine->getStartWidget() == to->circle && 
                existingLine->getEndWidget() == from->circle) {
                // This is the opposite direction line
                // Offset it to the other side
                QPointF oppositeControl = mid - perp * offsetAmount;
                existingLine->setControlPoint(oppositeControl);
                existingLine->update();
                break;
            }
        }
    } 
    else {
        // For a standard line (no bend, no opposite direction)
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

// This method has been replaced with a free function in graphvisualizer.h

// Add this helper method to find a vertex by value
VisualVertex* GraphVisualizer::findVertexByValue(int value)
{
    for (VisualVertex* v : vertices) {
        if (v->value == value) {
            return v;
        }
    }
    return nullptr;
}

void GraphVisualizer::startBfsAnimation(int startValue)
{
    // Find the start vertex
    startVertex = findVertexByValue(startValue);
    if (!startVertex) {
        emit bfsStatusMessage("Start vertex not found!");
        return;
    }
    
    // Reset all vertices to initial state
    resetBfsColors();
    
    // Restore the start vertex since resetBfsColors clears it
    startVertex = findVertexByValue(startValue);
    animationStep = Running;
    
    emit bfsStatusMessage("BFS Starting from vertex " + QString::number(startVertex->value));
    
    // Set up the animation timer if not already created
    if (!animationStepTimer) {
        animationStepTimer = new QTimer(this);
        connect(animationStepTimer, &QTimer::timeout, this, &GraphVisualizer::performBfsStep);
    }
    
    // Initialize BFS state
    bfsQueue.clear();
    exploredVertices.clear();
    
    // Add start vertex to the queue
    bfsQueue.append(startVertex);
    startVertex->setColor(gray); // Mark as discovered
    startVertex->setHops(0);     // Distance from start is 0
    
    // Start the timer to process BFS steps
    animationStepTimer->start(animationDelay);
}

void GraphVisualizer::stopBfsAnimation()
{
    // Stop the animation timer if it's running
    if (animationStepTimer && animationStepTimer->isActive()) {
        animationStepTimer->stop();
    }
    
    // Reset animation state
    animationStep = NotRunning;
    bfsQueue.clear();
    exploredVertices.clear();
    
    // Clear the start vertex pointer - NOTE: Any code calling this method needs
    // to be aware that this clears the startVertex pointer
    startVertex = nullptr;
}

void GraphVisualizer::performBfsStep()
{
    // If the queue is empty, BFS is completed
    if (bfsQueue.isEmpty()) {
        animationStepTimer->stop();
        animationStep = Completed;
        emit bfsStatusMessage("BFS completed successfully");
        return;
    }
    
    // Get the next vertex from the queue (first-in, first-out)
    VisualVertex* currentVertex = bfsQueue.takeFirst();
    
    // Process this vertex - find all its adjacent vertices
    QList<VisualVertex*> adjacentVertices;
    
    // Build list of adjacent vertices
    for (VisualVertex* v : vertices) {
        if (graph.isAdjacentGraph(currentVertex, v) && !exploredVertices.contains(v)) {
            adjacentVertices.append(v);
        }
    }
    
    // Update status message
    emit bfsStatusMessage("Exploring vertex " + QString::number(currentVertex->value) + 
                         " with " + QString::number(adjacentVertices.size()) + " unvisited neighbors");
    
    // Process each adjacent vertex
    for (VisualVertex* adjVertex : adjacentVertices) {
        if (adjVertex->getColor() == white) {
            // First discovery of this vertex
            adjVertex->setColor(gray);
            adjVertex->setHops(currentVertex->getHops() + 1);
            bfsQueue.append(adjVertex);
        }
    }
    
    // Mark current vertex as completely processed
    currentVertex->setColor(black);
    exploredVertices.append(currentVertex);
}

void GraphVisualizer::resetBfsColors()
{
    // Reset all vertices to white color and -1 hops (not visited)
    for (VisualVertex* v : vertices) {
        if (v) {
            v->setColor(white);
            v->setHops(-1);
        }
    }
    
    // Reset animation state
    stopBfsAnimation();
    
    // Signal that BFS has been reset
    emit bfsStatusMessage("BFS visualization reset");
}

void GraphVisualizer::setAnimationDelay(int delay)
{
    animationDelay = delay;
    
    // If the animation timer is active, update its interval
    if (animationStepTimer && animationStepTimer->isActive()) {
        animationStepTimer->setInterval(delay);
    }
}
