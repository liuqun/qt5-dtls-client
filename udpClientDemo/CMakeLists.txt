cmake_minimum_required(VERSION 3.13.5)

project(DemoProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 5.9 REQUIRED
  COMPONENTS Core Network
)

set(my_exe_file ${PROJECT_NAME}.exe)
set(my_src_files "main.cpp" "client.cpp" "client.h")
add_executable(${my_exe_file} ${my_src_files})

target_link_libraries(${my_exe_file} Qt5::Core Qt5::Network)
