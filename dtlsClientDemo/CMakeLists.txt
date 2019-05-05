cmake_minimum_required(VERSION 3.1.0)

project(myDtlsClientDemoApp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 5.12 REQUIRED
  COMPONENTS Core Network
)

aux_source_directory(./src my_project_sources)

add_executable(${PROJECT_NAME} ${my_project_sources})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network)
