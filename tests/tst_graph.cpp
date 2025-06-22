#include <QtTest/QtTest>
#include <string>
#include "../graph.h"
#include "../bfs.h"
#include "../bfsvertex.h"

class TestGraph : public QObject
{
    Q_OBJECT

private slots:
    // Runs before each test function
    void init();

    // Runs after each test function
    void cleanup();

    // Test functions - each test is a separate function
    void testInsertVertex();
    void testInsertEdge();
    void testRemoveVertex();
    void testRemoveEdge();
    void testOwnership();
    void testBFS();
    void testCustomFunctors();
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

// Complex test data class with multiple fields
class ComplexData
{
public:
    int id;
    std::string name;
    double weight;

    ComplexData(int i = 0, const std::string& n = "", double w = 0.0) 
        : id(i), name(n), weight(w) {}

    bool operator==(const ComplexData &other) const
    {
        return id == other.id && name == other.name && weight == other.weight;
    }
};

struct CompareTestData
{
    bool operator()(const BfsVertex<TestData> &key1, const BfsVertex<TestData> &key2) const
    {
        // Check for null data pointers first
        if (key1.data == nullptr || key2.data == nullptr)
        {
            return false; // Not equal if any data is null
        }
        
        // Compare the values inside TestData
        return key1.data->value == key2.data->value;
    }
};

struct CompareTestString
{
    bool operator()(const BfsVertex<std::string> &key1, const BfsVertex<std::string> &key2) const
    {
        // Check for null data pointers first
        if (key1.data == nullptr || key2.data == nullptr)
        {
            return false; // Not equal if any data is null
        }
        
        // Compare the string values (not the pointers)
        return *key1.data == *key2.data;
    }
};

// Functor that compares only by ID (partial comparison)
struct CompareById
{
    bool operator()(const BfsVertex<ComplexData> &key1, const BfsVertex<ComplexData> &key2) const
    {
        if (key1.data == nullptr || key2.data == nullptr)
            return false;
            
        return key1.data->id == key2.data->id;
    }
};

// Functor that compares only by name (case-insensitive)
struct CompareByNameCaseInsensitive
{
    bool operator()(const BfsVertex<ComplexData> &key1, const BfsVertex<ComplexData> &key2) const
    {
        if (key1.data == nullptr || key2.data == nullptr)
            return false;
        
        // Convert both strings to lowercase for case-insensitive comparison
        std::string name1 = key1.data->name;
        std::string name2 = key2.data->name;
        
        // Convert to lowercase
        std::transform(name1.begin(), name1.end(), name1.begin(), 
                       [](unsigned char c){ return std::tolower(c); });
        std::transform(name2.begin(), name2.end(), name2.begin(), 
                       [](unsigned char c){ return std::tolower(c); });
                       
        return name1 == name2;
    }
};

// Functor that uses a weighted comparison (more flexible matching)
struct CompareByWeightThreshold
{
    double threshold;
    
    CompareByWeightThreshold(double t = 0.1) : threshold(t) {}
    
