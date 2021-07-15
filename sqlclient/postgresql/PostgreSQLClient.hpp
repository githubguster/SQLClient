/**
 * author: guster
 * date: 2021/07/07
 * description: postgresql client
 * reference: https://zetcode.com/db/postgresqlc/
 */

#ifndef __POSTGRE_SQL_CLIENT__
#define __POSTGRE_SQL_CLIENT__

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <tuple>
#include <netinet/in.h>
#include <postgresql/libpq-fe.h>
#include "SQLClient.hpp"
#include "PostgresSQLDataConvert.hpp"
#include "PostgreSQLBind.hpp"
#include "PostgreSQLResult.hpp"

using namespace std;

namespace ODBC
{
    class PostgreSQLClient : public SQLClient<shared_ptr<PostgreSQLResult>, PostgreSQLBind*>
    {
    private:
        PGconn *postgresql;
    public:
        PostgreSQLClient(string server_ip, uint16_t server_port, string user_name, string user_password, string database_name, SQLError error = nullptr);
        ~PostgreSQLClient();

        bool open();
        bool open(string source);
        bool close();
        bool isOpen();
        shared_ptr<PostgreSQLResult> read(string sql, PrepareStatement command = nullptr);
        uint64_t query(string sql, PrepareStatement command = nullptr);
        shared_ptr<PostgreSQLResult> select(string table, string columns, string wheres, string order, string group, string having, string limit, PrepareStatement command = nullptr);
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