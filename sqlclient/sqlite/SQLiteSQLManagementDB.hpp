/**
 * author: guster
 * date: 2021/11/02
 * description: sqlite management
 */

#ifndef __SQLITE_SQL_MANAGEMENT__
#define __SQLITE_SQL_MANAGEMENT__

#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <list>
#include <pthread.h>
#include <sqlite3.h>
#include "SQLiteSQLClient.hpp"

using namespace std;

namespace ODBC
{
    class SQLiteSQLManagementDB : public SQLManagementDB<SQLiteSQLClient>
    {
    private:
        const float ERROR_VERSION = -1;
        static bool MUTEX_IS_INIT;
        static pthread_mutex_t MUTEX;
        static bool IS_INIT;
        const string CREATE_TABLE = NAME + " varchar(255) , "  +
                                    VERSION + " float ";
        static list<SQLManagement> MANAGEMENTS;
    protected:
        void init();
        bool check(string name);
        uint64_t add(SQLManagement management);
        uint64_t update(SQLManagement management);
        uint64_t remove(string name);
        SQLManagement get(string name);
    public:
        SQLiteSQLManagementDB(string database_name, SQLError error = nullptr);
        SQLiteSQLManagementDB(SQLiteSQLClient *db);
        virtual ~SQLiteSQLManagementDB();

        virtual SQLManagement table_info() = 0;
        virtual bool create_table() = 0;
        virtual bool update_table(float version) = 0;
    };
}

#endif