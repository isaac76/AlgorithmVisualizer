#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPushButton>
#include "circle.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addCircle();

private:
    Ui::MainWindow *ui;
    QVector<Circle*> circles;
    QPushButton* addCircleButton;
    QWidget* visualizationArea; // Area for algorithm visualization
};
#endif // MAINWINDOW_H
