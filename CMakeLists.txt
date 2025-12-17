cmake_minimum_required(VERSION 4.0)
project(aoc25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS " \
-O0 \
-Wall \
-Wextra \
-Wpedantic \
-Wshadow \
-Werror \
")

find_package(z3 REQUIRED CONFIG)
find_package(nvl)

function(add_day day)
    set(name "day${day}")
    set(file "Day${day}.cpp")
    add_executable(${name} "${name}/${file}")

    target_link_libraries(${name} PRIVATE nvl)

    # TODO: Make conditional
    target_include_directories(${name} PRIVATE ${Z3_CXX_INCLUDE_DIRS})
    target_link_libraries(${name} PRIVATE ${Z3_LIBRARIES})
endfunction()

add_day("01")
add_day("02")
add_day("03")
add_day("04")
add_day("05")
add_day("06")
add_day("07")
add_day("08")
add_day("09")
add_day("10")
