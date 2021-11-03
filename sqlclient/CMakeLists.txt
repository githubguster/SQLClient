#option
option(ENABLE_MYSQL_SQL_CLIENT "Enable MySQL Client" TRUE)
option(ENABLE_POSTGRESQL_SQL_CLIENT "Enable PostgreSQL Client" TRUE)
option(ENABLE_SQLITE_SQL_CLIENT "Enable SQLite Client" TRUE)
option(ENABLE_MSSQL_SQL_CLIENT "Enable MSSQL Client" FALSE)

#sql client
set(SQL_CLIENT_C_PLUS_PLUS_INCLUDE_FILES
     SQLClient.hpp
     SQLException.hpp)

#mysql client
if(ENABLE_MYSQL_SQL_CLIENT)
     set(MYSQL_CLIENT_BASE_DIR mysql)

     set(MYSQL_CLIENT_C_PLUS_PLUS_INCLUDE_FILES
          ${SQL_CLIENT_C_PLUS_PLUS_INCLUDE_FILES}
          ${MYSQL_CLIENT_BASE_DIR}/MySQLBind.hpp
          ${MYSQL_CLIENT_BASE_DIR}/MySQLResult.hpp
          ${MYSQL_CLIENT_BASE_DIR}/MySQLClient.hpp
          ${MYSQL_CLIENT_BASE_DIR}/MySQLManagementDB.hpp)

     set(MYSQL_CLIENT_C_PLUS_PLUS_SOURCE_FILES     
          ${MYSQL_CLIENT_BASE_DIR}/MySQLClient.cpp
          ${MYSQL_CLIENT_BASE_DIR}/MySQLManagementDB.cpp)


     find_package(Boost)
     find_package(Threads)
     find_package(MySQL)

     if(Boost_FOUND AND THREADS_FOUND AND MYSQL_FOUND)
          add_library(MYSQL_CLIENT_LIBRARY SHARED ${MYSQL_CLIENT_C_PLUS_PLUS_INCLUDE_FILES} ${MYSQL_CLIENT_C_PLUS_PLUS_SOURCE_FILES})
          target_include_directories(MYSQL_CLIENT_LIBRARY PUBLIC . PUBLIC ${MYSQL_INCLUDE_DIR})
          target_compile_definitions(MYSQL_CLIENT_LIBRARY PUBLIC -DENABLE_MYSQL)
          target_link_libraries(MYSQL_CLIENT_LIBRARY ${MYSQL_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
          set_target_properties(MYSQL_CLIENT_LIBRARY PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")
     endif()
endif()

#postsql client
if(ENABLE_POSTGRESQL_SQL_CLIENT)
     set(POSTGRESQL_CLIENT_BASE_DIR postgresql)

     set(POSTGRESQL_CLIENT_C_PLUS_PLUS_INCLUDE_FILES
          ${SQL_CLIENT_C_PLUS_PLUS_INCLUDE_FILES}
          ${POSTGRESQL_CLIENT_BASE_DIR}/PostgreSQLDataConvert.hpp
          ${POSTGRESQL_CLIENT_BASE_DIR}/PostgreSQLBind.hpp
          ${POSTGRESQL_CLIENT_BASE_DIR}/PostgreSQLResult.hpp
          ${POSTGRESQL_CLIENT_BASE_DIR}/PostgreSQLClient.hpp
          ${POSTGRESQL_CLIENT_BASE_DIR}/PostgreSQLManagementDB.hpp)

     set(POSTGRESQL_CLIENT_C_PLUS_PLUS_SOURCE_FILES
          ${POSTGRESQL_CLIENT_BASE_DIR}/PostgreSQLClient.cpp
          ${POSTGRESQL_CLIENT_BASE_DIR}/PostgreSQLManagementDB.cpp)

     find_package(Boost)
     find_package(Threads)
     find_package(PostgreSQL)

     if(Boost_FOUND AND THREADS_FOUND AND PostgreSQL_FOUND)
          add_library(POSTGRESQL_CLIENT_LIBRARY SHARED ${POSTGRESQL_CLIENT_C_PLUS_PLUS_INCLUDE_FILES} ${POSTGRESQL_CLIENT_C_PLUS_PLUS_SOURCE_FILES})
          target_include_directories(POSTGRESQL_CLIENT_LIBRARY PUBLIC . PUBLIC ${PostgreSQL_INCLUDE_DIRS})
          target_compile_definitions(POSTGRESQL_CLIENT_LIBRARY PUBLIC -DENABLE_POSTGRESQL)
          target_link_libraries(POSTGRESQL_CLIENT_LIBRARY ${PostgreSQL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
          set_target_properties(POSTGRESQL_CLIENT_LIBRARY PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")
     endif()
endif()

if(ENABLE_SQLITE_SQL_CLIENT)
     set(SQLITE_CLIENT_BASE_DIR sqlite)

     set(SQLITE_CLIENT_C_PLUS_PLUS_INCLUDE_FILE
          ${SQL_CLIENT_C_PLUS_PLUS_INCLUDE_FILES}
          ${SQLITE_CLIENT_BASE_DIR}/SQLiteSQLDataConvert.hpp
          ${SQLITE_CLIENT_BASE_DIR}/SQLiteSQLBind.hpp
          ${SQLITE_CLIENT_BASE_DIR}/SQLiteSQLBind.hpp
          ${SQLITE_CLIENT_BASE_DIR}/SQLiteSQLResult.hpp
          ${SQLITE_CLIENT_BASE_DIR}/SQLiteSQLClient.hpp
          ${SQLITE_CLIENT_BASE_DIR}/SQLiteSQLManagementDB.hpp)

     set(SQLITE_CLIENT_C_PLUS_PLUS_SOURCE_FILE
          ${SQLITE_CLIENT_BASE_DIR}/SQLiteSQLClient.cpp
          ${SQLITE_CLIENT_BASE_DIR}/SQLiteSQLManagementDB.cpp)

     find_package(Boost)
     find_package(Threads)
     find_package(SQLite3)

     if(Boost_FOUND AND THREADS_FOUND AND SQLite3_FOUND)
          add_library(SQLITE_CLIENT_LIBRARY SHARED ${SQLITE_CLIENT_C_PLUS_PLUS_INCLUDE_FILE} ${SQLITE_CLIENT_C_PLUS_PLUS_SOURCE_FILE})
          target_include_directories(SQLITE_CLIENT_LIBRARY PUBLIC . PUBLIC ${SQLite3_INCLUDE_DIRS})
          target_compile_definitions(SQLITE_CLIENT_LIBRARY PUBLIC -DENABLE_SQLITE)
          target_link_libraries(SQLITE_CLIENT_LIBRARY ${SQLite3_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
          set_target_properties(SQLITE_CLIENT_LIBRARY PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")
     endif()
endif()

if(ENABLE_MSSQL_SQL_CLIENT)
     set(MSSQL_CLIENT_BASE_DIR mssql)

     set(MSSQL_CLIENT_C_PLUS_PLUS_INCLUDE_FILES
          ${SQL_CLIENT_C_PLUS_PLUS_INCLUDE_FILES}
          ${MSSQL_CLIENT_BASE_DIR}/MsSQLBind.hpp
          ${MSSQL_CLIENT_BASE_DIR}/MsSQLResult.hpp
          ${MSSQL_CLIENT_BASE_DIR}/MsSQLClient.hpp)

     set(MSSQL_CLIENT_C_PLUS_PLUS_SOURCE_FILES
          ${MSSQL_CLIENT_BASE_DIR}/MsSQLClient.cpp)

     find_package(Boost)
     find_package(Threads)
     # find_package(MSSQL REQUIRED)
     find_package(ODBC)
     if(Boost_FOUND 
     AND THREADS_FOUND 
     # AND MSSQL_FOUND 
     AND ODBC_FOUND)
          add_library(MSSQL_CLIENT_LIBRARY SHARED ${MSSQL_CLIENT_C_PLUS_PLUS_INCLUDE_FILES} ${MSSQL_CLIENT_C_PLUS_PLUS_SOURCE_FILES})
          target_include_directories(MSSQL_CLIENT_LIBRARY 
          PUBLIC . 
          # PUBLIC ${MSSQL_INCLUDE_DIR} 
          PUBLIC ${ODBC_INCLUDE_DIRS})
          target_compile_definitions(MSSQL_CLIENT_LIBRARY PUBLIC -DENABLE_MSSQL)
          target_link_libraries(MSSQL_CLIENT_LIBRARY 
          # ${MSSQL_LIBRARIES}
          ${ODBC_LIBRARIES} 
          ${CMAKE_THREAD_LIBS_INIT})
          set_target_properties(MSSQL_CLIENT_LIBRARY PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")
     endif()
endif()