    bool operator()(const BfsVertex<ComplexData> &key1, const BfsVertex<ComplexData> &key2) const
    {
        if (key1.data == nullptr || key2.data == nullptr)
            return false;
            
        // Consider items equal if their weight difference is below threshold
        return std::abs(key1.data->weight - key2.data->weight) < threshold;
    }
};


void TestGraph::init()
{
    // Setup code that runs before each test
}

void TestGraph::cleanup()
{
    // Cleanup code that runs after each test
}

void TestGraph::testInsertVertex()
{
    // Create a graph
    Graph<BfsVertex<TestData>, CompareTestData> graph;

    // Create some test vertices
    BfsVertex<TestData> *v1 = new BfsVertex<TestData>(new TestData(1));
    BfsVertex<TestData> *v2 = new BfsVertex<TestData>(new TestData(2));
    BfsVertex<TestData> *v3 = new BfsVertex<TestData>(new TestData(3));

    // Insert vertices into the graph
    QCOMPARE(graph.insertVertex(v1, true), 0);
    QCOMPARE(graph.insertVertex(v2, true), 0);
    QCOMPARE(graph.insertVertex(v3, true), 0);

    // Verify vertex count
    QCOMPARE(graph.getVertexCount(), 3);

    // Try to insert a duplicate vertex
    BfsVertex<TestData> *v1Duplicate = new BfsVertex<TestData>(new TestData(1));
    QCOMPARE(graph.insertVertex(v1Duplicate), 1); // Should return 1 (error) for duplicate

    // Clean up the duplicate since the graph didn't take ownership
    delete v1Duplicate->data;
    delete v1Duplicate;
}

void TestGraph::testInsertEdge()
{
    // Create a graph
    Graph<BfsVertex<TestData>, CompareTestData> graph;

    // Create some test vertices
    BfsVertex<TestData> *v1 = new BfsVertex<TestData>(new TestData(1));
    BfsVertex<TestData> *v2 = new BfsVertex<TestData>(new TestData(2));
    BfsVertex<TestData> *v3 = new BfsVertex<TestData>(new TestData(3));

    // Insert vertices into the graph
    graph.insertVertex(v1, true);
    graph.insertVertex(v2, true);
    graph.insertVertex(v3, true);

    // Create edges
    QCOMPARE(graph.insertEdge(v1, v2), 0);
    QCOMPARE(graph.insertEdge(v2, v3), 0);
    QCOMPARE(graph.insertEdge(v1, v3), 0);

    // Verify edge count
    QCOMPARE(graph.getEdgeCount(), 3);

    // Test adjacency
    QVERIFY(graph.isAdjacentGraph(v1, v2));
    QVERIFY(graph.isAdjacentGraph(v2, v3));
    QVERIFY(graph.isAdjacentGraph(v1, v3));
    QVERIFY(!graph.isAdjacentGraph(v3, v1)); // Directed graph, so this should be false
}

void TestGraph::testRemoveVertex()
{
    // Create a graph
    Graph<BfsVertex<TestData>, CompareTestData> graph;

    // Create some test vertices
    BfsVertex<TestData> *v1 = new BfsVertex<TestData>(new TestData(1));
    BfsVertex<TestData> *v2 = new BfsVertex<TestData>(new TestData(2));
    BfsVertex<TestData> *v3 = new BfsVertex<TestData>(new TestData(3));

    // Insert vertices into the graph
    graph.insertVertex(v1, true);
    graph.insertVertex(v2, true);
    graph.insertVertex(v3, false); // Don't transfer ownership for v3

    // Verify vertex count
    QCOMPARE(graph.getVertexCount(), 3);

    // Remove a vertex - pass a pointer to the pointer we want to remove
    BfsVertex<TestData> *removedVertex = v3; // Start with v3
    QCOMPARE(graph.removeVertex(&removedVertex), 0);

    // Verify vertex count decreased
    QCOMPARE(graph.getVertexCount(), 2);

    // Verify we got back the vertex we removed
    QCOMPARE(removedVertex->data->value, 3);

    // Clean up manually since we didn't transfer ownership
    delete removedVertex->data;
    delete removedVertex;

    // Try to remove a vertex that doesn't exist
    BfsVertex<TestData> *nonExistentVertex = new BfsVertex<TestData>(new TestData(4));
    int result = graph.removeVertex(&nonExistentVertex);
    QCOMPARE(result, -1); // Should fail as vertex doesn't exist

    // Clean up the non-existent vertex we created
    delete nonExistentVertex->data;
    delete nonExistentVertex;
}

void TestGraph::testRemoveEdge()
{
    // Create a graph
    Graph<BfsVertex<TestData>, CompareTestData> graph;

    // Create some test vertices
    BfsVertex<TestData> *v1 = new BfsVertex<TestData>(new TestData(1));
    BfsVertex<TestData> *v2 = new BfsVertex<TestData>(new TestData(2));
    BfsVertex<TestData> *v3 = new BfsVertex<TestData>(new TestData(3));

    // Insert vertices into the graph
    graph.insertVertex(v1, true);
    graph.insertVertex(v2, true);
    graph.insertVertex(v3, true);

    // Create edges
    graph.insertEdge(v1, v2);
    graph.insertEdge(v2, v3);
    graph.insertEdge(v1, v3);

    // Verify initial edge count
    QCOMPARE(graph.getEdgeCount(), 3);

    // Remove an edge - we need to specify which edge to remove
    // Let's remove the edge from v1 to v2
    QCOMPARE(graph.removeEdge(v1, &v2), 0);

    // Verify edge count decreased
    QCOMPARE(graph.getEdgeCount(), 2);

    // Verify adjacency was updated
    QVERIFY(!graph.isAdjacentGraph(v1, v2));
    QVERIFY(graph.isAdjacentGraph(v1, v3)); // This edge should still exist

    // Try removing a non-existent edge
    BfsVertex<TestData> *nonExistentVertex = new BfsVertex<TestData>(new TestData(4));
    int result = graph.removeEdge(v1, &nonExistentVertex);
    QCOMPARE(result, -1); // Should fail as this edge doesn't exist

    // Clean up the non-existent vertex
    delete nonExistentVertex->data;
    delete nonExistentVertex;
}

void TestGraph::testOwnership()
{
    // Use a separate scope to test automatic cleanup
    {
        // Create a graph
        Graph<BfsVertex<TestData>, CompareTestData> graph;

        // Create vertices with graph taking ownership
        BfsVertex<TestData> *v1 = new BfsVertex<TestData>(new TestData(1));
        BfsVertex<TestData> *v2 = new BfsVertex<TestData>(new TestData(2));

        // Insert vertices with ownership transfer
        graph.insertVertex(v1, true);
        graph.insertVertex(v2, true);

        // Create an edge
        graph.insertEdge(v1, v2);

        // Don't manually delete v1 or v2 - the graph should handle it
    }
    // Graph goes out of scope here and should delete all owned vertices

    // This test passes if there's no crash or memory leak

    // Test without ownership
    Graph<BfsVertex<TestData>, CompareTestData> graph;

    // Create vertex without graph taking ownership
    BfsVertex<TestData> *v3 = new BfsVertex<TestData>(new TestData(3));
    graph.insertVertex(v3, false);

    // Must manually delete the vertex since the graph doesn't own it
    delete v3->data;
    delete v3;
}

void TestGraph::testBFS()
{
    // Create a graph for BFS testing
    Graph<BfsVertex<std::string>, CompareTestString> graph;

    // Create vertices
    BfsVertex<std::string> *a = new BfsVertex<std::string>(new std::string("A"));
    BfsVertex<std::string> *b = new BfsVertex<std::string>(new std::string("B"));
    BfsVertex<std::string> *c = new BfsVertex<std::string>(new std::string("C"));
    BfsVertex<std::string> *d = new BfsVertex<std::string>(new std::string("D"));
    BfsVertex<std::string> *e = new BfsVertex<std::string>(new std::string("E"));

    // Insert vertices with ownership transfer
    graph.insertVertex(a, true);
    graph.insertVertex(b, true);
    graph.insertVertex(c, true);
    graph.insertVertex(d, true);
    graph.insertVertex(e, true);

    // Create edges - forming a graph like:
    //    A --- B --- C
    //    |           |
    //    D --- E ----+
    graph.insertEdge(a, b);
    graph.insertEdge(b, c);
    graph.insertEdge(a, d);
    graph.insertEdge(d, e);
    graph.insertEdge(e, c);

    // Create a list to store hop counts
    List<BfsVertex<std::string>> hops;

    // Run BFS starting from vertex A
    QCOMPARE(bfs(&graph, a, &hops), 0);

    // Verify all vertices were reached
    QCOMPARE(hops.getSize(), 5);

    // Check expected hop counts
    // First, gather all results into an easily testable form
    int hopCounts[5] = {-1, -1, -1, -1, -1};

    ListNode<BfsVertex<std::string>> *node = hops.head();
    while (node != nullptr)
    {
        BfsVertex<std::string> *vertex = node->data();
        if (vertex && vertex->data)
        {
            if (*vertex->data == "A")
                hopCounts[0] = vertex->hops;
            else if (*vertex->data == "B")
                hopCounts[1] = vertex->hops;
            else if (*vertex->data == "C")
                hopCounts[2] = vertex->hops;
            else if (*vertex->data == "D")
                hopCounts[3] = vertex->hops;
            else if (*vertex->data == "E")
                hopCounts[4] = vertex->hops;
        }
        node = node->next();
    }

    // Verify the hop counts
    QCOMPARE(hopCounts[0], 0); // A is the start vertex (0 hops)
    QCOMPARE(hopCounts[1], 1); // B is 1 hop from A
    QCOMPARE(hopCounts[2], 2); // C is 2 hops from A (via B)
    QCOMPARE(hopCounts[3], 1); // D is 1 hop from A
    QCOMPARE(hopCounts[4], 2); // E is 2 hops from A (via D)
}

void TestGraph::testCustomFunctors()
{
    // 1. Test ID-based comparison
    {
        Graph<BfsVertex<ComplexData>, CompareById> idGraph;
        
        // Create vertices with same ID but different other fields
        BfsVertex<ComplexData> *v1 = new BfsVertex<ComplexData>(new ComplexData(1, "First", 10.5));
        BfsVertex<ComplexData> *v2 = new BfsVertex<ComplexData>(new ComplexData(1, "Different", 20.3));
        BfsVertex<ComplexData> *v3 = new BfsVertex<ComplexData>(new ComplexData(2, "Second", 15.2));
        
        // Insert first vertex
        QCOMPARE(idGraph.insertVertex(v1, true), 0);
        
        // Try inserting vertex with same ID - should be considered a duplicate
        QCOMPARE(idGraph.insertVertex(v2), 1); // Should return 1 (error) for duplicate
        
        // Insert vertex with different ID - should succeed
        QCOMPARE(idGraph.insertVertex(v3, true), 0);
        
        // Verify graph contents
        QCOMPARE(idGraph.getVertexCount(), 2);
        
        // Clean up the duplicate that wasn't inserted
        delete v2->data;
        delete v2;
    }
    
    // 2. Test case-insensitive name comparison
    {
        Graph<BfsVertex<ComplexData>, CompareByNameCaseInsensitive> nameGraph;
        
        // Create vertices with same name in different cases
        BfsVertex<ComplexData> *v1 = new BfsVertex<ComplexData>(new ComplexData(1, "test", 10.5));
        BfsVertex<ComplexData> *v2 = new BfsVertex<ComplexData>(new ComplexData(2, "TEST", 20.3)); // Same name, different case
        BfsVertex<ComplexData> *v3 = new BfsVertex<ComplexData>(new ComplexData(3, "different", 15.2));
        
        // Insert first vertex
        QCOMPARE(nameGraph.insertVertex(v1, true), 0);
        
        // Try inserting vertex with same name in different case - should be considered duplicate
        QCOMPARE(nameGraph.insertVertex(v2), 1); // Should return 1 (error) for duplicate
        
        // Insert vertex with different name
        QCOMPARE(nameGraph.insertVertex(v3, true), 0);
        
        // Verify graph contents
        QCOMPARE(nameGraph.getVertexCount(), 2);
        
        // Create a search vertex with mixed case
        BfsVertex<ComplexData> searchVertex(new ComplexData(0, "TeSt", 0.0));
        
        // Test that we can create an edge using the mixed-case name (testing that the comparison works)
        QCOMPARE(nameGraph.insertVertex(&searchVertex, false), 1); // Should detect as duplicate
        
        // Clean up
        delete v2->data;
        delete v2;
        delete searchVertex.data;
    }
    
    // 3. Test weight-based threshold comparison
    {
        // Create a graph with weight threshold of 1.0
        Graph<BfsVertex<ComplexData>, CompareByWeightThreshold> weightGraph;
        
        // Create vertices with weights that are close to each other
        BfsVertex<ComplexData> *v1 = new BfsVertex<ComplexData>(new ComplexData(1, "First", 10.5));
        BfsVertex<ComplexData> *v2 = new BfsVertex<ComplexData>(new ComplexData(2, "Second", 10.9)); // Within threshold of v1
        BfsVertex<ComplexData> *v3 = new BfsVertex<ComplexData>(new ComplexData(3, "Third", 12.0));  // Outside threshold of v1
        
        // Insert first vertex
        QCOMPARE(weightGraph.insertVertex(v1, true), 0);
        
        // Try inserting vertex with weight within threshold - should be considered duplicate
        QCOMPARE(weightGraph.insertVertex(v2), 1);
        
        // Try inserting vertex with weight outside threshold - should succeed
        QCOMPARE(weightGraph.insertVertex(v3, true), 0);
        
        // Verify graph contents
        QCOMPARE(weightGraph.getVertexCount(), 2);
        
        // Verify edges with threshold comparison
        weightGraph.insertEdge(v1, v3);
        
        // Search vertex with weight close to v1 but not exactly the same
        BfsVertex<ComplexData> searchVertex(new ComplexData(0, "", 10.6));
        
        // Should detect as duplicate of v1 because weights are within threshold
        QCOMPARE(weightGraph.insertVertex(&searchVertex, false), 1);
        
        // Clean up
        delete v2->data;
        delete v2;
        delete searchVertex.data;
    }
    
    // 4. Test interoperability with algorithms (BFS with custom comparison)
    {
        Graph<BfsVertex<ComplexData>, CompareById> graph;
        
        // Create vertices
        BfsVertex<ComplexData> *v1 = new BfsVertex<ComplexData>(new ComplexData(1, "Node 1", 10.0));
        BfsVertex<ComplexData> *v2 = new BfsVertex<ComplexData>(new ComplexData(2, "Node 2", 20.0));
        BfsVertex<ComplexData> *v3 = new BfsVertex<ComplexData>(new ComplexData(3, "Node 3", 30.0));
        BfsVertex<ComplexData> *v4 = new BfsVertex<ComplexData>(new ComplexData(4, "Node 4", 40.0));
        BfsVertex<ComplexData> *v5 = new BfsVertex<ComplexData>(new ComplexData(5, "Node 5", 50.0));
        
        // Insert vertices
        graph.insertVertex(v1, true);
        graph.insertVertex(v2, true);
        graph.insertVertex(v3, true);
        graph.insertVertex(v4, true);
        graph.insertVertex(v5, true);
        
        // Create edges
        graph.insertEdge(v1, v2);
        graph.insertEdge(v2, v3);
        graph.insertEdge(v1, v4);
        graph.insertEdge(v4, v5);
        graph.insertEdge(v5, v3);
        
        // Run BFS
        List<BfsVertex<ComplexData>> hops;
        QCOMPARE(bfs(&graph, v1, &hops), 0);
        
        // Verify all vertices were reached
        QCOMPARE(hops.getSize(), 5);
        
        // Verify we can search for vertices by ID
        BfsVertex<ComplexData> searchId(new ComplexData(3, "Different name shouldn't matter", 99.9));
        QVERIFY(graph.insertVertex(&searchId, false) == 1); // Should detect as duplicate
        
        // Clean up
        delete searchId.data;
    }
}

QTEST_APPLESS_MAIN(TestGraph)
#include "tst_graph.moc"
