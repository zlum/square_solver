cmake_minimum_required(VERSION 3.0)

project(square_solver_test)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 -Wall -Wextra -pedantic")
set(CMAKE_CXX_STANDARD 14)

include_directories(../.)

set(BIGNUMBER_TEST
    ../bignumber/bignumber.cpp
    ../bignumber/bignumber.h
    ../bignumber/bignumberbuilder.cpp
    ../bignumber/bignumberbuilder.h
    ../bignumber/bignumbertypes.h
)

set(SRC_TEST
    ${BIGNUMBER_TEST}
    ../buffer.hpp
    ../producerconsumer.cpp
    ../producerconsumer.h
    ../quadprinter.cpp
    ../quadprinter.h
    ../quadreader.cpp
    ../quadreader.h
    ../quadsolver.cpp
    ../quadsolver.h
    ../task.h
)

set(SRC
    ${SRC_TEST}
    main.cpp
    test.cpp
)

add_executable(${PROJECT_NAME}
    ${SRC}
)

target_link_libraries(${PROJECT_NAME}
    Threads::Threads
    gtest
)
