cmake_minimum_required(VERSION 3.5)
project("chrono_date_playground")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(CURL)
if(CURL_FOUND)
  message(STATUS "Curl libraries found at: ${CURL_LIBRARIES}")
  message(STATUS "Curl includes found at: ${CURL_INCLUDE_DIRS}")
else()
  message(SEND_ERROR "Could not find cURL on your system")
endif(CURL_FOUND)

include_directories(SYSTEM "${CURL_INCLUDE_DIRS}")
include_directories(SYSTEM "../Catch/single_include")
include_directories(SYSTEM "../date")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(chrono_date_playground
  main.cpp
  ../date/tz.cpp)

set_property(TARGET chrono_date_playground PROPERTY CXX_STANDARD 14)
set_property(TARGET chrono_date_playground PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(chrono_date_playground
    ${CURL_LIBRARIES})
