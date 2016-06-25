cmake_minimum_required(VERSION 3.5)
project("date_playground")

include_directories(SYSTEM "/opt/local/include")
include_directories(SYSTEM "../date")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(date_playground
  main.cpp)
set_property(TARGET date_playground PROPERTY CXX_STANDARD 14)
set_property(TARGET date_playground PROPERTY CXX_STANDARD_REQUIRED ON)
