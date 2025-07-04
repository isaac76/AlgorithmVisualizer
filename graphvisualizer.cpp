#include "graphvisualizer.h"
#include <QRandomGenerator>
#include <QtMath>
#include <thread>
#include <QApplication>
#include <QThread>
#include <QDebug>

GraphVisualizer::GraphVisualizer(QWidget* area, QObject* parent)
    : QObject(parent), area(area)
{
    // Create the DFS order label at initialization
    dfsOrderLabel = new QLabel(area->parentWidget());
    dfsOrderLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    dfsOrderLabel->setStyleSheet("background-color: white; border: 1px solid black; padding: 4px;");
    dfsOrderLabel->setFixedWidth(220);  // Wider to fit the explanation
    dfsOrderLabel->setWordWrap(true);   // Enable word wrap for longer explanations
    
    // Position the label on the right side of the parent to avoid overlap with graph
    QSize parentSize = area->parentWidget()->size();
    dfsOrderLabel->setGeometry(parentSize.width() - 230, 10, 220, 300);
    
    // Set default content
    dfsOrderLabel->setText("<b>DFS Order:</b><br>(No DFS run yet)");
    dfsOrderLabel->hide();
}

GraphVisualizer::~GraphVisualizer() {
    // Stop any running animations
    stopBfsAnimation();
    stopDfsAnimation();
    
    // Clean up DFS order label
    if (dfsOrderLabel) {
        dfsOrderLabel->hide();
        dfsOrderLabel->setParent(nullptr);
        dfsOrderLabel->deleteLater();
        dfsOrderLabel = nullptr;
    }
    
    // Clean up
    clear();
}

VisualVertex* GraphVisualizer::addVertex(int value) {
    // Create visual representation
    Circle* circle = new Circle(value, area);
    QPoint pos = findNonOverlappingPosition(circle->width(), circle->height());
    circle->setGeometry(pos.x(), pos.y(), circle->width(), circle->height());
    circle->show();

    QWidget* p = circle->parentWidget();

    // Create the vertex object that will be tracked by both the graph and our vertices list
    VisualVertex* v = new VisualVertex(value, circle);
    
    // Connect signals from the notifier to our slots
    connect(&v->notifier, &VisualVertexNotifier::colorChanged,
            this, &GraphVisualizer::onVertexColorChanged);
    connect(&v->notifier, &VisualVertexNotifier::hopsChanged,
            this, &GraphVisualizer::onVertexHopChanged);
    
    vertices.append(v);
    
    // Insert into the graph with ownership
    // This means the Graph will take responsibility for deleting the vertex
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
    
    // Remove only lines connected to this vertex
    for (int i = lines.size() - 1; i >= 0; --i) {
        Line* line = lines[i];
        if ((line->getStartWidget() == vertex->circle) || (line->getEndWidget() == vertex->circle)) {
            // This line is connected to the vertex we're removing
            line->disconnectWidgets();
            line->hide();
            line->setParent(nullptr); // Remove from parent
            line->deleteLater();
            lines.removeAt(i);
        }
    }

    // Hide the circle and schedule it for deletion
    if (vertex->circle) {
        vertex->circle->hide();
        vertex->circle->setParent(nullptr); // Remove from parent
        vertex->circle->deleteLater();
    }
    
    // Remove from visual list first
    vertices.removeOne(vertex);
    
    // Remove from graph - this will also delete the vertex since graph has ownership
    graph.removeVertex(&vertex);
    
    // Process pending deletions
    QApplication::processEvents();
}

void GraphVisualizer::removeEdge(VisualVertex* from, VisualVertex* to) {
    if (!from || !to) return;

    // Only remove the line that connects 'from' and 'to'
    for (int i = lines.size() - 1; i >= 0; --i) {
        Line* line = lines[i];
        // Check if this line connects the correct widgets
        if ((line->getStartWidget() == from->circle && line->getEndWidget() == to->circle) ||
            (line->getStartWidget() == to->circle && line->getEndWidget() == from->circle)) {
            line->disconnectWidgets();
            line->hide();
            line->setParent(nullptr); // Remove from parent
            line->deleteLater();
            lines.removeAt(i);
            break; // Remove only one matching edge
        }
    }
    
    // Process pending deletions
    QApplication::processEvents();
    
    graph.removeEdge(from, &to);
}

