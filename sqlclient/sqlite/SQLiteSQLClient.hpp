/**
 * author: guster
 * date: 2021/11/02
 * description: sqlite client
 */

#ifndef __SQLITE_SQL_CLIENT__
#define __SQLITE_SQL_CLIENT__

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <tuple>
#include <netinet/in.h>
#include <sqlite3.h>
#include "SQLClient.hpp"
#include "SQLiteSQLDataConvert.hpp"
#include "SQLiteSQLBind.hpp"
#include "SQLiteSQLResult.hpp"

using namespace std;

namespace ODBC
{
    class SQLiteSQLClient : public SQLClient<shared_ptr<SQLiteSQLResult>, SQLiteSQLBind*>
    {
    private:
        sqlite3 *sqlitesql;
    public:
        SQLiteSQLClient(string database_name, SQLError error = nullptr);
        ~SQLiteSQLClient();

        bool open();
        bool open(string source);
        bool close();
        bool isOpen();
        shared_ptr<SQLiteSQLResult> read(string sql, PrepareStatement command = nullptr);
        uint64_t query(string sql, PrepareStatement command = nullptr);
        shared_ptr<SQLiteSQLResult> select(string table, string columns, string wheres, string order, string group, string having, string limit, PrepareStatement command = nullptr);
        uint64_t update(string table, string values, string wheres, PrepareStatement command = nullptr);
        uint64_t insert(string table, string columns, string values, PrepareStatement command = nullptr);
        uint64_t remove(string table, string wheres, PrepareStatement command = nullptr);
        uint64_t create_table(string table, string items, PrepareStatement command = nullptr);
        uint64_t drop_table(string table, PrepareStatement command = nullptr);
        uint64_t add_table_column(string table, string column, string type, PrepareStatement command = nullptr);
        uint64_t alter_table_column(string table, string column, string type, PrepareStatement command = nullptr);
        uint64_t drop_table_column(string table, string column, PrepareStatement command = nullptr);

        string escape_string(string value);
    };
}

#endif