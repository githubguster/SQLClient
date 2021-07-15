#option
option(ENABLE_MYSQL_SQL_CLIENT "Enable MySQL Client" TRUE)
option(ENABLE_POSTGRESQL_SQL_CLIENT "Enable PostgreSQL Client" TRUE)

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
          ${POSTGRESQL_CLIENT_BASE_DIR}/PostgresSQLDataConvert.hpp
          ${POSTGRESQL_CLIENT_BASE_DIR}/PostgreSQLBind.hpp
          ${POSTGRESQL_CLIENT_BASE_DIR}/PostgreSQLResult.hpp
          ${POSTGRESQL_CLIENT_BASE_DIR}/PostgreSQLClient.hpp
          ${POSTGRESQL_CLIENT_BASE_DIR}/PostgreSQLManagementDB.hpp)

     set(POSTGRESQL_CLIENT_C_PLUS_PLUS_SOURCE_FILES
          ${SQL_CLIENT_C_PLUS_PLUS_INCLUDE_FILES}
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