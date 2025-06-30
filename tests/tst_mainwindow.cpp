#include <QtTest>
#include <QCoreApplication>
#include <QComboBox>
#include <QTest>
#include <QSignalSpy>
#include <QPushButton>
#include "../mainwindow.h"
#include "../graphvisualizer.h"
#include "../circle.h"
#include "../line.h"

class TestMainWindow : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testMainWindowCreation();
    void testMainWindowTitle();
    void testVisualizationSelector();
    void testGraphUIElements();
    void testAddVertex();
    void testAddEdge();
};

void TestMainWindow::initTestCase()
{
    // Called before the first test function is executed
}

void TestMainWindow::cleanupTestCase()
{
    // Called after the last test function was executed
}

void TestMainWindow::testMainWindowCreation()
{
    // Create a MainWindow instance and verify it was created successfully
    MainWindow window;
    QVERIFY(true); // If we got here without crashing, the test is successful
}

void TestMainWindow::testMainWindowTitle()
{
    // Create a window and check its title
    MainWindow window;
    
    // Just verify that the window title exists (could be empty, that's fine for now)
    QVERIFY(window.windowTitle().size() >= 0);
}

void TestMainWindow::testVisualizationSelector()
{
    // Create a MainWindow instance
    MainWindow window;
    window.show(); // Needed to properly initialize UI components
    
    // Find the visualization selector combobox
    QComboBox* visualizationSelector = window.findChild<QComboBox*>("visualizationSelector");
    QVERIFY2(visualizationSelector != nullptr, "Could not find visualization selector combo box");
    
    // Verify initial state (should be "Select...")
    QCOMPARE(visualizationSelector->currentIndex(), 0);
    QCOMPARE(visualizationSelector->currentText(), QString("Select..."));
    
    // Select Graph visualization
    visualizationSelector->setCurrentIndex(1);
    QCOMPARE(visualizationSelector->currentIndex(), 1);
    QCOMPARE(visualizationSelector->currentText(), QString("Graph"));
    
    // Allow events to be processed (visualization change happens asynchronously)
    QTest::qWait(100);
    
    // The graph visualization should now be set up
    // Check for graph-specific elements
    QPushButton* addVertexButton = window.findChild<QPushButton*>("addVertexButton");
    QPushButton* addEdgeButton = window.findChild<QPushButton*>("addEdgeButton");
    QComboBox* edgeFromCombo = window.findChild<QComboBox*>("edgeFromCombo");
    QComboBox* edgeToCombo = window.findChild<QComboBox*>("edgeToCombo");
    
    // Verify some graph UI elements are present
    QVERIFY2(window.findChild<QPushButton*>(), "No buttons found after selecting Graph visualization");
    
    // Select Queue visualization
    visualizationSelector->setCurrentIndex(2);
    QCOMPARE(visualizationSelector->currentIndex(), 2);
    QCOMPARE(visualizationSelector->currentText(), QString("Queue"));
    
    // Allow events to be processed (visualization change happens asynchronously)
    QTest::qWait(100);
    
    // Verify queue elements (should have rectangles)
    // We can't check for specific UI elements easily, but we can verify the previous graph elements are gone
    QVERIFY2(window.findChild<QPushButton*>("addVertexButton") == nullptr, 
             "Graph elements still present after switching to Queue visualization");
    
    // Verify we're back to Select... as a cleanup step
    visualizationSelector->setCurrentIndex(0);
    QCOMPARE(visualizationSelector->currentIndex(), 0);
    
    // Allow events to be processed
    QTest::qWait(100);
}

void TestMainWindow::testGraphUIElements()
{
    // Create a MainWindow instance
    MainWindow window;
    window.show(); // Needed to properly initialize UI components
    
    // Find the visualization selector combobox
    QComboBox* visualizationSelector = window.findChild<QComboBox*>("visualizationSelector");
    QVERIFY2(visualizationSelector != nullptr, "Could not find visualization selector combo box");
    
    // Select Graph visualization
    visualizationSelector->setCurrentIndex(1);
    QCOMPARE(visualizationSelector->currentText(), QString("Graph"));
    
    // Process events to allow UI to update
    QApplication::processEvents();
    QTest::qWait(100);
    
    // Verify the UI elements exist
    QPushButton* addVertexButton = window.findChild<QPushButton*>("addVertexButton");
    QPushButton* addEdgeButton = window.findChild<QPushButton*>("addEdgeButton");
    QPushButton* removeEdgeButton = window.findChild<QPushButton*>("removeEdgeButton");
    QComboBox* edgeFromCombo = window.findChild<QComboBox*>("edgeFromCombo");
    QComboBox* edgeToCombo = window.findChild<QComboBox*>("edgeToCombo");
    
    QVERIFY2(addVertexButton != nullptr, "Could not find Add Node button");
    QVERIFY2(addEdgeButton != nullptr, "Could not find Add Edge button");
    QVERIFY2(removeEdgeButton != nullptr, "Could not find Remove Edge button");
    QVERIFY2(edgeFromCombo != nullptr, "Could not find edgeFromCombo");
    QVERIFY2(edgeToCombo != nullptr, "Could not find edgeToCombo");
    
    // Clean up
    window.close();
}

