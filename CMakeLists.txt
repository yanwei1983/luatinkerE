cmake_minimum_required(VERSION 2.8)
project(luatinkerE)





if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g --std=c++14 --stdlib=libc++")
elseif(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g --std=c++14")
elseif(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g --std=c++14")
endif(APPLE)

include_directories(lua luatinkere tests)

aux_source_directory(luatinkere cur_src)
aux_source_directory(tests cur_src)


link_directories(lua/src)

add_executable(test_runner ${cur_src})
target_link_libraries(test_runner liblua.a)