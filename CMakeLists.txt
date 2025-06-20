cmake_minimum_required(VERSION 3.5)

# Setup versioning using semantic versioning (major.minor.patch)
project(AlgorithmVisualizer VERSION 0.1.0 LANGUAGES CXX)

# Extract version components for use in the code
set(PROJECT_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER gcc-11)
set(CMAKE_CXX_COMPILER g++-11)

# Generate build date
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")

# Get Git commit hash
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# Default value if Git commit not available
if("${GIT_COMMIT}" STREQUAL "")
    set(GIT_COMMIT "unknown")
endif()

# Configure version.h
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

# Standard C++17 settings and compiler selection are now set above

# Enable testing
include(CTest)
enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(AlgorithmVisualizer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        graph.h graph.cpp
        collection.h
        owneddatanode.h owneddatanode.cpp
        list.h list.cpp
        node.h node.cpp
        listnode.h listnode.cpp
        set.h set.cpp
        setnode.h setnode.cpp
        adjacentlist.h adjacentlist.cpp
        bfsvertex.h bfsvertex.cpp
        queue.h queue.cpp
        bfs.h bfs.cpp
    )
    
    # Include generated version.h
    target_include_directories(AlgorithmVisualizer PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET AlgorithmVisualizer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(AlgorithmVisualizer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(AlgorithmVisualizer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(AlgorithmVisualizer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.AlgorithmVisualizer)
endif()
set_target_properties(AlgorithmVisualizer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS AlgorithmVisualizer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(AlgorithmVisualizer)
endif()

# Include the tests directory
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
