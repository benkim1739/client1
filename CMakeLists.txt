cmake_minimum_required(VERSION 3.16)
project(client1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(GTest REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(client1
    src/main.cpp
    src/mainwindow.cpp
    include/mainwindow.h
)

target_link_libraries(client1 PRIVATE Qt6::Widgets)

add_executable(client1_tests
    tests/test_mainwindow.cpp
    src/mainwindow.cpp
    include/mainwindow.h
)

target_link_libraries(client1_tests PRIVATE Qt6::Widgets GTest::gtest_main)

include(CTest)
add_test(NAME client1_tests COMMAND client1_tests)
