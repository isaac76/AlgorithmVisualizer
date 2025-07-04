#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QResizeEvent>

/***************************************************************
 * Filename:   mainwindow.cpp
 * Author:     IO
 * Description:
 *   The MainWindow class presents a user interface for choosing 
 *   an algorithm and then adding data and visualing how the 
 *   algorith inserts data and traverses the data structure.
 ****************************************************************/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // setup main window and menu bar
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    QWidget* menuBarWidget = new QWidget(this);
    QHBoxLayout* menuBarLayout = new QHBoxLayout(menuBarWidget);
    menuBarLayout->setContentsMargins(10, 0, 10, 0);
    
    QLabel* selectorLabel = new QLabel("Select Visualization:", this);
    visualizationSelector = new QComboBox(this);
    visualizationSelector->setObjectName("visualizationSelector");
    
    visualizationSelector->addItem("Select...");
    visualizationSelector->addItem("BFS");
    visualizationSelector->addItem("DFS");
    visualizationSelector->addItem("Queue");
    
    // handle visualization selection
    connect(visualizationSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onVisualizationSelected);
    
    menuBarLayout->addWidget(selectorLabel);
    menuBarLayout->addWidget(visualizationSelector);
    menuBarLayout->addStretch();
    
    ui->menubar->setCornerWidget(menuBarWidget);
    
    visualizationArea = new QWidget();
    visualizationArea->setMinimumHeight(400);
    visualizationArea->setStyleSheet("background-color: white;");
    
    visualizationArea->setLayout(new QVBoxLayout());
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(visualizationArea);
    
    addVertexButton = nullptr;
    
    mainLayout->addWidget(scrollArea);
    
    mainLayout->setContentsMargins(5, 5, 5, 5);
    
    setCentralWidget(centralWidget);
}

/**
 * @brief   adds a random number to the graph visualization
 */
void MainWindow::addVertex()
{
    if (graphVisualizer) {
        int value = QRandomGenerator::global()->bounded(1, 101);
        graphVisualizer->addVertex(value);
        
        // Update all vertex combo boxes
        updateStartVertexCombo();
        updateEdgeComboBoxes();
    }
}

/**
 * @brief   connects two vertices
 */
void MainWindow::addEdge()
{
    if (!graphVisualizer || edgeFromCombo->count() == 0 || edgeToCombo->count() == 0) return;
    
    bool ok1 = false, ok2 = false;
    int fromVal = edgeFromCombo->currentText().toInt(&ok1);
    int toVal = edgeToCombo->currentText().toInt(&ok2);
    if (!ok1 || !ok2) return;

    auto verts = graphVisualizer->getVertices();
    VisualVertex* from = nullptr;
    VisualVertex* to = nullptr;
    for (VisualVertex* v : verts) {
        if (v->value == fromVal) from = v;
        if (v->value == toVal) to = v;
    }
    if (from && to) {
        graphVisualizer->addEdge(from, to);
    }
}

void MainWindow::addRectangle()
{
    int value = QRandomGenerator::global()->bounded(1, 101);
    Rectangle* rect = new Rectangle(value, visualizationArea);
    
    rectangles.append(rect);
    
    updateRectanglePositions();
}

/**
 * @brief sets up the graph visualization when the user chooses graph from the 
 *        drop down menu
 */
