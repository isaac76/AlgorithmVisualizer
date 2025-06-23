#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // Create a central widget with a layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Create visualization selector in the menu bar
    QWidget* menuBarWidget = new QWidget(this);
    QHBoxLayout* menuBarLayout = new QHBoxLayout(menuBarWidget);
    menuBarLayout->setContentsMargins(10, 0, 10, 0);
    
    QLabel* selectorLabel = new QLabel("Select Visualization:", this);
    visualizationSelector = new QComboBox(this);
    
    // Add visualization options
    visualizationSelector->addItem("Select...");
    visualizationSelector->addItem("Graph");
    visualizationSelector->addItem("Queue");
    
    // Connect the selector to the slot
    connect(visualizationSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onVisualizationSelected);
    
    // Add the selector to the menu bar widget
    menuBarLayout->addWidget(selectorLabel);
    menuBarLayout->addWidget(visualizationSelector);
    menuBarLayout->addStretch();
    
    // Add the widget to the menu bar
    ui->menubar->setCornerWidget(menuBarWidget);
    
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
    
    // Initialize addCircleButton as nullptr (will be created in setupGraphVisualization)
    addCircleButton = nullptr;
    
    // Add the scroll area to the main layout
    mainLayout->addWidget(scrollArea);
    
    // Set small margins for the central widget
    mainLayout->setContentsMargins(5, 5, 5, 5);
    
    // Set the central widget
    setCentralWidget(centralWidget);
    
    // Set a reasonable window size
    resize(800, 600);
}

void MainWindow::addCircle()
{
    // Create a circle with a random value between 1 and 100
    int value = QRandomGenerator::global()->bounded(1, 101);
    Circle* circle = new Circle(value, visualizationArea);
    
    // Set initial position (will be adjusted to avoid overlap)
    int maxX = visualizationArea->width() - circle->width();
    int maxY = visualizationArea->height() - circle->height();
    
    // Ensure we don't position outside the visible area
    maxX = qMax(maxX, 50);
    maxY = qMax(maxY, 50);
    
    // Try up to 20 positions to find one without overlap
    bool foundValidPosition = false;
    for (int attempt = 0; attempt < 20; attempt++) {
        int x = QRandomGenerator::global()->bounded(50, maxX);
        int y = QRandomGenerator::global()->bounded(50, maxY);
        
        // Check if this position overlaps with existing circles
        bool hasOverlap = false;
        for (Circle* existingCircle : circles) {
            int dx = existingCircle->x() - x;
            int dy = existingCircle->y() - y;
            int minDistance = existingCircle->width() + circle->width();
            
            if (dx*dx + dy*dy < minDistance*minDistance/4) {
                hasOverlap = true;
                break;
            }
        }
        
        if (!hasOverlap) {
            circle->setGeometry(x, y, circle->width(), circle->height());
            foundValidPosition = true;
            break;
        }
    }
    
    // If no valid position found, just place it somewhere
    if (!foundValidPosition) {
        int x = QRandomGenerator::global()->bounded(50, maxX);
        int y = QRandomGenerator::global()->bounded(50, maxY);
        circle->setGeometry(x, y, circle->width(), circle->height());
    }
    
    // Show the circle
    circle->show();
    
    // Add to our vector of circles
    circles.append(circle);
    
    // If we have at least 2 circles, connect the last two with a line
    if (circles.size() >= 2) {
        Circle* lastCircle = circles.at(circles.size() - 1);
        Circle* prevCircle = circles.at(circles.size() - 2);
        
        Line* line = new Line(visualizationArea);
        line->connectWidgets(prevCircle, lastCircle);
        
        // Make sure the line is drawn and visible
        line->setGeometry(0, 0, visualizationArea->width(), visualizationArea->height());
        line->raise(); // Bring the line to the front
        line->show();
        
        // Store the line for cleanup
        lines.append(line);
    }
}

void MainWindow::addRectangle()
{
    // Create a rectangle with a random value between 1 and 100
    int value = QRandomGenerator::global()->bounded(1, 101);
    Rectangle* rect = new Rectangle(value, visualizationArea);
    
    // Add to our vector of rectangles
    rectangles.append(rect);
    
    // Position all rectangles in a stack
    updateRectanglePositions();
}

void MainWindow::setupGraphVisualization()
{
    // Clear any previous visualization
    clearVisualization();
    
    // Create a button to add new circles (nodes)
    addCircleButton = new QPushButton("Add Node", this);
    connect(addCircleButton, &QPushButton::clicked, this, &MainWindow::addCircle);
    
    // Add the button to the status bar
    ui->statusbar->addWidget(addCircleButton);
    
    // No more default circles - user will add them with the button
}

void MainWindow::setupQueueVisualization()
{
    clearVisualization();

    for (int i=0;i<5;i++ ) {
        addRectangle();
    }
}

void MainWindow::onVisualizationSelected(int index)
{
    // Clear previous visualization first
    clearVisualization();
    
    switch (index) {
        case 0: // "Select..."
            // Just keep the area clear
            break;
        case 1: // "Graph"
            setupGraphVisualization();
            break;
        case 2: // "Queue"
            setupQueueVisualization();
            break;
    }
}

void MainWindow::clearVisualization()
{
    // Clear all circles
    for (Circle* circle : circles) {
        delete circle;
    }
    circles.clear();
    
    // Clear all rectangles
    for (Rectangle* rectangle : rectangles) {
        delete rectangle;
    }
    rectangles.clear();
    
    // Clear all lines
    for (Line* line : lines) {
        delete line;
    }
    lines.clear();
    
    // Remove add button from status bar if it exists
    if (addCircleButton) {
        ui->statusbar->removeWidget(addCircleButton);
        delete addCircleButton;
        addCircleButton = nullptr;
    }
}

void MainWindow::updateRectanglePositions()
{
    // Start position for the first rectangle (near the top with some margin)
    int yPos = 30;
    
    // Center all rectangles horizontally in the visualization area
    for (Rectangle* rect : rectangles) {
        // Calculate centered position
        int xPos = (visualizationArea->width() - rect->width()) / 2;
        
        // Ensure position is valid
        xPos = qMax(xPos, 20);
        
        // Set the rectangle's position
        rect->setGeometry(xPos, yPos, rect->width(), rect->height());
        
        // Make sure the rectangle is visible
        rect->show();
        
        // Increment y position for the next rectangle without spacing
        yPos += rect->height();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

