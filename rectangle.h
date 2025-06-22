#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "shape.h"

class Rectangle : public Shape
{
private:
    int value = 0;
public:
    explicit Rectangle(QWidget* parent = nullptr);
    Rectangle(int value, QWidget* parent = nullptr);

    int getValue();
    void setValue(int value);

    void draw(QPainter* painter) override;
};

#endif // RECTANGLE_H
