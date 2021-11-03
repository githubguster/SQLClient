#include "MsSQLClient.hpp"

using namespace ODBC;

MsSQLClient::MsSQLClient(string server_ip, uint16_t server_port, string user_name, string user_password, string database_name, SQLError error)
             :SQLClient(server_ip, server_port, user_name, user_password, database_name, error),
             connected(false)
{
    this->open();
}

MsSQLClient::~MsSQLClient()
{
    this->close();
}

bool MsSQLClient::open()
{
    return this->open("");
}

bool MsSQLClient::open(string source)
{
    bool ret = true;
    this->close();
    
    if(!this->isOpen())
    {
        if(this->error != nullptr)
        {
        }
        ret = false;
    }
    return ret;
}

bool MsSQLClient::close()
{
    bool ret = true;
    if(this->isOpen())
    {
    }
    return ret;
}

bool MsSQLClient::isOpen()
{
    return this->connected;
}

shared_ptr<MsSQLResult> MsSQLClient::read(string sql, PrepareStatement command)
{
    shared_ptr<MsSQLResult> ret = nullptr;
    if(this->isOpen())
    {
        
    }
    return ret;
}

uint64_t MsSQLClient::query(string sql, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        
    }
    return ret;
}

shared_ptr<MsSQLResult> MsSQLClient::select(string table, string columns, string wheres, string order, string group, string having, string limit, PrepareStatement command)
{
    shared_ptr<MsSQLResult> ret = nullptr;
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
            limit = " TOP " + limit + " ";
        }
        if(!wheres.empty())
        {
            wheres = " WHERE " + wheres;
        }

        string sql = "SELECT " + limit + columns + " FROM " + table + wheres + group + having + order;
        ret = this->read(sql, command);
    }
    return ret;
}

uint64_t MsSQLClient::update(string table, string values, string wheres, PrepareStatement command)
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

uint64_t MsSQLClient::insert(string table, string columns, string values, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "INSERT INTO " + table + " ( " + columns + " ) VALUES ( " + values + " )" ;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t MsSQLClient::remove(string table, string wheres, PrepareStatement command)
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

uint64_t MsSQLClient::create_table(string table, string items, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "CREATE TABLE IF NOT EXISTS " + table + " ( " + items + " )";
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t MsSQLClient::drop_table(string table, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "DROP TABLE IF EXISTS " + table;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t MsSQLClient::add_table_column(string table, string column, string type, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "ALERT TABLE " + table + " ADD COLUMN IF NOT EXISTS " + column + " " + type;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t MsSQLClient::alter_table_column(string table, string column, string type, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "ALERT TABLE " + table + " MODIFY IF EXISTS " + column + " " + type;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t MsSQLClient::drop_table_column(string table, string column, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "ALERT TABLE " + table + " DROP COLUMN IF EXISTS " + column;
        ret = this->query(sql, command);
    }
    return ret;
}

string MsSQLClient::escape_string(string value)
{
    string ret = value;
    if(this->isOpen())
    {
    }
    return ret;
}