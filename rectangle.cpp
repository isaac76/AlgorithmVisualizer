#include "rectangle.h"

Rectangle::Rectangle(QWidget* parent) : Shape(parent)
{
}

Rectangle::Rectangle(int value, QWidget* parent) : Shape(parent)
{
    this->value = value;

    // Set a fixed size for the rectangle widget
    setMinimumSize(120, 60); // Changed to a more rectangular shape (120×60)
    setMaximumSize(120, 60);
    
    // Important: set fixed margins to zero to avoid spacing
    setContentsMargins(0, 0, 0, 0);
}

int Rectangle::getValue()
{
    return this->value;
}

void Rectangle::setValue(int value)
{
    this->value = value;
    update(); // Trigger repaint when value changes
}

void Rectangle::draw(QPainter* painter) {
    // Enable anti-aliasing for smoother drawing
    painter->setRenderHint(QPainter::Antialiasing);

    // Fill background with transparent color
    painter->fillRect(rect(), Qt::transparent);

    // Use the full widget area for the rectangle, no margins
    QRect rectangle = rect(); // No adjustments to use full size

    QPen rectPen(Qt::black, 2); // Thicker pen for better visibility
    painter->setPen(rectPen);
    painter->setBrush(this->Shape::getColor());
    painter->drawRect(rectangle);

    painter->setFont(QFont("Arial", 14, QFont::Bold));
    painter->drawText(rectangle, Qt::AlignCenter, QString::number(this->value));
}
