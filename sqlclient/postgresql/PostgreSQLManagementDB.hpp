/**
 * author: guster
 * date: 2021/07/07
 * description: postgresql management
 */

#ifndef __POSTGRE_SQL_MANAGEMENT__
#define __POSTGRE_SQL_MANAGEMENT__

#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <list>
#include <pthread.h>
#include <postgresql/libpq-fe.h>
#include "PostgreSQLClient.hpp"

using namespace std;

namespace ODBC
{
    class PostgreManagementDB : public SQLManagementDB<PostgreSQLClient>
    {
    private:
        const float ERROR_VERSION = -1;
        static bool MUTEX_IS_INIT;
        static pthread_mutex_t MUTEX;
        static bool IS_INIT;
        const string CREATE_TABLE = NAME + " varchar(255) , "  +
                                    VERSION + " float4 ";
        static list<SQLManagement> MANAGEMENTS;
    protected:
        void init();
        bool check(string name);
        uint64_t add(SQLManagement management);
        uint64_t update(SQLManagement management);
        uint64_t remove(string name);
        SQLManagement get(string name);
    public:
        PostgreManagementDB(string server_ip, uint16_t server_port, string user_name, string user_password, string database_name, SQLError error = nullptr);
        PostgreManagementDB(PostgreSQLClient *db);
        virtual ~PostgreManagementDB();

        virtual SQLManagement table_info() = 0;
        virtual bool create_table() = 0;
        virtual bool update_table(float version) = 0;
    };
}

#endif