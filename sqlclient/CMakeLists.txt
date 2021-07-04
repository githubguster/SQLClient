#option
option(ENABLE_MYSQL_SQL_CLIENT "Enable MySQL Client" TRUE)

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
     find_package(MySQL)
     find_package(Threads)

     if(Boost_FOUND AND MYSQL_FOUND AND THREADS_FOUND)
          add_library(MYSQL_CLIENT_LIBRARY SHARED ${MYSQL_CLIENT_C_PLUS_PLUS_INCLUDE_FILES} ${MYSQL_CLIENT_C_PLUS_PLUS_SOURCE_FILES})
          target_include_directories(MYSQL_CLIENT_LIBRARY PUBLIC . PUBLIC ${MYSQL_INCLUDE_DIR})
          target_compile_definitions(MYSQL_CLIENT_LIBRARY PUBLIC -DENABLE_MYSQL)
          target_link_libraries(MYSQL_CLIENT_LIBRARY ${MYSQL_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
     endif()
endif()