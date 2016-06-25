cmake_minimum_required(VERSION 3.5)
project("chrono_date_playground")

include_directories(SYSTEM "../Catch/single_include")
include_directories(SYSTEM "../date")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(chrono_date_playground
  main.cpp)
set_property(TARGET chrono_date_playground PROPERTY CXX_STANDARD 14)
set_property(TARGET chrono_date_playground PROPERTY CXX_STANDARD_REQUIRED ON)
