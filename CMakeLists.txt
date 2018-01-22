cmake_minimum_required(VERSION 3.8)
project(EnCo___Busqueda)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES main.cpp Distance_Matrix.cpp Distance_Matrix.h Output_Manager.cpp Output_Manager.h Solution_vector.cpp Solution_vector.h)
add_executable(EnCo___Busqueda ${SOURCE_FILES})