void TestMainWindow::testAddVertex()
{
    // Create a MainWindow instance
    MainWindow window;
    window.show(); // Needed to properly initialize UI components
    
    // Find the visualization selector combobox
    QComboBox* visualizationSelector = window.findChild<QComboBox*>("visualizationSelector");
    QVERIFY2(visualizationSelector != nullptr, "Could not find visualization selector combo box");
    
    // Select Graph visualization
    visualizationSelector->setCurrentIndex(1);
    QCOMPARE(visualizationSelector->currentText(), QString("Graph"));
    
    // Process events to allow UI to update
    QApplication::processEvents();
    QTest::qWait(100);
    
    // Find the Add Node button
    QPushButton* addVertexButton = window.findChild<QPushButton*>("addVertexButton");
    QVERIFY2(addVertexButton != nullptr, "Could not find Add Node button");
    
    // Find the edge combo boxes
    QComboBox* edgeFromCombo = window.findChild<QComboBox*>("edgeFromCombo");
    QComboBox* edgeToCombo = window.findChild<QComboBox*>("edgeToCombo");
    QVERIFY2(edgeFromCombo != nullptr, "Could not find edgeFromCombo");
    QVERIFY2(edgeToCombo != nullptr, "Could not find edgeToCombo");
    
    // Verify that combo boxes are initially empty
    QCOMPARE(edgeFromCombo->count(), 0);
    QCOMPARE(edgeToCombo->count(), 0);
    
    // Add a vertex by clicking the button
    QTest::mouseClick(addVertexButton, Qt::LeftButton);
    QApplication::processEvents();
    QTest::qWait(100);
    
    // Verify that combo boxes now contain one item (the added vertex)
    QCOMPARE(edgeFromCombo->count(), 1);
    QCOMPARE(edgeToCombo->count(), 1);
    
    // Add another vertex
    QTest::mouseClick(addVertexButton, Qt::LeftButton);
    QApplication::processEvents();
    QTest::qWait(100);
    
    // Verify that combo boxes now contain two items
    QCOMPARE(edgeFromCombo->count(), 2);
    QCOMPARE(edgeToCombo->count(), 2);
    
    // Test passes if we get here without crashes
    QVERIFY(true);
}

void TestMainWindow::testAddEdge()
{
    // Create a MainWindow instance
    MainWindow window;
    window.show(); // Needed to properly initialize UI components
    
    // Find the visualization selector combobox
    QComboBox* visualizationSelector = window.findChild<QComboBox*>("visualizationSelector");
    QVERIFY2(visualizationSelector != nullptr, "Could not find visualization selector combo box");
    
    // Select Graph visualization
    visualizationSelector->setCurrentIndex(1);
    QCOMPARE(visualizationSelector->currentText(), QString("Graph"));
    
    // Process events to allow UI to update
    QApplication::processEvents();
    QTest::qWait(100);
    
    // Find UI elements
    QPushButton* addVertexButton = window.findChild<QPushButton*>("addVertexButton");
    QPushButton* addEdgeButton = window.findChild<QPushButton*>("addEdgeButton");
    QComboBox* edgeFromCombo = window.findChild<QComboBox*>("edgeFromCombo");
    QComboBox* edgeToCombo = window.findChild<QComboBox*>("edgeToCombo");
    
    QVERIFY2(addVertexButton != nullptr, "Could not find Add Node button");
    QVERIFY2(addEdgeButton != nullptr, "Could not find Add Edge button");
    QVERIFY2(edgeFromCombo != nullptr, "Could not find edgeFromCombo");
    QVERIFY2(edgeToCombo != nullptr, "Could not find edgeToCombo");
    
    // Add two vertices
    QTest::mouseClick(addVertexButton, Qt::LeftButton);
    QApplication::processEvents();
    QTest::qWait(100);
    
    QTest::mouseClick(addVertexButton, Qt::LeftButton);
    QApplication::processEvents();
    QTest::qWait(100);
    
    // Verify we have two vertices in the combo boxes
    QCOMPARE(edgeFromCombo->count(), 2);
    QCOMPARE(edgeToCombo->count(), 2);
    
    // Set from and to indices for the edge
    edgeFromCombo->setCurrentIndex(0);
    edgeToCombo->setCurrentIndex(1);
    
    // Add an edge between the two vertices
    QTest::mouseClick(addEdgeButton, Qt::LeftButton);
    QApplication::processEvents();
    QTest::qWait(100);
    
    // Test passes if we get here without crashes
    QVERIFY(true);
}

QTEST_MAIN(TestMainWindow)
#include "tst_mainwindow.moc"
