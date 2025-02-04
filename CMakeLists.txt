cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(WebServer VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR})

# Find Boost libraries
find_package(Boost REQUIRED COMPONENTS system filesystem)

# Add the executable
add_executable(WebServer
    Main.cpp
    Server.hpp
    Server.cpp
)

# Link Boost libraries
target_link_libraries(WebServer Boost::system Boost::filesystem)

# Add Boost include directories
target_include_directories(WebServer PRIVATE ${Boost_INCLUDE_DIRS})
