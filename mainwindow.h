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
#include <QStatusBar>

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

protected:
    // Override resizeEvent to handle window resizing
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void addVertex();
    void addEdge();
    void removeEdge();
    void startBfs();
    void clearBfs();
    void startDfs();  // New slot for starting DFS algorithm
    void clearDfs();  // New slot for clearing DFS visualization
    void addRectangle();
    void onVisualizationSelected(int index);
    void updateStartVertexCombo();
    void updateEdgeComboBoxes();
    void animationSpeedChanged(int value);

private:
    void setupGraphVisualization();
    void setupGraphVisualization2();
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
    QPushButton* bfsButton = nullptr;
    QPushButton* bfsClearButton = nullptr;
    QPushButton* dfsButton = nullptr;         // New button for starting DFS
    QPushButton* dfsClearButton = nullptr;    // New button for clearing DFS
    QSlider* animationSpeedSlider = nullptr;
    QComboBox* edgeFromCombo = nullptr;
    QComboBox* edgeToCombo = nullptr;
    QComboBox* startVertexCombo = nullptr;
    QList<Rectangle*> rectangles;
    GraphVisualizer* graphVisualizer = nullptr;
};
#endif // MAINWINDOW_H
