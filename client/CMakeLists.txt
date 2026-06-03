project(testObjDetectorClient)
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_executable(testObjDetectorClient main.cpp aux.cpp
                              ${CMAKE_SOURCE_DIR}/../common/poly.cpp
                              ${CMAKE_SOURCE_DIR}/../grpc/ObjectFinder.grpc.pb.cc
                              ${CMAKE_SOURCE_DIR}/../grpc/ObjectFinder.pb.cc
                              )

target_include_directories(testObjDetectorClient PRIVATE ${CMAKE_SOURCE_DIR}/../common)
target_include_directories(testObjDetectorClient PRIVATE ${CMAKE_SOURCE_DIR}/../grpc)

find_package(OpenCV REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
target_link_libraries(testObjDetectorClient ${OpenCV_LIBS})
target_link_libraries(testObjDetectorClient yaml-cpp)
target_link_libraries(testObjDetectorClient gRPC::grpc++)
target_link_libraries(testObjDetectorClient protobuf::libprotobuf)

add_custom_command(TARGET testObjDetectorClient POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:testObjDetectorClient> ${CMAKE_SOURCE_DIR})
