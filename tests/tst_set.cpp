#include <QtTest/QtTest>
#include <string>
#include "../set.h"

class TestSet : public QObject
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
    void testIsMember();
    void testOwnership();
    void testUnion();
    void testIntersection();
    void testDifference();
    void testSubset();
    void testEqualSet();
};

// Simple test data class
class TestData
{
public:
    int value;

    TestData(int val = 0) : value(val) {}

    bool operator==(const TestData &other) const
    {
        return value == other.value;
    }
};

// Comparison function for TestData - with the correct signature for Set's constructor
int compareTestData(const TestData *v1, const TestData *v2)
{
    if (v1 == nullptr || v2 == nullptr)
    {
        return 0; // Not equal if any is null
    }
    return (v1->value == v2->value) ? 1 : 0;
}

struct CompareTestData
{
    bool operator()(const TestData &v1, const TestData &v2) const
    {
        return v1.value == v2.value;
    }
};

void TestSet::init()
{
    // Setup code that runs before each test
    // Nothing needed here for these tests
}

void TestSet::cleanup()
{
    // Cleanup code that runs after each test
    // Nothing needed here as each test handles its own cleanup
}

void TestSet::testInsert()
{
    // Create a set with our comparison function
    Set<TestData, CompareTestData> set(compareTestData);

    // Create some test data
    TestData *d1 = new TestData(1);
    TestData *d2 = new TestData(2);
    TestData *d3 = new TestData(3);

    // Insert data into the set
    set.insert(d1, true); // Set takes ownership
    set.insert(d2, true);
    set.insert(d3, true);

    // Verify the size is correct
    QCOMPARE(set.getSize(), 3);

    // Verify the head and tail are set correctly
    QVERIFY(set.head() != nullptr);
    QVERIFY(set.tail() != nullptr);

    // Try to insert a duplicate value (not identical object)
    TestData *duplicate = new TestData(1);
    set.insert(duplicate, true);

    // Verify that the duplicate wasn't added (size still 3)
    QCOMPARE(set.getSize(), 3);

    // Clean up the duplicate since it wasn't added to the set
    delete duplicate;

    // Check internal structure by traversing nodes (verify all elements are present)
    SetNode<TestData> *current = set.head();
    bool found1 = false, found2 = false, found3 = false;

    while (current != nullptr)
    {
        QVERIFY(current->data() != nullptr);
        int val = current->data()->value;
        if (val == 1)
            found1 = true;
        if (val == 2)
            found2 = true;
        if (val == 3)
            found3 = true;
        current = current->next();
    }

    QVERIFY(found1);
    QVERIFY(found2);
    QVERIFY(found3);
}

void TestSet::testRemove()
{
    // Create a set
    Set<TestData, CompareTestData> set(compareTestData);

    // Create some test data
    TestData *d1 = new TestData(1);
    TestData *d2 = new TestData(2);
    TestData *d3 = new TestData(3);

    // Insert data into the set
    set.insert(d1, true);
    set.insert(d2, true);
    set.insert(d3, true);

    // Verify initial size
    QCOMPARE(set.getSize(), 3);

    // Remove an element by value (not the same pointer)
    TestData searchKey(2);
    QVERIFY(set.remove(&searchKey));

    // Verify the size decreased
    QCOMPARE(set.getSize(), 2);

    // Ensure d2 is no longer in the set
    QVERIFY(!set.isMember(&searchKey));

    // Try to remove a non-existent element
    TestData nonExistent(4);
    QVERIFY(!set.remove(&nonExistent));

    // Verify the size is still 2
    QCOMPARE(set.getSize(), 2);

    // Remove the head element
    TestData k1(1);
    QVERIFY(set.remove(&k1));
    QCOMPARE(set.getSize(), 1);

    // Verify the head has been updated correctly
    QVERIFY(set.head() != nullptr);
    QVERIFY(set.head()->data() != nullptr);
    QCOMPARE(set.head()->data()->value, 3);

    // Remove the last element (tail)
    TestData k3(3);
    QVERIFY(set.remove(&k3));

    // Verify the set is empty
    QCOMPARE(set.getSize(), 0);
    QVERIFY(set.head() == nullptr);
    QVERIFY(set.tail() == nullptr);

    // Since we used owned data throughout, we don't need to manually delete anything
}

