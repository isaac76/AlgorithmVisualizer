#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QWidget>
#include <QPainter>

class Connector : public QWidget
{
    Q_OBJECT
public:
    explicit Connector(QWidget *parent = nullptr);
    virtual ~Connector();

    // Virtual method for drawing the shape
    virtual void draw(QPainter *painter) = 0;

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // CONNECTOR_H
