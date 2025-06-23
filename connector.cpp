#include "connector.h"


Connector::Connector(QWidget *parent) : QWidget(parent)
{
    // Default initialization
}

Connector::~Connector()
{
    // Cleanup
}

void Connector::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    draw(&painter);
}
