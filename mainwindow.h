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

private slots:
    void addVertex();
    void addEdge();
    void removeEdge();
    void startBfs(); // New slot for BFS button
    void clearBfs(); // New slot for Clear BFS button
    void updateBfsStatus(const QString& message); // New slot for BFS status updates
    void addRectangle();
    void onVisualizationSelected(int index);
    void updateStartVertexCombo(); // New slot for updating the start vertex dropdown
    void animationSpeedChanged(int value); // New slot for animation speed slider

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
    QPushButton* bfsClearButton = nullptr; // New Clear BFS button
    QSlider* animationSpeedSlider = nullptr; // Slider to control BFS animation speed
    QLineEdit* edgeFromEdit = nullptr;
    QLineEdit* edgeToEdit = nullptr;
    QComboBox* startVertexCombo = nullptr; // New dropdown for selecting start vertex
    QLabel* statusLabel = nullptr; // New label for BFS status messages
    QList<Rectangle*> rectangles;
    GraphVisualizer* graphVisualizer = nullptr;
};
#endif // MAINWINDOW_H
