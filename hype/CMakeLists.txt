cmake_minimum_required(VERSION 3.27)
project(Hype)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "-O1")

file(GLOB SRC_FILES CONFIGURE_DEPENDS
        "*.cpp"
)

add_library(Hype SHARED ${SRC_FILES})