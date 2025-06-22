#ifndef CIRCLE_H
#define CIRCLE_H

#include "shape.h"
#include <QPainter>

class Circle : public Shape
{
    Q_OBJECT

private:
    int value = 0;

public:
    explicit Circle(QWidget* parent = nullptr);
    Circle(int value, QWidget* parent = nullptr);

    int getValue();
    void setValue(int value);
    
    void draw(QPainter* painter) override;
};

#endif // CIRCLE_H
