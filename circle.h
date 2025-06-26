#ifndef CIRCLE_H
#define CIRCLE_H

#include "shape.h"
#include <QPainter>
#include <QPoint>
#include <QFont>
#include <QMouseEvent>

class Circle : public Shape
{
    Q_OBJECT

private:
    int value = 0;
    int hopCount = -1; // -1 means not visited yet
    bool dragging;
    QPoint dragOffset;
    QPoint center;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

public:
    explicit Circle(QWidget* parent = nullptr);
    Circle(int value, QWidget* parent = nullptr);

    int getValue();
    void setValue(int value);
    
    int getHopCount();
    void setHopCount(int hops);
    
    // Make setColor accessible for VisualVertex
    void setColor(const QColor& color) {
        Shape::setColor(color);
    }
    
    void draw(QPainter* painter) override;
};

#endif // CIRCLE_H
