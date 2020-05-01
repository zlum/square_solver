cmake_minimum_required(VERSION 3.0)

project(square_solver)

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)

set(SRC
    buffer.hpp
    main.cpp
    producerconsumer.cpp
    producerconsumer.h
    quadprinter.cpp
    quadprinter.h
    quadreader.cpp
    quadreader.h
    quadsolver.cpp
    quadsolver.h
    task.h
)

add_executable(${PROJECT_NAME}
    ${SRC}
)

target_link_libraries(${PROJECT_NAME}
    Threads::Threads
)
