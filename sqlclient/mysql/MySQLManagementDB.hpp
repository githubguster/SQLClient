/**
 * author: guster
 * date: 2021/06/26
 * description: mysql management
 */

#ifndef __MY_SQL_MANAGEMENT__
#define __MY_SQL_MANAGEMENT__

#include <iostream>
#include <string>
#include <cstring>
#include <mysql/mysql.h>
#include <algorithm>
#include <list>
#include <pthread.h>
#include "MySQLClient.hpp"

using namespace std;

namespace ODBC
{
    class MySQLManagementDB : public SQLManagementDB<MySQLClient>
    {
    private:
        const float ERROR_VERSION = -1;
        static bool MUTEX_IS_INIT;
        static pthread_mutex_t MUTEX;
        static bool IS_INIT;
        const string CREATE_TABLE = NAME + " nvarchar(255) BINARY , "  +
                                    VERSION + " float ";
        static list<SQLManagement> MANAGEMENTS;
    protected:
        void init();
    public:
        MySQLManagementDB(string server_ip, uint16_t server_port, string user_name, string user_password, string database_name);
        MySQLManagementDB(MySQLClient *db);
        virtual ~MySQLManagementDB();

        bool check(string name);
        uint64_t add(SQLManagement management);
        uint64_t update(SQLManagement management);
        uint64_t remove(string name);
        SQLManagement get(string name);

        virtual SQLManagement table_info() = 0;
        virtual bool create_table() = 0;
        virtual bool update_table(float version) = 0;
    };
}

#endif