void TestSet::testIsMember()
{
    // Create a set
    Set<TestData, CompareTestData> set(compareTestData);

    // Create some test data
    TestData *d1 = new TestData(1);
    TestData *d2 = new TestData(2);
    TestData *d3 = new TestData(3);

    // Insert data into the set
    set.insert(d1, true);
    set.insert(d2, true);
    set.insert(d3, true);

    // Check for members that are in the set (using value equality, not pointer equality)
    TestData find1(1);
    TestData find2(2);
    TestData find3(3);
    QVERIFY(set.isMember(&find1));
    QVERIFY(set.isMember(&find2));
    QVERIFY(set.isMember(&find3));

    // Check for a member that is not in the set
    TestData find4(4);
    QVERIFY(!set.isMember(&find4));

    // Check directly using the original pointers
    QVERIFY(set.isMember(d1));
    QVERIFY(set.isMember(d2));
    QVERIFY(set.isMember(d3));

    // Test with null pointer (should return false)
    QVERIFY(!set.isMember(nullptr));
}

void TestSet::testOwnership()
{
    // Test 1: Verify set cleans up owned data when it goes out of scope
    {
        Set<TestData, CompareTestData> set(compareTestData);

        // Create data with the set taking ownership
        TestData *d1 = new TestData(1);
        TestData *d2 = new TestData(2);

        set.insert(d1, true);
        set.insert(d2, true);

        // Verify ownership
        QVERIFY(set.hasOwnership(d1));
        QVERIFY(set.hasOwnership(d2));

        // No need to delete d1 or d2 - the set should handle it
    }
    // Set goes out of scope here and should delete all owned data
    // This test passes if there's no crash or memory leak

    // Test 2: Mix of owned and non-owned data
    {
        Set<TestData, CompareTestData> set(compareTestData);

        // Create owned data
        TestData *owned = new TestData(10);
        set.insert(owned, true);
        QVERIFY(set.hasOwnership(owned));

        // Create non-owned data
        TestData onStack(20);
        set.insert(&onStack, false);
        QVERIFY(!set.hasOwnership(&onStack));

        // Verify both are in the set
        QVERIFY(set.isMember(owned));
        QVERIFY(set.isMember(&onStack));
    }
    // Set will clean up owned data but not onStack (which will be cleaned automatically)

    // Test 3: Removing an owned element
    Set<TestData, CompareTestData> set(compareTestData);
    TestData *d3 = new TestData(3);

    // Insert with ownership
    set.insert(d3, true);
    QVERIFY(set.hasOwnership(d3));

    // Remove by value - set should release ownership
    TestData key(3);
    QVERIFY(set.remove(&key));

    // Verify the set no longer has ownership
    QVERIFY(!set.hasOwnership(d3));

    // Clean up manually since we've removed it from the set
    delete d3;
}

void TestSet::testUnion()
{
    // Create two sets
    Set<TestData, CompareTestData> set1(compareTestData);
    Set<TestData, CompareTestData> set2(compareTestData);

    // Create test data for set1
    TestData *d1 = new TestData(1);
    TestData *d2 = new TestData(2);
    TestData *d3 = new TestData(3);

    // Create test data for set2
    TestData *d3_dup = new TestData(3); // Duplicate of d3
    TestData *d4 = new TestData(4);
    TestData *d5 = new TestData(5);

    // Insert into sets
    set1.insert(d1, true);
    set1.insert(d2, true);
    set1.insert(d3, true);

    set2.insert(d3_dup, true);
    set2.insert(d4, true);
    set2.insert(d5, true);

    // Create a result set
    Set<TestData, CompareTestData> result(compareTestData);

    // Perform the union operation
    Set<TestData, CompareTestData>::unionSet(&result, &set1, &set2);

    // Check the result
    QCOMPARE(result.getSize(), 5); // 1, 2, 3, 4, 5

    // Verify each element is in the result
    for (int i = 1; i <= 5; i++)
    {
        TestData find(i);
        QVERIFY(result.isMember(&find));
    }

    // Test some edge cases

    // Empty set union
    Set<TestData, CompareTestData> empty1(compareTestData);
    Set<TestData, CompareTestData> empty2(compareTestData);
    Set<TestData, CompareTestData> emptyResult(compareTestData);

    // Empty U Empty = Empty
    Set<TestData, CompareTestData>::unionSet(&emptyResult, &empty1, &empty2);
    QCOMPARE(emptyResult.getSize(), 0);

    // Set U Empty = Set
    Set<TestData, CompareTestData> nonEmptyResult(compareTestData);
    Set<TestData, CompareTestData>::unionSet(&nonEmptyResult, &set1, &empty1);

    QCOMPARE(nonEmptyResult.getSize(), 3);
    TestData find1(1), find2(2), find3(3);
    QVERIFY(nonEmptyResult.isMember(&find1));
    QVERIFY(nonEmptyResult.isMember(&find2));
    QVERIFY(nonEmptyResult.isMember(&find3));
}

