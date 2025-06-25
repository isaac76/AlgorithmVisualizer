#include <QtTest/QtTest>
#include <string>
#include "../queue.h"

class TestQueue : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testQueueAndDequeue();
    void testEmptyQueue();     // Test empty queue operations
    void testNullPointers();   // Test null pointer handling
    void testClear();          // Test clearing the queue
    void testPeekEmpty();      // Test peeking on empty queue
};

void TestQueue::init()
{
    // Setup code that runs before each test
}

void TestQueue::cleanup()
{
    // Cleanup code that runs after each test
}

void TestQueue::testQueueAndDequeue()
{
    // Create a queue
    Queue<int> queue;

    // Enqueue some elements
    int* data1 = new int(1);
    int* data2 = new int(2);
    int* data3 = new int(3);

    queue.enqueue(data1);
    queue.enqueue(data2);
    queue.enqueue(data3);

    // Check the size of the queue
    QCOMPARE(queue.getSize(), 3);

    // Peek at the front element
    int* front = queue.peek();
    QVERIFY(front != nullptr);
    QCOMPARE(*front, 1);

    // Dequeue elements and check order
    int* dequeuedData;
    queue.dequeue(&dequeuedData);
    QVERIFY(dequeuedData != nullptr);
    QCOMPARE(*dequeuedData, 1);
    delete dequeuedData; // Clean up

    queue.dequeue(&dequeuedData);
    QVERIFY(dequeuedData != nullptr);
    QCOMPARE(*dequeuedData, 2);
    delete dequeuedData; // Clean up

    queue.dequeue(&dequeuedData);
    QVERIFY(dequeuedData != nullptr);
    QCOMPARE(*dequeuedData, 3);
    delete dequeuedData; // Clean up

    // Queue should now be empty
    QCOMPARE(queue.getSize(), 0);
}

void TestQueue::testEmptyQueue()
{
    Queue<int> queue;
    
    // Test operations on empty queue
    QCOMPARE(queue.getSize(), 0);
    
    // Dequeue from empty queue should set pointer to null
    int* data = nullptr;
    queue.dequeue(&data);
    QVERIFY(data == nullptr);
    
    // Peeking at an empty queue should return nullptr
    QVERIFY(queue.peek() == nullptr);
}

void TestQueue::testNullPointers()
{
    Queue<int> queue;
    
    // Enqueueing nullptr should be a no-op
    queue.enqueue(nullptr);
    QCOMPARE(queue.getSize(), 0);
    
    // Enqueue valid data
    int* value = new int(42);
    queue.enqueue(value);
    QCOMPARE(queue.getSize(), 1);
    
    // Dequeue with null pointer shouldn't crash
    queue.dequeue(nullptr);
    QCOMPARE(queue.getSize(), 0);
    
    // Check that enqueuing and then dequeuing with nullptr works
    queue.enqueue(new int(10));
    queue.dequeue(nullptr);
    QCOMPARE(queue.getSize(), 0);
}

void TestQueue::testClear()
{
    Queue<int> queue;
    
    // Add several elements
    for(int i = 0; i < 5; i++) {
        queue.enqueue(new int(i));
    }
    
    QCOMPARE(queue.getSize(), 5);
    
    // Clear all elements
    int* data;
    while(queue.getSize() > 0) {
        queue.dequeue(&data);
        delete data;
    }
    
    // Queue should now be empty
    QCOMPARE(queue.getSize(), 0);
    QVERIFY(queue.peek() == nullptr);
}

void TestQueue::testPeekEmpty()
{
    Queue<int> queue;
    
    // Peek on empty queue
    QVERIFY(queue.peek() == nullptr);
    
    // Add and remove to end with empty
    int* value = new int(100);
    queue.enqueue(value);
    
    QVERIFY(queue.peek() != nullptr);
    QCOMPARE(*queue.peek(), 100);
    
    int* removed;
    queue.dequeue(&removed);
    delete removed;
    
    // Should be empty again
    QVERIFY(queue.peek() == nullptr);
}

QTEST_APPLESS_MAIN(TestQueue)
#include "tst_queue.moc"