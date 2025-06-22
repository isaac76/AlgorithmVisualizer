cmake_minimum_required(VERSION 3.5)

# Enable Qt features
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Ensure C++17 standard is used
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER gcc-11)
set(CMAKE_CXX_COMPILER g++-11)

# List class tests
set(TEST_LIST_SOURCES
    tst_list.cpp
)

# Set class tests
set(TEST_SET_SOURCES
    tst_set.cpp
)

# Graph class tests
set(TEST_GRAPH_SOURCES
    tst_graph.cpp
)

# MainWindow tests
set(TEST_MAINWINDOW_SOURCES
    tst_mainwindow.cpp
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Widgets)

# List test
add_executable(tst_list ${TEST_LIST_SOURCES})
target_link_libraries(tst_list PRIVATE Qt${QT_VERSION_MAJOR}::Test)
add_test(NAME tst_list COMMAND tst_list)

# Set test
add_executable(tst_set ${TEST_SET_SOURCES})
target_link_libraries(tst_set PRIVATE Qt${QT_VERSION_MAJOR}::Test)
add_test(NAME tst_set COMMAND tst_set)

# Graph test
add_executable(tst_graph ${TEST_GRAPH_SOURCES})
target_link_libraries(tst_graph PRIVATE Qt${QT_VERSION_MAJOR}::Test)
add_test(NAME tst_graph COMMAND tst_graph)

# First, create a library from the main project source files
# so we can link against it in our tests
add_library(algorithm_visualizer_lib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../circle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../shape.cpp
)
target_link_libraries(algorithm_visualizer_lib PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
)
target_include_directories(algorithm_visualizer_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
set_target_properties(algorithm_visualizer_lib PROPERTIES AUTOMOC ON)

# MainWindow test
add_executable(tst_mainwindow ${TEST_MAINWINDOW_SOURCES})
target_link_libraries(tst_mainwindow PRIVATE 
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Widgets
    algorithm_visualizer_lib  # Link against our library
)
# Include the parent directory to find mainwindow.h
target_include_directories(tst_mainwindow PRIVATE ${CMAKE_SOURCE_DIR})
add_test(NAME tst_mainwindow COMMAND tst_mainwindow)
