cmake_minimum_required(VERSION 3.16)
project(OrbitalEncountersLobby CXX)

set(CMAKE_CXX_STANDARD          14)
set(CMAKE_CXX_EXTENSIONS        OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
link_libraries(Boost::boost Threads::Threads)

if(CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
    add_compile_options(/W4)
    add_compile_definitions(_WIN32_WINNT=0x0601)
endif()

add_subdirectory(src)
add_subdirectory(Client)
