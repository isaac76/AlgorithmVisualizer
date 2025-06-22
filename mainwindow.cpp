#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "circle.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QRandomGenerator>
#include <QFrame>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // Create a central widget with a layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Create a visualization area with coordinate system
    visualizationArea = new QWidget();
    visualizationArea->setMinimumHeight(400);
    visualizationArea->setStyleSheet("background-color: white;");
    
    // Elements will be positioned absolutely within this area
    visualizationArea->setLayout(new QVBoxLayout());
    
    // Create a scroll area for visualization
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(visualizationArea);
    
    // Create control panel for adding new elements
    QHBoxLayout* controlPanel = new QHBoxLayout();
    
    // Add a button to add new circles
    addCircleButton = new QPushButton("Add Node", this);
    connect(addCircleButton, &QPushButton::clicked, this, &MainWindow::addCircle);
    
    // Add controls to the control panel
    controlPanel->addWidget(addCircleButton);
    controlPanel->addStretch(); // Add spacer to push controls to the left
    
    // Add the scroll area and controls to the main layout
    mainLayout->addWidget(scrollArea);
    mainLayout->addLayout(controlPanel);
    
    // Set the central widget
    setCentralWidget(centralWidget);
    
    // Add initial example nodes
    for (int i = 0; i < 5; i++) {
        addCircle();
    }
    
    // Set a reasonable window size
    resize(800, 600);
}

void MainWindow::addCircle()
{
    // Create a circle with a random value between 1 and 100
    int value = QRandomGenerator::global()->bounded(1, 101);
    Circle* circle = new Circle(value, visualizationArea);
    
    // Add to our vector of circles
    circles.append(circle);
    
    // Set the circle's position using absolute coordinates
    // For demo purposes, position them randomly in the visualization area
    int maxX = visualizationArea->width() - circle->width();
    int maxY = visualizationArea->height() - circle->height();
    
    // Ensure we don't position outside the visible area
    maxX = qMax(maxX, 50);
    maxY = qMax(maxY, 50);
    
    int x = QRandomGenerator::global()->bounded(50, maxX);
    int y = QRandomGenerator::global()->bounded(50, maxY);
    
    // Move to absolute position
    circle->setGeometry(x, y, circle->width(), circle->height());
    
    // Make the circle draggable (to be implemented)
    // For now, just show it
    circle->show();
}

MainWindow::~MainWindow()
{
    // No need to delete circles as they are child widgets and will be deleted automatically
    delete ui;
}

