cmake_minimum_required(VERSION 3.5)
project("chrono_date_playground")

include_directories(SYSTEM "../Catch/single_include")
include_directories(SYSTEM "../date")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(chrono_date_playground
  main.cpp)

macro(add_compiler_flags)
  foreach(flag ${ARGV})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
  endforeach()
endmacro()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compiler_flags(-Werror)
  add_compiler_flags(-pedantic)
  add_compiler_flags(-pedantic-errors)
  add_compiler_flags(-fvisibility=hidden)
  add_compiler_flags(-fstrict-aliasing)
  add_compiler_flags(-Weverything)
  add_compiler_flags(-Wno-c++98-compat)
  add_compiler_flags(-Wno-exit-time-destructors)
endif()

set_property(TARGET chrono_date_playground PROPERTY CXX_STANDARD 14)
set_property(TARGET chrono_date_playground PROPERTY CXX_STANDARD_REQUIRED ON)
