cmake_minimum_required(VERSION 3.0)

project(square_solver)

find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 -Wall -Wextra -pedantic")
set(CMAKE_CXX_STANDARD 14)

set(BIGNUMBER
    bignumber/bignumber.cpp
    bignumber/bignumber.h
    bignumber/bignumberbuilder.cpp
    bignumber/bignumberbuilder.h
    bignumber/bignumbertypes.h
)

set(SRC
    ${BIGNUMBER}
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
