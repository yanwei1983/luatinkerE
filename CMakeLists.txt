cmake_minimum_required(VERSION 2.8)
project(luatinkerE)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")

include(CheckCXXCompilerFlag)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")


include_directories(lua/src luatinkere tests)

aux_source_directory(luatinkere cur_src)
aux_source_directory(tests cur_src)


link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lua/src)

add_executable(test_runner ${cur_src})
target_link_libraries(test_runner liblua.a dl)
