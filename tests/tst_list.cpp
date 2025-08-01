#include <QtTest/QtTest>
#include "../list.h"

class TestList : public QObject
{
    Q_OBJECT

private slots:
    // Runs before each test function
    void init();
    
    // Runs after each test function
    void cleanup();
    
    // Test functions - each test is a separate function
    void testInsert();
    void testRemove();
    void testOwnership();
    void testIteration();
    void testEmpty();
    void testEdgeCases();
    void testNullOperations();
};

// Simple test data class
class TestData {
public:
    int value;
    
    TestData(int val = 0) : value(val) {}
    
    // Operator to check equality for tests
    bool operator==(const TestData& other) const {
        return value == other.value;
    }
};

void TestList::init()
{
    // Setup code that runs before each test
}

void TestList::cleanup()
{
    // Cleanup code that runs after each test
}

void TestList::testInsert()
{
    // Create a list
    List<TestData> list;
    
    // Create some test data
    TestData* data1 = new TestData(1);
    TestData* data2 = new TestData(2);
    TestData* data3 = new TestData(3);
    
    // Test insert at head (null means insert at head)
    list.insert(nullptr, data1, true);
    
    // Make sure we have a head node and it has the right data
    QVERIFY(list.head() != nullptr);
    QVERIFY(list.head()->data() != nullptr);
    QCOMPARE(list.head()->data()->value, 1);
    
    // Insert more data (after the head node)
    list.insert(list.head(), data2, true);
    
    // Make sure the second node exists and has the right data
    QVERIFY(list.head()->next() != nullptr);
    QVERIFY(list.head()->next()->data() != nullptr);
    QCOMPARE(list.head()->next()->data()->value, 2);
    
    // Insert a third node (after the second node)
    list.insert(list.head()->next(), data3, true);
    
    // Verify the complete list
    QVERIFY(list.head() != nullptr);
    QVERIFY(list.head()->data() != nullptr);
    QCOMPARE(list.head()->data()->value, 1);
    
    QVERIFY(list.head()->next() != nullptr);
    QVERIFY(list.head()->next()->data() != nullptr);
    QCOMPARE(list.head()->next()->data()->value, 2);
    
    QVERIFY(list.head()->next()->next() != nullptr);
    QVERIFY(list.head()->next()->next()->data() != nullptr);
    QCOMPARE(list.head()->next()->next()->data()->value, 3);
}

void TestList::testRemove()
{
    // Create a list
    List<TestData> list;
    
    // Add some data
    list.insert(nullptr, new TestData(1), true);
    list.insert(list.head(), new TestData(2), true);
    list.insert(list.head()->next(), new TestData(3), true);
    
    // Verify initial list structure
    QVERIFY(list.head() != nullptr);
    QVERIFY(list.head()->next() != nullptr);
    QVERIFY(list.head()->next()->next() != nullptr);
    
    // Remove from the middle
    TestData* removed = nullptr;
    list.remove(list.head(), &removed);
    
    // Verify the data was removed correctly
    QVERIFY(removed != nullptr);
    QCOMPARE(removed->value, 2);
    
    // Verify the list structure after removal
    QVERIFY(list.head() != nullptr);
    QVERIFY(list.head()->data() != nullptr);
    QCOMPARE(list.head()->data()->value, 1);
    
    QVERIFY(list.head()->next() != nullptr);
    QVERIFY(list.head()->next()->data() != nullptr);
    QCOMPARE(list.head()->next()->data()->value, 3);
    
    // Since we added the data with takeOwnership=true, the list has already
    // removed it from its ownership set, so we're responsible for it now
    delete removed;
}

