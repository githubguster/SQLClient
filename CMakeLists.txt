cmake_minimum_required(VERSION 3.0)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

project(sqlclienttest)

add_subdirectory(sqlclient)

set(PROJECT_NAME_INCLUDE_FILES 
    test/UserDB.hpp)
set(PROJECT_NAME_SOURCE_FILES 
    test/UserDB.cpp
    test/main.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_NAME_INCLUDE_FILES} ${PROJECT_NAME_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC sqlclient)
target_link_libraries(${PROJECT_NAME} MYSQL_CLIENT_LIBRARY)