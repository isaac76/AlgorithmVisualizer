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
    
    // Initialize addVertexButton as nullptr (will be created in setupGraphVisualization)
    addVertexButton = nullptr;
    
    // Add the scroll area to the main layout
    mainLayout->addWidget(scrollArea);
    
    // Set small margins for the central widget
    mainLayout->setContentsMargins(5, 5, 5, 5);
    
    // Set the central widget
    setCentralWidget(centralWidget);
    
    // Set a reasonable window size
    resize(800, 600);
}

void MainWindow::addVertex()
{
    if (graphVisualizer) {
        int value = QRandomGenerator::global()->bounded(1, 101);
        graphVisualizer->addVertex(value);
        
        // Update the start vertex dropdown
        updateStartVertexCombo();
    }
}

void MainWindow::addEdge()
{
    if (!graphVisualizer) return;
    bool ok1 = false, ok2 = false;
    int fromVal = edgeFromEdit->text().toInt(&ok1);
    int toVal = edgeToEdit->text().toInt(&ok2);
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
    clearVisualization();

    // Instantiate graphVisualizer
    graphVisualizer = new GraphVisualizer(visualizationArea, this);

    // Create widgets for adding/removing vertices and edges
    addVertexButton = new QPushButton("Add Node", this);
    addEdgeButton = new QPushButton("Add Edge", this);
    removeEdgeButton = new QPushButton("Remove Edge", this);
    bfsButton = new QPushButton("BFS", this); // New BFS button
    bfsClearButton = new QPushButton("Clear BFS", this); // New Clear BFS button
    edgeFromEdit = new QLineEdit(this);
    edgeToEdit = new QLineEdit(this);
    edgeFromEdit->setPlaceholderText("From");
    edgeToEdit->setPlaceholderText("To");
    edgeFromEdit->setFixedWidth(50);
    edgeToEdit->setFixedWidth(50);
    
    // Create start vertex dropdown
    startVertexCombo = new QComboBox(this);
    startVertexCombo->setFixedWidth(70);
    updateStartVertexCombo(); // Initialize the dropdown
    
    // Create animation speed slider
    animationSpeedSlider = new QSlider(Qt::Horizontal, this);
    animationSpeedSlider->setRange(100, 2000); // 100ms to 2000ms
    animationSpeedSlider->setValue(1000); // Default: 1000ms
    animationSpeedSlider->setFixedWidth(100);
    QLabel* speedLabel = new QLabel("Speed:", this);

    // Layout for the controls
    QWidget* controlsWidget = new QWidget(this);
    QHBoxLayout* controlsLayout = new QHBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->addWidget(addVertexButton);
    controlsLayout->addWidget(edgeFromEdit);
    controlsLayout->addWidget(edgeToEdit);
    controlsLayout->addWidget(addEdgeButton);
    controlsLayout->addWidget(removeEdgeButton);
    controlsLayout->addWidget(bfsButton); // Add BFS button
    controlsLayout->addWidget(bfsClearButton); // Add Clear BFS button
    controlsLayout->addWidget(startVertexCombo); // Add start vertex dropdown
    controlsLayout->addWidget(new QLabel("Speed:", this));
    controlsLayout->addWidget(animationSpeedSlider); // Add speed slider

    // Create a status label for BFS messages
    statusLabel = new QLabel("", this);
    controlsLayout->addWidget(statusLabel);
    
    // Connect signals
    connect(addVertexButton, &QPushButton::clicked, this, &MainWindow::addVertex);
    connect(addEdgeButton, &QPushButton::clicked, this, &MainWindow::addEdge);
    connect(removeEdgeButton, &QPushButton::clicked, this, &MainWindow::removeEdge);
    connect(bfsButton, &QPushButton::clicked, this, &MainWindow::startBfs);
    connect(bfsClearButton, &QPushButton::clicked, this, &MainWindow::clearBfs);
    connect(animationSpeedSlider, &QSlider::valueChanged, this, &MainWindow::animationSpeedChanged);
    
    // Connect the graphVisualizer's BFS status signal to our slot
    connect(graphVisualizer, &GraphVisualizer::bfsStatusMessage, 
            this, &MainWindow::updateBfsStatus);

    // Add the controls to the status bar
    ui->statusbar->addWidget(controlsWidget);
}

void MainWindow::removeEdge()
{
    if (!graphVisualizer) return;
    bool ok1 = false, ok2 = false;
    int fromVal = edgeFromEdit->text().toInt(&ok1);
    int toVal = edgeToEdit->text().toInt(&ok2);
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
    if (graphVisualizer) {
        graphVisualizer->clear();
        delete graphVisualizer;
        graphVisualizer = nullptr;
    }

    // Clear all rectangles
    for (Rectangle* rectangle : rectangles) {
        delete rectangle;
    }
    rectangles.clear();

    // Remove addVertexButton, addEdgeButton, and edits from status bar if they exist
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
        startVertexCombo = nullptr;
        edgeFromEdit = nullptr;
        edgeToEdit = nullptr;
        statusLabel = nullptr;
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

void MainWindow::updateStartVertexCombo()
{
    if (!graphVisualizer || !startVertexCombo) return;
    
    // Remember the current selection if possible
    int currentIndex = startVertexCombo->currentIndex();
    int currentValue = (currentIndex >= 0) ? startVertexCombo->currentText().toInt() : -1;
    
    // Clear the dropdown
    startVertexCombo->clear();
    
    // Get all vertices and add them to the dropdown
    auto vertices = graphVisualizer->getVertices();
    for (VisualVertex* vertex : vertices) {
        startVertexCombo->addItem(QString::number(vertex->value));
    }
    
    // Try to restore the previous selection
    if (currentValue >= 0) {
        int index = startVertexCombo->findText(QString::number(currentValue));
        if (index >= 0) {
            startVertexCombo->setCurrentIndex(index);
        }
    }
}

void MainWindow::startBfs()
{
    if (!graphVisualizer || startVertexCombo->count() == 0) {
        return;
    }
    
    // Get the selected start vertex value
    bool ok = false;
    int startValue = startVertexCombo->currentText().toInt(&ok);
    if (!ok) {
        return;
    }
    
    // Start the BFS animation
    graphVisualizer->startBfsAnimation(startValue);
}

void MainWindow::updateBfsStatus(const QString& message)
{
    if (statusLabel) {
        statusLabel->setText(message);
        statusLabel->setToolTip(message); // Also set as tooltip in case it's too long
    }
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
        // The slider gives higher values for slower speeds (more delay)
        // So we need to invert it to make the slider feel natural
        int delay = 3000 - value; // Invert the range: 100ms to 2000ms becomes 2900ms to 1000ms
        graphVisualizer->setAnimationDelay(delay);
        
        // Update status to show the new speed
        QString speedText = QString("Animation delay set to %1 ms").arg(delay);
        updateBfsStatus(speedText);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