void GraphVisualizer::clear() {
    // First disconnect and schedule deletion of all Line objects
    for (Line* line : lines) {
        line->disconnectWidgets();
        line->hide();
        line->setParent(nullptr);
        line->deleteLater();
    }
    lines.clear();
    
    // Hide all Circle objects and schedule them for deletion
    for (VisualVertex* v : vertices) {
        if (v->circle) {
            v->circle->hide();
            v->circle->setParent(nullptr);
            v->circle->deleteLater();
        }
    }
    
    // Process pending deletions to make sure Circles are gone
    QApplication::processEvents();
    
    // Now clear the vertices list - Graph owns these objects and will delete them
    vertices.clear();
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
        return;
    }
    
    // Reset all vertices to initial state
    resetBfsColors();
    
    // Restore the start vertex since resetBfsColors clears it
    startVertex = findVertexByValue(startValue);
    animationStep = Running;
    
    // Create a list to store the result
    List<VisualVertex> hopList;
    
    // Run the standard BFS algorithm - this will automatically update colors and hop counts
    // through calls to setColor and setHops on each VisualVertex
    int result = bfs(&graph, startVertex, hopList);
    
    if (result != 0) {
        return;
    }
    
    // Signal completion
    animationStep = Completed;
}

void GraphVisualizer::stopBfsAnimation()
{
    // Reset animation state
    animationStep = NotRunning;
    
    // Clear the start vertex pointer
    startVertex = nullptr;
}

// performBfsStep has been removed as we now use the standard BFS algorithm

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
}

void GraphVisualizer::setAnimationDelay(int delay)
{
    // Ensure delay is reasonable (between 10ms and 2000ms)
    animationDelay = qBound(10, delay, 500);
}

void GraphVisualizer::onVertexColorChanged(VertexColor newColor)
{
    // When vertex color changes, update the UI and add a delay for animation
    if (animationStep == Running) {
        // Process events to update UI
        QApplication::processEvents();
        
        // Add a delay for animation
        QThread::msleep(animationDelay / 2); // Use half delay for color changes
    }
}

void GraphVisualizer::onVertexHopChanged(int newHops)
{
    // When hop count changes, update the UI and add a delay for animation
    if (animationStep == Running) {
        // Process events to update UI
        QApplication::processEvents();
        
        // Add a delay for animation
        QThread::msleep(animationDelay);
    }
}

// Custom DfsVertex that adds the vertex value to the ordered list
class DfsVisualVertex : public DfsVertex<VisualVertex> {
public:
    QList<int>* orderedList;
    
    DfsVisualVertex(VisualVertex* v, QList<int>* list)
        : DfsVertex<VisualVertex>(v), orderedList(list) {}
        
    void setColor(VertexColor newColor) override {
        DfsVertex<VisualVertex>::setColor(newColor);
        
        // When a vertex is colored black (processed), add to the ordered list
        if (newColor == black && data && orderedList) {
            orderedList->append(data->value);
        }
        
        // Update the visual representation
        if (data) {
            data->setColor(newColor);
        }
    }
};

// Custom compare for DfsVisualVertex
struct DfsVisualVertexCompare {
    bool operator()(const DfsVisualVertex& a, const DfsVisualVertex& b) const {
        if (!a.data || !b.data) return false;
        return a.data->value == b.data->value;
    }
};

