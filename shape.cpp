#include "shape.h"
#include <QPainter>
#include <QPaintEvent>

Shape::Shape(QWidget *parent) : QWidget(parent)
{
    // Default initialization
}

Shape::~Shape()
{
    // Cleanup
}

void Shape::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    draw(&painter);
}

void Shape::setColor(const QColor& color) {
    this->color = color;
    update();
}

QColor Shape::getColor() const {
    return this->color;
}