
cmake_minimum_required(VERSION 3.14)
project(server_tests)

set(CMAKE_CXX_STANDARD 20)

find_package(GTest REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(OpenCV REQUIRED)
find_package(gRPC REQUIRED)

add_executable(server_tests server_tests.cpp 
                            ${CMAKE_SOURCE_DIR}/..
                            ${CMAKE_SOURCE_DIR}/../../common/poly.cpp
                            ${CMAKE_SOURCE_DIR}/../../grpc/ObjectFinder.grpc.pb.cc
                            ${CMAKE_SOURCE_DIR}/../../grpc/ObjectFinder.pb.cc
                            ${CMAKE_SOURCE_DIR}/../nnengine.cpp
                            ${CMAKE_SOURCE_DIR}/../utils.cpp
                            )

target_include_directories(server_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/../../grpc
    ${CMAKE_SOURCE_DIR}/../../common
    /usr/include/onnx
)

target_link_libraries(server_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    yaml-cpp
    ${OpenCV_LIBS}
    gRPC::grpc++
    protobuf::libprotobuf
    /usr/lib/onnx/libonnxruntime.so
)

include(GoogleTest)
gtest_discover_tests(server_tests)