void TestList::testOwnership()
{
    // Use a separate scope to test automatic cleanup
    {
        // Test that owned data is deleted automatically
        List<TestData> list;
    
        // Add data with ownership
        list.insert(nullptr, new TestData(1), true);
    
        // Add data without ownership
        TestData staticData(2);
        list.insert(list.head(), &staticData, false);
        
        // Verify the data was added
        QVERIFY(list.head() != nullptr);
        QVERIFY(list.head()->data() != nullptr);
        QCOMPARE(list.head()->data()->value, 1);
        
        QVERIFY(list.head()->next() != nullptr);
        QVERIFY(list.head()->next()->data() != nullptr);
        QCOMPARE(list.head()->next()->data()->value, 2);
    }
    // List goes out of scope here and should delete the owned data
    // but not the static data
    
    // The list destructor should delete data1 but not staticData
    // This test passes if there's no crash when list goes out of scope
}

void TestList::testIteration()
{
    List<TestData> list;
    
    // Add some data
    list.insert(nullptr, new TestData(1), true);
    list.insert(list.head(), new TestData(2), true);
    list.insert(list.head()->next(), new TestData(3), true);
    
    // Verify the list was populated correctly
    QVERIFY(list.head() != nullptr);
    QVERIFY(list.head()->next() != nullptr);
    QVERIFY(list.head()->next()->next() != nullptr);
    
    // Test iteration through the list
    int sum = 0;
    ListNode<TestData>* current = list.head();
    while (current != nullptr) {
        QVERIFY(current->data() != nullptr);
        sum += current->data()->value;
        current = current->next();
    }
    
    QCOMPARE(sum, 6); // 1+2+3
}

void TestList::testEmpty() {
    List<TestData> list;
    
    // Test empty list properties
    QCOMPARE(list.getSize(), 0);
    QVERIFY(list.head() == nullptr);
    QVERIFY(list.tail() == nullptr);
    
    // Test removing from empty list
    TestData* removed = nullptr;
    list.remove(nullptr, &removed);
    QVERIFY(removed == nullptr);
    QCOMPARE(list.getSize(), 0);
    
    // Test removing with null output pointer - should not crash
    list.remove(nullptr, nullptr);
    QCOMPARE(list.getSize(), 0);
    
    // Test inserting null data
    list.insert(nullptr, nullptr, false);
    QCOMPARE(list.getSize(), 0);  // Size should still be 0
}

void TestList::testEdgeCases() {
    List<TestData> list;
    TestData* data1 = new TestData(1);
    
    // Insert valid data
    list.insert(nullptr, data1, true);
    QCOMPARE(list.getSize(), 1);
    
    // Try to insert at a non-existent position 
    // This should be handled safely by inserting at head instead 
    ListNode<TestData>* nonExistentNode = reinterpret_cast<ListNode<TestData>*>(0xDEADBEEF);
    
    // Try to insert at invalid position but with valid data
    TestData* data2 = new TestData(2);
    list.insert(nonExistentNode, data2, true);
    
    // Verify the list state after insertion - should have added to head
    QCOMPARE(list.getSize(), 2);
    QVERIFY(list.head() != nullptr);
    
    // Now try removing invalid node
    TestData* removed = nullptr;
    list.remove(nonExistentNode, &removed);
    
    // This should be handled safely without segfault
    QVERIFY(list.getSize() > 0); // Size should still be positive
}

void TestList::testNullOperations() {
    List<TestData> list;
    
    // Test null data operations
    TestData* nullData = nullptr;
    list.insert(nullptr, nullData, false);
    QCOMPARE(list.getSize(), 0);  // Should not insert null data
    
    // Test null node/data removal in empty list - should not crash
    list.remove(nullptr, nullptr);  
    QCOMPARE(list.getSize(), 0);
    
    // Insert a valid node first
    TestData* data = new TestData(5);
    list.insert(nullptr, data, true);
    QCOMPARE(list.getSize(), 1);
    
    // Then remove without caring about the output
    list.remove(nullptr, nullptr);
    QCOMPARE(list.getSize(), 0);
    
    // Try removing with null parameters on empty list
    list.remove(nullptr, nullptr);
    QCOMPARE(list.getSize(), 0);
}

QTEST_APPLESS_MAIN(TestList)
#include "tst_list.moc"
