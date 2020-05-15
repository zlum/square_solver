cmake_minimum_required(VERSION 3.0)

project(square_solver)

find_package(Threads REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

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

if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC
        $<$<CONFIG:RELEASE>: /Wall /O2>
)

    target_compile_options(${PROJECT_NAME} PUBLIC
        $<$<CONFIG:DEBUG>: /DEBUG:FULL /Zi /Zo>
)
else()
    target_compile_options(${PROJECT_NAME} PUBLIC
        $<$<CONFIG:RELEASE>: -O3>
)

    target_compile_options(${PROJECT_NAME} PUBLIC
        $<$<CONFIG:DEBUG>: -Wall -Wextra -pedantic>
)
endif()

#set(GCC_COVERAGE_COMPILE_FLAGS "-fsanitize=address -fsanitize=leak -g")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