void TestSet::testIntersection()
{
    // Create two sets
    Set<TestData, CompareTestData> set1(compareTestData);
    Set<TestData, CompareTestData> set2(compareTestData);

    // Create test data for set1
    TestData *d1 = new TestData(1);
    TestData *d2 = new TestData(2);
    TestData *d3 = new TestData(3);

    // Create test data for set2
    TestData *d2_dup = new TestData(2); // Duplicate of d2
    TestData *d3_dup = new TestData(3); // Duplicate of d3
    TestData *d4 = new TestData(4);

    // Insert into sets
    set1.insert(d1, true);
    set1.insert(d2, true);
    set1.insert(d3, true);

    set2.insert(d2_dup, true);
    set2.insert(d3_dup, true);
    set2.insert(d4, true);

    // Create a result set
    Set<TestData, CompareTestData> result(compareTestData);

    // Perform the intersection operation
    Set<TestData, CompareTestData>::intersectionSet(&result, &set1, &set2);

    // Check the result
    QCOMPARE(result.getSize(), 2); // 2, 3

    // Verify the intersection elements are in the result
    TestData find1(1);
    TestData find2(2);
    TestData find3(3);
    TestData find4(4);

    QVERIFY(!result.isMember(&find1)); // Not in intersection
    QVERIFY(result.isMember(&find2));  // In intersection
    QVERIFY(result.isMember(&find3));  // In intersection
    QVERIFY(!result.isMember(&find4)); // Not in intersection

    // Test edge cases

    // Empty set intersection
    Set<TestData, CompareTestData> empty1(compareTestData);
    Set<TestData, CompareTestData> empty2(compareTestData);
    Set<TestData, CompareTestData> emptyResult(compareTestData);

    // Empty ∩ Empty = Empty
    Set<TestData, CompareTestData>::intersectionSet(&emptyResult, &empty1, &empty2);
    QCOMPARE(emptyResult.getSize(), 0);

    // Set ∩ Empty = Empty
    Set<TestData, CompareTestData> nonEmptyResult(compareTestData);
    Set<TestData, CompareTestData>::intersectionSet(&nonEmptyResult, &set1, &empty1);
    QCOMPARE(nonEmptyResult.getSize(), 0);

    // Disjoint sets
    Set<TestData, CompareTestData> disjointSet1(compareTestData);
    Set<TestData, CompareTestData> disjointSet2(compareTestData);

    disjointSet1.insert(new TestData(100), true);
    disjointSet1.insert(new TestData(200), true);

    disjointSet2.insert(new TestData(300), true);
    disjointSet2.insert(new TestData(400), true);

    Set<TestData, CompareTestData> disjointResult(compareTestData);
    Set<TestData, CompareTestData>::intersectionSet(&disjointResult, &disjointSet1, &disjointSet2);

    QCOMPARE(disjointResult.getSize(), 0); // Empty intersection
}

