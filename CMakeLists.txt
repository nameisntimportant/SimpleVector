set(CMAKE_MIN_VERSION 3.5)
cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})

project(vector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} main.cpp vector.h utils.h utils.cpp testRunner.h)

if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Werror -Wextra -O2

        -pedantic -std=c++17

        -Wsign-conversion -Wsign-promo -Wfloat-equal -Wenum-compare
        -Wshadow -Wformat=2 -Wconversion -Wshift-overflow=2 -Wduplicated-cond -Wlogical-op
        -Wcast-qual -Wcast-align -Wold-style-cast -Wredundant-decls
        -Woverloaded-virtual -Wctor-dtor-privacy -Wnon-virtual-dtor

        -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2

        -fsanitize=address -fsanitize=undefined -fno-sanitize-recover
        -fstack-protector
    )
target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message("building under MSVC wasn't tested")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX /fsanitize=address)
endif()

