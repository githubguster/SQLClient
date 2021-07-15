cmake_minimum_required(VERSION 3.0)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

project(sqlclienttest)

add_subdirectory(sqlclient)

set(PROJECT_NAME_INCLUDE_FILES 
    test/MySQLUserDB.hpp
    test/PostgreSQLUserDB.hpp)
set(PROJECT_NAME_SOURCE_FILES 
    test/MySQLUserDB.cpp
    test/PostgreSQLUserDB.cpp
    test/main.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_NAME_INCLUDE_FILES} ${PROJECT_NAME_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC sqlclient)

if(ENABLE_MYSQL_SQL_CLIENT)
    target_link_libraries(${PROJECT_NAME} MYSQL_CLIENT_LIBRARY)
endif()

if(ENABLE_POSTGRESQL_SQL_CLIENT)
    target_link_libraries(${PROJECT_NAME} POSTGRESQL_CLIENT_LIBRARY)
endif()