void TestSet::testDifference()
{
    // Create two sets
    Set<TestData, CompareTestData> set1(compareTestData);
    Set<TestData, CompareTestData> set2(compareTestData);

    // Create test data for set1
    TestData *d1 = new TestData(1);
    TestData *d2 = new TestData(2);
    TestData *d3 = new TestData(3);

    // Create test data for set2
    TestData *d2_dup = new TestData(2); // Duplicate of d2
    TestData *d3_dup = new TestData(3); // Duplicate of d3
    TestData *d4 = new TestData(4);

    // Insert into sets
    set1.insert(d1, true);
    set1.insert(d2, true);
    set1.insert(d3, true);

    set2.insert(d2_dup, true);
    set2.insert(d3_dup, true);
    set2.insert(d4, true);

    // Create a result set
    Set<TestData, CompareTestData> result(compareTestData);

    // Perform the difference operation (set1 - set2)
    Set<TestData, CompareTestData>::differenceSet(&result, &set1, &set2);

    // Check the result
    QCOMPARE(result.getSize(), 1); // Only 1

    // Verify the difference elements are in the result
    TestData find1(1);
    TestData find2(2);
    TestData find4(4);

    QVERIFY(result.isMember(&find1));  // In difference
    QVERIFY(!result.isMember(&find2)); // Not in difference
    QVERIFY(!result.isMember(&find4)); // Not in difference

    // Test the opposite difference (set2 - set1)
    Set<TestData, CompareTestData> result2(compareTestData);
    Set<TestData, CompareTestData>::differenceSet(&result2, &set2, &set1);

    // Check the result of set2 - set1
    QCOMPARE(result2.getSize(), 1); // Only 4

    TestData find3(3);
    QVERIFY(!result2.isMember(&find1)); // Not in difference
    QVERIFY(!result2.isMember(&find2)); // Not in difference
    QVERIFY(!result2.isMember(&find3)); // Not in difference
    QVERIFY(result2.isMember(&find4));  // In difference

    // Test edge cases

    // Empty set difference
    Set<TestData, CompareTestData> empty(compareTestData);
    Set<TestData, CompareTestData> emptyResult(compareTestData);

    // Empty - Set = Empty
    Set<TestData, CompareTestData>::differenceSet(&emptyResult, &empty, &set1);
    QCOMPARE(emptyResult.getSize(), 0);

    // Set - Empty = Set
    Set<TestData, CompareTestData> fullResult(compareTestData);
    Set<TestData, CompareTestData>::differenceSet(&fullResult, &set1, &empty);

    QCOMPARE(fullResult.getSize(), 3);
    QVERIFY(fullResult.isMember(&find1));
    QVERIFY(fullResult.isMember(&find2));
    QVERIFY(fullResult.isMember(&find3));

    // Set - Set = Empty
    Set<TestData, CompareTestData> sameSetResult(compareTestData);
    Set<TestData, CompareTestData>::differenceSet(&sameSetResult, &set1, &set1);
    QCOMPARE(sameSetResult.getSize(), 0);
}

