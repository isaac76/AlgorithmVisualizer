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
    void removeEdge();
    void addRectangle();
    void onVisualizationSelected(int index);
    void updateStartVertexCombo(); // New slot for updating the start vertex dropdown

private:
    void setupGraphVisualization();
    void setupQueueVisualization();
    void clearVisualization();
    void updateRectanglePositions();
    
    Ui::MainWindow *ui;
    QComboBox* visualizationSelector;
    QWidget* visualizationArea;
    QPushButton* addCircleButton;
    QPushButton* addVertexButton = nullptr;
    QPushButton* addEdgeButton = nullptr;
    QPushButton* removeEdgeButton = nullptr;
    QPushButton* bfsButton = nullptr; // New BFS button
    QLineEdit* edgeFromEdit = nullptr;
    QLineEdit* edgeToEdit = nullptr;
    QComboBox* startVertexCombo = nullptr; // New dropdown for selecting start vertex
    QList<Rectangle*> rectangles;
    GraphVisualizer* graphVisualizer = nullptr;
};
#endif // MAINWINDOW_H
