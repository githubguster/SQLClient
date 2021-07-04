/**
 * author: guster
 * date: 2021/06/26
 * description: mysql client
 * reference: https://github.com/bskari/mysql-cpp
 */

#ifndef __MY_SQL_CLIENT__
#define __MY_SQL_CLIENT__

#include <iostream>
#include <string>
#include <cstring>
#include <mysql/mysql.h>
#include <vector>
#include <tuple>
#include "SQLClient.hpp"
#include "MySQLBind.hpp"
#include "MySQLResult.hpp"

using namespace std;

namespace ODBC
{
    class MySQLClient : public SQLClient<shared_ptr<MySQLResult>, MySQLBind*>
    {
    private:
        MYSQL *mysql;
        bool connected;
    public:
        MySQLClient(string server_ip, uint16_t server_port, string user_name, string user_password, string database_name);
        ~MySQLClient();

        bool open();
        bool open(string source);
        bool close();
        bool isOpen();
        shared_ptr<MySQLResult> read(string sql, PrepareStatement command = nullptr);
        uint64_t query(string sql, PrepareStatement command = nullptr);
        shared_ptr<MySQLResult> select(string table, string columns, string wheres, string order, string group, string having, string limit, PrepareStatement command = nullptr);
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