#ifndef SHAPE_H
#define SHAPE_H

#include <QWidget>
#include <QPainter>

class Shape : public QWidget
{
    Q_OBJECT

public:
    explicit Shape(QWidget *parent = nullptr);
    virtual ~Shape();
    
    // Virtual method for drawing the shape
    virtual void draw(QPainter *painter) = 0;
    
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // SHAPE_H
