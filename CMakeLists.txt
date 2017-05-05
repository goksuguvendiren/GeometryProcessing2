cmake_minimum_required(VERSION 3.7)
project(GeometryProcessing2)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

set(RTK_ROOT "/Users/goksu/rtk/rtk")
find_package(RTK REQUIRED)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES Mesh.cpp Mesh.h Triangle.cpp Triangle.h Vertex.cpp Vertex.h main.cpp)
add_executable(GeometryProcessing2 ${SOURCE_FILES})

target_link_libraries(GeometryProcessing2 voro++)

target_include_directories(GeometryProcessing2 PUBLIC ${RTK_INCLUDE_DIRS})
target_include_directories(GeometryProcessing2 PUBLIC "/Users/goksu/rtk/3rd_party/gsl/include")
target_link_libraries(GeometryProcessing2 ${RTK_LIBRARIES})
target_compile_definitions(GeometryProcessing2 PUBLIC ${RTK_COMPILE_FLAGS} GLM_ENABLE_EXPERIMENTAL=1)