cmake_minimum_required(VERSION 3.15)
project(cs499_raymondjune)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
find_package(glog 0.4.0 REQUIRED)
find_package(gflags REQUIRED)
set(CMAKE_LIBRARY_PATH "${CMAKE_LIBRARY_PATH}:/usr/local/lib")

# find protobuf
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf REQUIRED)
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
find_program(_PROTOBUF_PROTOC protoc)

# find grpc
find_package(PkgConfig REQUIRED)
pkg_search_module(GRPC REQUIRED grpc)
pkg_search_module(GRPCPP REQUIRED grpc++)
list(APPEND GRPCPP_INCLUDE_DIRS ${GRPC_INCLUDE_DIRS})
list(APPEND GRPCPP_LIBRARIES ${GRPC_LIBRARIES})

find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)

foreach(_proto func kvstore warble)
    # Proto file
    get_filename_component(${_proto}_proto "./protos/${_proto}.proto" ABSOLUTE)
    get_filename_component(${_proto}_proto_path "${${_proto}_proto}" PATH)
    # Generated sources
    set(${_proto}_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/${_proto}.pb.cc")
    set(${_proto}_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${_proto}.pb.h")
    set(${_proto}_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/${_proto}.grpc.pb.cc")
    set(${_proto}_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${_proto}.grpc.pb.h")
    add_custom_command(
            OUTPUT "${${_proto}_proto_srcs}" "${${_proto}_proto_hdrs}" "${${_proto}_grpc_srcs}" "${${_proto}_grpc_hdrs}"
            COMMAND ${_PROTOBUF_PROTOC}
            ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
            --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
            -I "${${_proto}_proto_path}"
            --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
            "${${_proto}_proto}"
            DEPENDS "${${_proto}_proto}")
endforeach()

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_executable(kvstore_server kvstore_server.cpp ${kvstore_proto_srcs} ${kvstore_grpc_srcs} kvstore.cpp kvstore.h)
add_executable(func func.cpp ${kvstore_proto_srcs} ${kvstore_grpc_srcs} kvstore_client.cpp kvstore_client.h)

foreach(_target kvstore_server func)
    target_include_directories(${_target}
            PRIVATE
            ${GTEST_INCLUDE_DIRS}
            ${GRPCPP_INCLUDE_DIRS})
    target_link_libraries(${_target}
            PRIVATE
            GTest::GTest
            Threads::Threads
            glog::glog
            gflags
            ${_PROTOBUF_LIBPROTOBUF}
            ${GRPCPP_LIBRARIES}
            )
    target_link_options(${_target}
            PRIVATE
            -L/usr/local/lib
            )
endforeach()