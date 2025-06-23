#ifndef LINE_H
#define LINE_H

#include <QPainter>
#include <QPainterPath>
#include <QPointF>
#include "circle.h"
#include "connector.h"

class Line : public Connector
{
private:
    QWidget* startWidget;
    QWidget* endWidget;
    QPointF startPoint;
    QPointF controlPoint;
    QPointF endPoint;
    
public:
    explicit Line(QWidget* parent = nullptr);
    void connectWidgets(QWidget* startWidget, QWidget* endWidget); // Remove 'override'
    void draw(QPainter* painter) override;
    void setControlPoint(const QPointF& pt) { controlPoint = pt; }
};

#endif // LINE_H
