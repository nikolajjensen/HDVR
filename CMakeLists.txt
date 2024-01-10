cmake_minimum_required(VERSION 3.27)
project(HDVR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O1")

file(GLOB SRC_FILES CONFIGURE_DEPENDS
        "src/*.h"
        "src/*.cpp"
)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(src)

add_subdirectory(hype)

add_executable(HDVR main.cpp ${SRC_FILES})
target_link_libraries(HDVR PRIVATE Hype)
