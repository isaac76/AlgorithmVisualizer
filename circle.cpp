#include "circle.h"

Circle::Circle(QWidget* parent) : Shape(parent)
{
}

Circle::Circle(int value, QWidget* parent) : Shape(parent)
{
    this->value = value;
    
    // Set a fixed size for the circle widget
    setMinimumSize(120, 120);
    setMaximumSize(120, 120);
}

int Circle::getValue()
{
    return this->value;
}

void Circle::setValue(int value)
{
    this->value = value;
    update(); // Trigger repaint when value changes
}

void Circle::draw(QPainter* painter) {
    // Enable anti-aliasing for smoother drawing
    painter->setRenderHint(QPainter::Antialiasing);
    
    // Fill background with transparent color
    painter->fillRect(rect(), Qt::transparent);
    
    // Calculate circle dimensions to fit within the widget
    int diameter = qMin(width(), height()) - 20; // Leave some margin
    QRect circleRect = rect().adjusted(10, 10, -10, -10);
    
    // Draw the circle
    QPen ellipsePen(Qt::black, 2); // Thicker pen for better visibility
    painter->setPen(ellipsePen);
    painter->setBrush(QBrush(Qt::white)); // Fill circle with white
    painter->drawEllipse(circleRect);
    
    // Draw the value text centered in the circle
    painter->setFont(QFont("Arial", 14, QFont::Bold));
    painter->drawText(circleRect, Qt::AlignCenter, QString::number(this->value));
}
