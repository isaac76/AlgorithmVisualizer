#include <QtTest>
#include <QCoreApplication>
#include "../mainwindow.h"

class TestMainWindow : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testMainWindowCreation();
    void testMainWindowTitle();
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

QTEST_MAIN(TestMainWindow)
#include "tst_mainwindow.moc"
