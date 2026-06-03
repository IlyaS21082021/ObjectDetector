cmake_minimum_required(VERSION 3.14)
project(client_tests)

set(CMAKE_CXX_STANDARD 17)

find_package(GTest REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(OpenCV REQUIRED)
find_package(gRPC REQUIRED)

add_executable(client_tests client_tests.cpp 
                            ${CMAKE_SOURCE_DIR}/../aux.cpp
                            ${CMAKE_SOURCE_DIR}/../../common/poly.cpp
                            ${CMAKE_SOURCE_DIR}/../../grpc/ObjectFinder.grpc.pb.cc
                            ${CMAKE_SOURCE_DIR}/../../grpc/ObjectFinder.pb.cc
                            )

target_include_directories(client_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/../../grpc
    ${CMAKE_SOURCE_DIR}/../../common
)

target_link_libraries(client_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    yaml-cpp
    ${OpenCV_LIBS}
)
target_link_libraries(client_tests PRIVATE gRPC::grpc++)
target_link_libraries(client_tests PRIVATE protobuf::libprotobuf)

include(GoogleTest)
gtest_discover_tests(client_tests)
