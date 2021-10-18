cmake_minimum_required(VERSION 3.17)
project(testrttr)

set(RTTR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rttr/build/install/cmake)
message(${RTTR_DIR})
find_package(RTTR CONFIG REQUIRED Core)

add_executable(testrttr main.cpp source.cpp)

target_link_libraries(testrttr RTTR::Core)