void TestSet::testSubset()
{
    // Create sets
    Set<TestData, CompareTestData> set1(compareTestData);
    Set<TestData, CompareTestData> set2(compareTestData);
    Set<TestData, CompareTestData> set3(compareTestData);

    // Create test data
    TestData *d1 = new TestData(1);
    TestData *d2 = new TestData(2);
    TestData *d3 = new TestData(3);
    TestData *d4 = new TestData(4);

    // Set1 will be {1, 2}
    set1.insert(d1, true);
    set1.insert(d2, true);

    // Set2 will be {1, 2, 3, 4} (a superset of set1)
    TestData *d1_dup = new TestData(1);
    TestData *d2_dup = new TestData(2);
    set2.insert(d1_dup, true);
    set2.insert(d2_dup, true);
    set2.insert(d3, true);
    set2.insert(d4, true);

    // Set3 will be {1, 3} (not a subset or superset of set1)
    TestData *d1_dup2 = new TestData(1);
    TestData *d3_dup = new TestData(3);
    set3.insert(d1_dup2, true);
    set3.insert(d3_dup, true);

    typedef Set<TestData, CompareTestData> TestDataSet;

    // Test subset relationships
    QVERIFY(TestDataSet::isSubset(&set1, &set2));  // set1 is a subset of set2
    QVERIFY(!TestDataSet::isSubset(&set2, &set1)); // set2 is not a subset of set1
    QVERIFY(!TestDataSet::isSubset(&set1, &set3)); // set1 is not a subset of set3
    QVERIFY(!TestDataSet::isSubset(&set3, &set1)); // set3 is not a subset of set1

    // Test edge cases

    // Empty set is a subset of any set
    Set<TestData, CompareTestData> emptySet(compareTestData);
    QVERIFY(TestDataSet::isSubset(&emptySet, &set1));
    QVERIFY(TestDataSet::isSubset(&emptySet, &set2));
    QVERIFY(TestDataSet::isSubset(&emptySet, &set3));
    QVERIFY(TestDataSet::isSubset(&emptySet, &emptySet)); // Empty is a subset of itself

    // Any set is a subset of itself
    QVERIFY(TestDataSet::isSubset(&set1, &set1));
    QVERIFY(TestDataSet::isSubset(&set2, &set2));
    QVERIFY(TestDataSet::isSubset(&set3, &set3));

    // Additional subset tests
    Set<TestData, CompareTestData> set4(compareTestData); // Will be {1, 2} like set1
    TestData *d1_dup3 = new TestData(1);
    TestData *d2_dup3 = new TestData(2);
    set4.insert(d1_dup3, true);
    set4.insert(d2_dup3, true);

    QVERIFY(TestDataSet::isSubset(&set1, &set4)); // Same elements - mutual subsets
    QVERIFY(TestDataSet::isSubset(&set4, &set1));
}

void TestSet::testEqualSet()
{
    // Create sets
    Set<TestData, CompareTestData> set1(compareTestData);
    Set<TestData, CompareTestData> set2(compareTestData);
    Set<TestData, CompareTestData> set3(compareTestData);

    // Create test data for set1
    TestData *d1 = new TestData(1);
    TestData *d2 = new TestData(2);

    // Create test data for set2 (same elements as set1)
    TestData *d1_dup = new TestData(1);
    TestData *d2_dup = new TestData(2);

    // Create test data for set3 (different from set1)
    TestData *d1_dup2 = new TestData(1);
    TestData *d3 = new TestData(3);

    // Insert into sets
    set1.insert(d1, true);
    set1.insert(d2, true);

    set2.insert(d1_dup, true);
    set2.insert(d2_dup, true);

    set3.insert(d1_dup2, true);
    set3.insert(d3, true);

    typedef Set<TestData, CompareTestData> TestDataSet;

    // Test equality
    QVERIFY(TestDataSet::isEqualSet(&set1, &set2));  // set1 equals set2
    QVERIFY(TestDataSet::isEqualSet(&set2, &set1));  // set2 equals set1
    QVERIFY(!TestDataSet::isEqualSet(&set1, &set3)); // set1 does not equal set3
    QVERIFY(!TestDataSet::isEqualSet(&set3, &set1)); // set3 does not equal set1

    // Test edge cases

    // Empty sets are equal to each other
    Set<TestData, CompareTestData> empty1(compareTestData);
    Set<TestData, CompareTestData> empty2(compareTestData);
    QVERIFY(TestDataSet::isEqualSet(&empty1, &empty2));

    // A set is equal to itself
    QVERIFY(TestDataSet::isEqualSet(&set1, &set1));
    QVERIFY(TestDataSet::isEqualSet(&set2, &set2));
    QVERIFY(TestDataSet::isEqualSet(&set3, &set3));

    // Empty set is not equal to non-empty set
    QVERIFY(!TestDataSet::isEqualSet(&empty1, &set1));
    QVERIFY(!TestDataSet::isEqualSet(&set1, &empty1));

    // Test with different insertion order but same elements
    Set<TestData, CompareTestData> set4(compareTestData);
    TestData *d2_dup2 = new TestData(2);
    TestData *d1_dup3 = new TestData(1);

    // Insert in reverse order compared to set1
    set4.insert(d2_dup2, true);
    set4.insert(d1_dup3, true);

    // Should still be equal
    QVERIFY(TestDataSet::isEqualSet(&set1, &set4));
}

QTEST_APPLESS_MAIN(TestSet)
#include "tst_set.moc"
