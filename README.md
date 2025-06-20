# Algorithm Visualizer

A Qt-based application that provides interactive visualizations of common data structures and algorithms. This educational tool helps users understand how data is stored, accessed, and manipulated in various data structures including linked lists, sets, and graphs.

## Features

- **Visual Data Structure Representation**: See how data is organized in:
  - Linked Lists
  - Sets
  - Graphs
  
- **Interactive Operations**: Watch in real-time as data structures change during:
  - Insertion
  - Removal
  - Search
  - Traversal
  
- **Memory Management Visualization**: Learn about ownership models and memory allocation/deallocation

- **Educational Interface**: Clear visuals with step-by-step animations to facilitate learning

## Project Structure

The project is organized as follows:

- **Base Components**:
  - `Collection`: Base class for data structures with shared functionality
  - `Node`: Base representation of data nodes

- **Data Structures**:
  - `List`: Implementation of a linked list
  - `Set`: Implementation of a set data structure
  - `Graph`: Implementation of a graph data structure

- **Memory Management**:
  - `OwnedDataNode`: Tracking node for memory ownership

## Building the Project

### Prerequisites

- CMake 3.5 or higher
- Qt 6.x (Qt 5.x may work but is not officially supported)
- C++17 compatible compiler (gcc-11/g++-11 recommended)

### Build Steps

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/AlgorithmVisualizer.git
   cd AlgorithmVisualizer
   ```

2. Create and enter the build directory:
   ```bash
   mkdir -p build
   cd build
   ```

3. Configure and build:
   ```bash
   cmake ..
   cmake --build .
   ```

4. Run the application:
   ```bash
   ./AlgorithmVisualizer
   ```

### Running Tests

The project includes unit tests for data structures and UI components:

```bash
cd build
ctest
# Or run individual tests
./tests/tst_list
./tests/tst_mainwindow
```

## Learning Objectives

This visualizer aims to help users:

1. Understand fundamental data structures and algorithms
2. Learn about memory management in C++
3. Visualize abstract concepts to reinforce understanding
4. Explore different implementation strategies for common data structures

## License

[Specify your license here]

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## Contact

Project Link: [https://github.com/isaac76/AlgorithmVisualizer](https://github.com/isaac76/AlgorithmVisualizer)
