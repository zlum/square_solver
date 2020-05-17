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
    ../bignumber/numvector.cpp
    ../bignumber/numvector.h
)

set(SRC_TEST
    ${BIGNUMBER_TEST}
)

set(SRC
    ${SRC_TEST}
    main.cpp
    bignumber.cpp
)

add_executable(${PROJECT_NAME}
    ${SRC}
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    Threads::Threads
    gtest
)
