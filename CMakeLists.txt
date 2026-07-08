cmake_minimum_required(VERSION 3.16)
project(client1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
find_package(OpenCV REQUIRED)
find_package(GTest REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(CLIENT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/video_receiver_thread.cpp
    src/rest_client.cpp
)

set(CLIENT_HEADERS
    include/mainwindow.h
    include/video_source.h
    include/video_receiver_thread.h
    include/rest_client.h
)

add_executable(client1
    ${CLIENT_SOURCES}
    ${CLIENT_HEADERS}
)

target_link_libraries(client1 PRIVATE Qt6::Widgets Qt6::Network ${OpenCV_LIBS})

add_executable(client1_tests
    tests/test_mainwindow.cpp
    src/mainwindow.cpp
    src/video_receiver_thread.cpp
    src/rest_client.cpp
    tests/mock_http_server.h
    tests/mock_video_source.h
    ${CLIENT_HEADERS}
)

target_link_libraries(client1_tests PRIVATE Qt6::Widgets Qt6::Network ${OpenCV_LIBS} GTest::gtest_main)

include(CTest)
add_test(NAME client1_tests COMMAND client1_tests)