void MainWindow::setupGraphVisualization()
{
    clearVisualization();

    graphVisualizer = new GraphVisualizer(visualizationArea, this);
    graphVisualizer->hideDfsLabel();  // Hide DFS label for BFS visualization

    addVertexButton = new QPushButton("Add Node", this);
    addEdgeButton = new QPushButton("Add Edge", this);
    removeEdgeButton = new QPushButton("Remove Edge", this);
    bfsButton = new QPushButton("BFS", this); // New BFS button
    bfsClearButton = new QPushButton("Clear BFS", this); // New Clear BFS button
    
    // Set object names for testing
    addVertexButton->setObjectName("addVertexButton");
    addEdgeButton->setObjectName("addEdgeButton");
    removeEdgeButton->setObjectName("removeEdgeButton");
    bfsButton->setObjectName("bfsButton");
    bfsClearButton->setObjectName("bfsClearButton");
    
    // Replace line edits with combo boxes for vertex selection
    edgeFromCombo = new QComboBox(this);
    edgeToCombo = new QComboBox(this);
    edgeFromCombo->setFixedWidth(70);
    edgeToCombo->setFixedWidth(70);
    
    // Set object names for testing
    edgeFromCombo->setObjectName("edgeFromCombo");
    edgeToCombo->setObjectName("edgeToCombo");
    
    // Labels for the combo boxes
    QLabel* fromLabel = new QLabel("From:", this);
    QLabel* toLabel = new QLabel("To:", this);
    
    startVertexCombo = new QComboBox(this);
    startVertexCombo->setFixedWidth(70);
    startVertexCombo->setObjectName("startVertexCombo");
    
    // Initialize all combo boxes with vertex values
    updateStartVertexCombo();
    updateEdgeComboBoxes();
    
    animationSpeedSlider = new QSlider(Qt::Horizontal, this);
    animationSpeedSlider->setRange(100, 2000); // Will be converted to 500-50ms delay
    animationSpeedSlider->setValue(1000); // Default: ~300ms delay
    animationSpeedSlider->setFixedWidth(100);
    QLabel* speedLabel = new QLabel("Speed:", this);

    QWidget* controlsWidget = new QWidget(this);
    QHBoxLayout* controlsLayout = new QHBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->addWidget(addVertexButton);
    controlsLayout->addWidget(fromLabel);
    controlsLayout->addWidget(edgeFromCombo);
    controlsLayout->addWidget(toLabel);
    controlsLayout->addWidget(edgeToCombo);
    controlsLayout->addWidget(addEdgeButton);
    controlsLayout->addWidget(removeEdgeButton);
    controlsLayout->addWidget(bfsButton);
    controlsLayout->addWidget(bfsClearButton);
    controlsLayout->addWidget(startVertexCombo);
    controlsLayout->addWidget(new QLabel("Speed:", this));
    controlsLayout->addWidget(animationSpeedSlider);

    connect(addVertexButton, &QPushButton::clicked, this, &MainWindow::addVertex);
    connect(addEdgeButton, &QPushButton::clicked, this, &MainWindow::addEdge);
    connect(removeEdgeButton, &QPushButton::clicked, this, &MainWindow::removeEdge);
    connect(bfsButton, &QPushButton::clicked, this, &MainWindow::startBfs);
    connect(bfsClearButton, &QPushButton::clicked, this, &MainWindow::clearBfs);
    connect(animationSpeedSlider, &QSlider::valueChanged, this, &MainWindow::animationSpeedChanged);
    
    ui->statusbar->addWidget(controlsWidget);
}

