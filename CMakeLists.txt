cmake_minimum_required(VERSION 3.15)
project(InventarioPOS CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

file(GLOB SOURCES "src/*.cpp")

add_executable(inventario_pos ${SOURCES})
target_include_directories(inventario_pos PRIVATE include)
