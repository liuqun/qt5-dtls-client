cmake_minimum_required(VERSION 3.1.0)

project(demo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 5.12 REQUIRED
  COMPONENTS Core Network
)

set(project_sources
  mydtlsclienthandler.cpp
  main.cpp
)

set(project_headers
  mydtlsclienthandler.h
)

add_executable(${PROJECT_NAME} ${project_sources} ${project_headers})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network)
