cmake_minimum_required(VERSION 3.16)

project(reliable_udp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(server
    src/server.cpp
)

add_executable(client
    src/client.cpp)

target_link_libraries(server PRIVATE pthread)
target_link_libraries(client PRIVATE pthread)