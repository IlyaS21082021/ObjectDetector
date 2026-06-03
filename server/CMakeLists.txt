project(testObjDetectorServer)
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_executable(testObjDetectorServer main.cpp 
                              service.cpp 
                              nnengine.cpp 
                              utils.cpp
                              ../common/poly.cpp
                              ../grpc/ObjectFinder.grpc.pb.cc
                              ../grpc/ObjectFinder.pb.cc
                              )

target_include_directories(testObjDetectorServer PRIVATE /usr/include/onnx)
target_include_directories(testObjDetectorServer PRIVATE /usr/include/opencv4)
target_include_directories(testObjDetectorServer PRIVATE ${CMAKE_SOURCE_DIR}/../common)
target_include_directories(testObjDetectorServer PRIVATE ${CMAKE_SOURCE_DIR}/../grpc)


find_package(OpenCV REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
target_link_libraries(testObjDetectorServer ${OpenCV_LIBS})
target_link_libraries(testObjDetectorServer  /usr/lib/onnx/libonnxruntime.so)
target_link_libraries(testObjDetectorServer gRPC::grpc++)
target_link_libraries(testObjDetectorServer protobuf::libprotobuf)

add_custom_command(TARGET testObjDetectorServer POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:testObjDetectorServer> ${CMAKE_SOURCE_DIR})
