cmake_minimum_required (VERSION 2.8)
project (main)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wl,--no-as-needed -g -fPIC -m64 -fpermissive")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ffast-math -Wno-unused-result -Wno-format")

file(GLOB CPP_SOURCES  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
    "src/*.cpp"
    )

add_executable(main ${CPP_SOURCES})
target_link_libraries( main
    )
