#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QRandomGenerator>
#include <QLabel>
#include <QFrame>
#include <QLineEdit>

#include "circle.h"
#include "rectangle.h"
#include "line.h"
#include "graphvisualizer.h"

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
    void addVertex();
    void addEdge();
    void addRectangle();
    void onVisualizationSelected(int index);

private:
    void setupGraphVisualization();
    void setupQueueVisualization();
    void clearVisualization();
    void updateRectanglePositions();
    
    Ui::MainWindow *ui;
    QComboBox* visualizationSelector;
    QWidget* visualizationArea;
    QPushButton* addCircleButton;
    QPushButton* addVertexButton;
    QPushButton* addEdgeButton;
    QLineEdit* edgeFromEdit;
    QLineEdit* edgeToEdit;
    QList<Rectangle*> rectangles;
    GraphVisualizer* graphVisualizer = nullptr;
};
#endif // MAINWINDOW_H
