cmake_minimum_required(VERSION 2.8)
project(Selene)

set(LUA_PC_CFG "lua;lua5.3;" CACHE STRING "pkg-config Lua configuration file (or files, separated by ;)")

find_package(PkgConfig)
if(PKG_CONFIG_EXECUTABLE)
  pkg_search_module(LUA REQUIRED ${LUA_PC_CFG})
else()
  if(NOT LUA_INCLUDE_DIR)
    message(SEND_ERROR "LUA_INCLUDE_DIR is required if no PkgConfig is available!")
  endif()
  if(NOT LUA_LIBRARIES)
    message(SEND_ERROR "LUA_LIBRARIES is required if no PkgConfig is available!")
  endif()
endif()

if(${LUA_INCLUDE_DIR})
  if(NOT EXISTS ${LUA_INCLUDE_DIR}/lua.h)
    message(SEND_ERROR "Can't find lua.h in ${LUA_INCLUDE_DIR}")
  endif()
endif()

include_directories(${LUA_INCLUDE_DIR})

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14 -stdlib=libc++")
elseif(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14")
elseif(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14")
endif(APPLE)

include_directories(luatinkere tests)

aux_source_directory(luatinkere cur_src)
aux_source_directory(tests cur_src)



add_executable(test_runner ${cur_src})
target_link_libraries(test_runner ${LUA_LIBRARIES})