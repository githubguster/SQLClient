#include "SQLiteSQLClient.hpp"

using namespace ODBC;

SQLiteSQLClient::SQLiteSQLClient(string database_name, SQLError error)
                :SQLClient("", 0, "", "", database_name, error)
{
    this->open();
}

SQLiteSQLClient::~SQLiteSQLClient()
{
    this->close();
}


bool SQLiteSQLClient::open()
{
    return this->open("");
}

bool SQLiteSQLClient::open(string source)
{
    bool ret = true;
    this->close();
    sqlite3_open(this->database_name.c_str(), &this->sqlitesql);
    if(!this->isOpen())
    {
        if(this->error != nullptr)
        {
            this->error(SQLException(sqlite3_errmsg(this->sqlitesql)));
        }
        ret = false;
    }
    return ret;
}

bool SQLiteSQLClient::close()
{
    bool ret = true;
    if(this->isOpen())
    {
        sqlite3_close(this->sqlitesql);
    }
    return ret;
}

bool SQLiteSQLClient::isOpen()
{
    return this->sqlitesql != nullptr;
}

shared_ptr<SQLiteSQLResult> SQLiteSQLClient::read(string sql, PrepareStatement command)
{
    shared_ptr<SQLiteSQLResult> ret = nullptr;
    if(this->isOpen())
    {
        string error = "";
        sqlite3_stmt *stmt = nullptr;
        sqlite3_prepare_v2(this->sqlitesql, sql.c_str(), -1, &stmt, 0);
        shared_ptr<SQLiteSQLResult> result = shared_ptr<SQLiteSQLResult>(new SQLiteSQLResult(stmt, true));
        SQLiteSQLBind *bind = new SQLiteSQLBind(result->getSTMT(), false);
        if(bind->getSTMT() != nullptr)
        {
            if(command != nullptr)
            {
                bind->bindsResize();
                command(bind);
            }
            ret = result;
        }
        else
        {
            error = sqlite3_errmsg(this->sqlitesql);
        }

        if(!error.empty())
        {
            if(this->error != nullptr)
            {
                this->error(SQLException(error));
            }
        }
        delete bind;
    }
    return ret;
}

uint64_t SQLiteSQLClient::query(string sql, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        sqlite3_stmt *stmt = nullptr;
        sqlite3_prepare_v2(this->sqlitesql, sql.c_str(), -1, &stmt, 0);
        SQLiteSQLBind *bind = new SQLiteSQLBind(stmt, true);
        if(bind->getSTMT() != nullptr)
        {
            string error = "";
            if(command != nullptr)
            {
                bind->bindsResize();
                command(bind);
            }

            if(sqlite3_step(bind->getSTMT()) == SQLITE_DONE)
            {
                ret = sqlite3_changes(this->sqlitesql);
                if(ret == 0)
                {
                    ret = 1;
                }
            }
            else
            { 
                error = sqlite3_errmsg(this->sqlitesql);
            }

            if(!error.empty())
            {
                if(this->error != nullptr)
                {
                    this->error(SQLException(error));
                }
            }
        }
        delete bind;
    }
    return ret;
}

shared_ptr<SQLiteSQLResult> SQLiteSQLClient::select(string table, string columns, string wheres, string order, string group, string having, string limit, PrepareStatement command)
{
    shared_ptr<SQLiteSQLResult> ret = nullptr;
    if(this->isOpen())
    {
        if(!order.empty())
        {
            order = " ORDER BY " + order;
        }
        if(!group.empty())
        {
            group = " GROUP BY " + group;
        }
        if(!having.empty())
        {
            having = " HAVING " + having;
        }
        if(!limit.empty())
        {
            limit = " LIMIT " + limit;
        }
        if(!wheres.empty())
        {
            wheres = " WHERE " + wheres;
        }

        string sql = "SELECT " + columns + " FROM " + table + wheres + group + having + order + limit;
        ret = this->read(sql, command);
    }
    return ret;
}

uint64_t SQLiteSQLClient::update(string table, string values, string wheres, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        if(!wheres.empty())
        {
            wheres = " WHERE " + wheres;
        }
        
        string sql = "UPDATE " + table + " SET " + values + wheres;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t SQLiteSQLClient::insert(string table, string columns, string values, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "INSERT INTO " + table + " ( " + columns + " ) VALUES ( " + values + " )" ;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t SQLiteSQLClient::remove(string table, string wheres, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        if(!wheres.empty())
        {
            wheres = " WHERE " + wheres;
        }

        string sql = "DELETE FROM " + table + wheres;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t SQLiteSQLClient::create_table(string table, string items, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "CREATE TABLE IF NOT EXISTS " + table + " ( " + items + " )";
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t SQLiteSQLClient::drop_table(string table, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "DROP TABLE IF EXISTS " + table;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t SQLiteSQLClient::add_table_column(string table, string column, string type, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "ALERT TABLE " + table + " ADD COLUMN IF NOT EXISTS " + column + " " + type;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t SQLiteSQLClient::alter_table_column(string table, string column, string type, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "ALERT TABLE " + table + " MODIFY IF EXISTS " + column + " " + type;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t SQLiteSQLClient::drop_table_column(string table, string column, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "ALERT TABLE " + table + " DROP COLUMN IF EXISTS " + column;
        ret = this->query(sql, command);
    }
    return ret;
}

string SQLiteSQLClient::escape_string(string value)
{
    string ret = value;
    if(this->isOpen())
    {
    }
    return ret;
}