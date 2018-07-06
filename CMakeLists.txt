set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(cpp_containers LANGUAGES CXX)

include(Sanitizers.cmake)

add_executable(test_sv_set app/test_sv_set.cpp include/ra/sv_set.hpp)
add_executable(test_intrusive_list app/test_intrusive_list.cpp include/ra/intrusive_list.hpp)

target_include_directories(test_sv_set PUBLIC include "${CMAKE_CURRENT_BINARY_DIR}/include")
target_include_directories(test_intrusive_list PUBLIC include "${CMAKE_CURRENT_BINARY_DIR}/include")