void MainWindow::setupGraphVisualization2()
{
    clearVisualization();

    graphVisualizer = new GraphVisualizer(visualizationArea, this);
    graphVisualizer->showDfsLabel();  // Show DFS label for DFS visualization

    addVertexButton = new QPushButton("Add Node", this);
    addEdgeButton = new QPushButton("Add Edge", this);
    removeEdgeButton = new QPushButton("Remove Edge", this);
    dfsButton = new QPushButton("DFS", this);
    dfsClearButton = new QPushButton("Clear DFS", this);
    
    // Set object names for testing
    addVertexButton->setObjectName("addVertexButton");
    addEdgeButton->setObjectName("addEdgeButton");
    removeEdgeButton->setObjectName("removeEdgeButton");
    dfsButton->setObjectName("dfsButton");
    dfsClearButton->setObjectName("dfsClearButton");
    
    // Replace line edits with combo boxes for vertex selection
    edgeFromCombo = new QComboBox(this);
    edgeToCombo = new QComboBox(this);
    edgeFromCombo->setFixedWidth(70);
    edgeToCombo->setFixedWidth(70);
    
    // Set object names for testing
    edgeFromCombo->setObjectName("edgeFromCombo");
    edgeToCombo->setObjectName("edgeToCombo");
    
    // Labels for the combo boxes
    QLabel* fromLabel = new QLabel("From:", this);
    QLabel* toLabel = new QLabel("To:", this);
    
    // For DFS, we don't need a start vertex combo since DFS processes all vertices
    
    // Initialize edge combo boxes with vertex values
    updateEdgeComboBoxes();
    
    animationSpeedSlider = new QSlider(Qt::Horizontal, this);
    animationSpeedSlider->setRange(100, 2000); // Will be converted to 500-50ms delay
    animationSpeedSlider->setValue(1000); // Default: ~300ms delay
    animationSpeedSlider->setFixedWidth(100);

    QWidget* controlsWidget = new QWidget(this);
    QHBoxLayout* controlsLayout = new QHBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->addWidget(addVertexButton);
    controlsLayout->addWidget(fromLabel);
    controlsLayout->addWidget(edgeFromCombo);
    controlsLayout->addWidget(toLabel);
    controlsLayout->addWidget(edgeToCombo);
    controlsLayout->addWidget(addEdgeButton);
    controlsLayout->addWidget(removeEdgeButton);
    controlsLayout->addWidget(dfsButton);
    controlsLayout->addWidget(dfsClearButton);
    controlsLayout->addWidget(new QLabel("Speed:", this));
    controlsLayout->addWidget(animationSpeedSlider);

    connect(addVertexButton, &QPushButton::clicked, this, &MainWindow::addVertex);
    connect(addEdgeButton, &QPushButton::clicked, this, &MainWindow::addEdge);
    connect(removeEdgeButton, &QPushButton::clicked, this, &MainWindow::removeEdge);
    connect(dfsButton, &QPushButton::clicked, this, &MainWindow::startDfs);
    connect(dfsClearButton, &QPushButton::clicked, this, &MainWindow::clearDfs);
    connect(animationSpeedSlider, &QSlider::valueChanged, this, &MainWindow::animationSpeedChanged);
    
    ui->statusbar->addWidget(controlsWidget);
}

/**
 * @brief removes a connection between two vertices
 */
void MainWindow::removeEdge()
{
    if (!graphVisualizer || edgeFromCombo->count() == 0 || edgeToCombo->count() == 0) return;
    
    bool ok1 = false, ok2 = false;
    int fromVal = edgeFromCombo->currentText().toInt(&ok1);
    int toVal = edgeToCombo->currentText().toInt(&ok2);
    if (!ok1 || !ok2) return;

    auto verts = graphVisualizer->getVertices();
    VisualVertex* from = nullptr;
    VisualVertex* to = nullptr;
    for (VisualVertex* v : verts) {
        if (v->value == fromVal) from = v;
        if (v->value == toVal) to = v;
    }
    if (from && to) {
        graphVisualizer->removeEdge(from, to);
    }
}

void MainWindow::setupQueueVisualization()
{
    clearVisualization();

    for (int i=0;i<5;i++ ) {
        addRectangle();
    }
}

/**
 * @brief handles user ineraction with the visualization drop down
 */
void MainWindow::onVisualizationSelected(int index)
{
    // Clear previous visualization first
    clearVisualization();
    
    switch (index) {
        case 0:
            break;
        case 1:
            setupGraphVisualization();
            break;
        case 2:
            setupGraphVisualization2();
            break;
        case 3:
            setupQueueVisualization();
            break;
    }
}

void MainWindow::clearVisualization()
{
    if (graphVisualizer) {
        delete graphVisualizer;
        graphVisualizer = nullptr;
    }

    for (Rectangle* rectangle : rectangles) {
        delete rectangle;
    }
    rectangles.clear();

    if (addVertexButton) {
        QWidget* controlsWidget = addVertexButton->parentWidget();
        if (controlsWidget) {
            ui->statusbar->removeWidget(controlsWidget);
            delete controlsWidget; // deletes all children (buttons/edits)
        }
        addVertexButton = nullptr;
        addEdgeButton = nullptr;
        removeEdgeButton = nullptr;
        bfsButton = nullptr;
        bfsClearButton = nullptr;
        dfsButton = nullptr;
        dfsClearButton = nullptr;
        startVertexCombo = nullptr;
        edgeFromCombo = nullptr;
        edgeToCombo = nullptr;
    }
}

