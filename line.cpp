#include "line.h"

Line::Line(QWidget* parent) : Connector(parent)
{
}

void Line::draw(QPainter* painter) {
    painter->setRenderHint(QPainter::Antialiasing);
    
    // Draw the curved path
    QPainterPath path;
    path.moveTo(this->startPoint);
    path.quadTo(this->controlPoint, this->endPoint);
    painter->setPen(QPen(Qt::black, 2));
    painter->drawPath(path);
    
    // Draw the arrowhead to show direction
    // Calculate the direction vector at the end point (tangent to the curve)
    QPointF direction = this->endPoint - this->controlPoint;
    
    // Normalize the direction vector manually
    double length = std::hypot(direction.x(), direction.y());
    if (length > 0) {
        direction.rx() /= length;
        direction.ry() /= length;
        
        // Arrow size - adjust as needed
        double arrowSize = 10.0;
        
        // Calculate arrow points
        QPointF arrowP1 = this->endPoint - direction * arrowSize + QPointF(-direction.y(), direction.x()) * 0.5 * arrowSize;
        QPointF arrowP2 = this->endPoint - direction * arrowSize - QPointF(-direction.y(), direction.x()) * 0.5 * arrowSize;
        
        // Draw arrowhead
        QPolygonF arrowHead;
        arrowHead << this->endPoint << arrowP1 << arrowP2;
        painter->setBrush(QBrush(Qt::black));
        painter->drawPolygon(arrowHead);
    }
}

void Line::connectWidgets(QWidget* startWidget, QWidget* endWidget) {
    this->startWidget = startWidget;
    this->endWidget = endWidget;

    // Get absolute positions within parent
    QPointF startPos = startWidget->pos();
    QPointF endPos = endWidget->pos();
    
    // Calculate centers by adding half the widget size
    QPointF centerA = startPos + QPointF(startWidget->width()/2.0, startWidget->height()/2.0);
    QPointF centerB = endPos + QPointF(endWidget->width()/2.0, endWidget->height()/2.0);

    QPointF dir = centerB - centerA;
    double len = std::hypot(dir.x(), dir.y());
    
    // Handle case when widgets are very close or on top of each other
    if (len < 1.0) {
        // Use default points to avoid division by zero
        this->startPoint = centerA;
        this->endPoint = centerB;
        this->controlPoint = (centerA + centerB) / 2.0;
        return;
    }
    
    QPointF unit = dir / len;

    // Calculate actual radii based on widget types
    double startWidgetRadius, endWidgetRadius;
    
    // If the widget is a Circle, get its actual radius
    if (auto* circle = qobject_cast<Circle*>(startWidget)) {
        startWidgetRadius = circle->width() / 2.0;
    } else {
        // For non-circular widgets, estimate radius as the average of width and height
        startWidgetRadius = (startWidget->width() + startWidget->height()) / 4.0;
    }
    
    if (auto* circle = qobject_cast<Circle*>(endWidget)) {
        endWidgetRadius = circle->width() / 2.0;
    } else {
        endWidgetRadius = (endWidget->width() + endWidget->height()) / 4.0;
    }

    // Calculate intersection points with the edges
    this->startPoint = centerA + unit * startWidgetRadius;
    this->endPoint = centerB - unit * endWidgetRadius;

    // Calculate control point for the curve
    QPointF mid = (this->startPoint + this->endPoint) / 2.0;
    QPointF perp(-dir.y() / len, dir.x() / len);
    
    // Make the curve height proportional to the distance between points
    double bow = qMin(40.0, len * 0.3); // Adjust curve based on distance
    this->controlPoint = mid + perp * bow;
    
    update();
}