void GraphVisualizer::startDfsAnimation()
{
    // Reset any previous DFS state
    resetDfsColors();
    
    // Clear the ordered list
    dfsOrderedList.clear();
    
    // Set up DFS animation state
    dfsAnimationStep = DfsRunning;
    
    // Create a Graph with DfsVisualVertex wrappers
    Graph<DfsVisualVertex, DfsVisualVertexCompare> dfsGraph;
    
    // Create a map to keep track of the wrapper vertices
    QMap<int, DfsVisualVertex*> dfsVertices;
    
    // Create DfsVisualVertex wrappers for each VisualVertex
    for (VisualVertex* v : vertices) {
        DfsVisualVertex* dfsVertex = new DfsVisualVertex(v, &dfsOrderedList);
        dfsVertices[v->value] = dfsVertex;
        dfsGraph.insertVertex(dfsVertex, true);
    }
    
    // Create edges in the DFS graph matching the original graph
    for (VisualVertex* from : vertices) {
        for (VisualVertex* to : vertices) {
            if (graph.isAdjacentGraph(from, to)) {
                dfsGraph.insertEdge(dfsVertices[from->value], dfsVertices[to->value]);
            }
        }
    }
    
    // Create a list to store the ordered vertices
    List<DfsVisualVertex, DfsVisualVertexCompare> ordered;
    
    // Run the DFS algorithm - this will automatically update colors
    // through calls to setColor on each DfsVisualVertex
    int result = dfs(&dfsGraph, ordered);
    
    if (result != 0) {
        // If DFS failed, reset and return
        resetDfsColors();
        return;
    }
    
    // We can verify that ordered List and dfsOrderedList contain the same vertices in the same order
    // by comparing them (for debugging/educational purposes)
    bool listsMatch = true;
    if (dfsOrderedList.size() == ordered.getSize()) {
        ListNode<DfsVisualVertex>* node = ordered.head();
        int i = 0;
        while (node != nullptr) {
            if (node->data() && node->data()->data && 
                node->data()->data->value != dfsOrderedList[i]) {
                listsMatch = false;
                break;
            }
            node = node->next();
            i++;
        }
    } else {
        listsMatch = false;
    }
    
    qDebug() << "DFS Lists match:" << listsMatch;
    
    // Update the label to show the ordered list
    // (Label is created in the constructor and always visible)
    
    // Update the label with the ordered list
    QString orderText = "<b>DFS Order:</b><br>";
    orderText += "<i>(Topological sort - reading from bottom to top)</i><br><br>";
    
    // Display in reverse order for proper topological sort
    // A topological sort means: if there's an edge from A to B, A comes before B
    for (int i = dfsOrderedList.size() - 1; i >= 0; --i) {
        orderText += QString("%1. Value %2<br>").arg(dfsOrderedList.size() - i).arg(dfsOrderedList[i]);
    }
    
    orderText += "<br><i>Note: In a topological sort, for each edge from A to B,<br>"
                 "A must come before B in the ordering.</i>";
                 
    dfsOrderLabel->setText(orderText);
    dfsOrderLabel->adjustSize();
    
    // Signal completion
    dfsAnimationStep = DfsCompleted;
}

void GraphVisualizer::stopDfsAnimation()
{
    // Reset animation state
    dfsAnimationStep = DfsNotRunning;
    
    // Update the label content
    if (dfsOrderLabel) {
        dfsOrderLabel->setText("<b>DFS Order:</b><br>(DFS stopped)");
    }
}

void GraphVisualizer::resetDfsColors()
{
    // Reset all vertices to white color
    for (VisualVertex* v : vertices) {
        if (v) {
            v->setColor(white);
        }
    }
    
    // Clear the ordered list
    dfsOrderedList.clear();
    
    // Update the label content
    if (dfsOrderLabel) {
        dfsOrderLabel->setText("<b>DFS Order:</b><br>(Reset - No DFS run yet)");
    }
    
    // Reset animation state
    stopDfsAnimation();
}

void GraphVisualizer::repositionDfsLabel()
{
    if (dfsOrderLabel && area && area->parentWidget()) {
        QSize parentSize = area->parentWidget()->size();
        dfsOrderLabel->setGeometry(parentSize.width() - 230, 10, 220, 300);
    }
}

void GraphVisualizer::showDfsLabel()
{
    if (dfsOrderLabel) {
        dfsOrderLabel->show();
    }
}

void GraphVisualizer::hideDfsLabel()
{
    if (dfsOrderLabel) {
        dfsOrderLabel->hide();
    }
}