void MainWindow::updateRectanglePositions()
{
    int yPos = 30;
    
    for (Rectangle* rect : rectangles) {
        int xPos = (visualizationArea->width() - rect->width()) / 2;
        
        xPos = qMax(xPos, 20);
        
        rect->setGeometry(xPos, yPos, rect->width(), rect->height());
        
        rect->show();
        
        yPos += rect->height();
    }
}

void MainWindow::updateStartVertexCombo()
{
    if (!graphVisualizer || !startVertexCombo) return;
    
    int currentIndex = startVertexCombo->currentIndex();
    int currentValue = (currentIndex >= 0) ? startVertexCombo->currentText().toInt() : -1;
    
    startVertexCombo->clear();
    
    auto vertices = graphVisualizer->getVertices();
    for (VisualVertex* vertex : vertices) {
        startVertexCombo->addItem(QString::number(vertex->value));
    }
    
    if (currentValue >= 0) {
        int index = startVertexCombo->findText(QString::number(currentValue));
        if (index >= 0) {
            startVertexCombo->setCurrentIndex(index);
        }
    }
}

void MainWindow::updateEdgeComboBoxes()
{
    if (!graphVisualizer || !edgeFromCombo || !edgeToCombo) return;
    
    // Remember selected values if possible
    int currentFromValue = -1;
    int currentToValue = -1;
    
    if (edgeFromCombo->currentIndex() >= 0) {
        bool ok = false;
        currentFromValue = edgeFromCombo->currentText().toInt(&ok);
        if (!ok) currentFromValue = -1;
    }
    
    if (edgeToCombo->currentIndex() >= 0) {
        bool ok = false;
        currentToValue = edgeToCombo->currentText().toInt(&ok);
        if (!ok) currentToValue = -1;
    }
    
    // Clear and refill the combo boxes
    edgeFromCombo->clear();
    edgeToCombo->clear();
    
    auto vertices = graphVisualizer->getVertices();
    for (VisualVertex* vertex : vertices) {
        edgeFromCombo->addItem(QString::number(vertex->value));
        edgeToCombo->addItem(QString::number(vertex->value));
    }
    
    // Restore selected values if they still exist
    if (currentFromValue >= 0) {
        int index = edgeFromCombo->findText(QString::number(currentFromValue));
        if (index >= 0) {
            edgeFromCombo->setCurrentIndex(index);
        }
    }
    
    if (currentToValue >= 0) {
        int index = edgeToCombo->findText(QString::number(currentToValue));
        if (index >= 0) {
            edgeToCombo->setCurrentIndex(index);
        }
    }
}

void MainWindow::startBfs()
{
    if (!graphVisualizer || startVertexCombo->count() == 0) {
        return;
    }
    
    bool ok = false;
    int startValue = startVertexCombo->currentText().toInt(&ok);
    if (!ok) {
        return;
    }
    
    graphVisualizer->startBfsAnimation(startValue);
}

void MainWindow::clearBfs()
{
    if (graphVisualizer) {
        graphVisualizer->resetBfsColors();
    }
}

void MainWindow::animationSpeedChanged(int value)
{
    if (graphVisualizer) {
        // Convert slider value (100-2000) to a delay (500-50ms)
        // Higher slider values = faster animation = lower delay
        int delay = 550 - (value / 4);
        graphVisualizer->setAnimationDelay(delay);
    }
}

void MainWindow::startDfs()
{
    if (!graphVisualizer) {
        return;
    }
    
    // Start the DFS animation
    // DFS visits all vertices regardless of starting point
    graphVisualizer->startDfsAnimation();
}

void MainWindow::clearDfs()
{
    if (graphVisualizer) {
        // Reset the DFS visualization
        graphVisualizer->resetDfsColors();
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    
    // Reposition the DFS label whenever the window is resized
    if (graphVisualizer) {
        graphVisualizer->repositionDfsLabel();
    }
}

MainWindow::~MainWindow()
{
    clearVisualization();
    delete ui;
}
