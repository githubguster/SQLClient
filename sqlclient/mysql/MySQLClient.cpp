#include "MySQLClient.hpp"

using namespace ODBC;

MySQLClient::MySQLClient(string server_ip, uint16_t server_port, string user_name, string user_password, string database_name, SQLError error)
             :SQLClient(server_ip, server_port, user_name, user_password, database_name, error),
             mysql(mysql_init(nullptr)),
             connected(false)
{
    this->open();
}

MySQLClient::~MySQLClient()
{
    this->close();
}

bool MySQLClient::open()
{
    return this->open("");
}

bool MySQLClient::open(string source)
{
    bool ret = true;
    this->close();
    this->connected = (mysql_real_connect(this->mysql, this->server_ip.c_str(), this->user_name.c_str(), this->user_password.c_str(), this->database_name.c_str(), this->server_port, nullptr, 0) != nullptr);
    if(!this->isOpen())
    {
        if(this->error != nullptr)
        {
            this->error(SQLException(mysql_error(this->mysql)));
        }
        ret = false;
    }
    return ret;
}

bool MySQLClient::close()
{
    bool ret = true;
    if(this->isOpen())
    {
        mysql_close(this->mysql);
        this->connected = false;
    }
    return ret;
}

bool MySQLClient::isOpen()
{
    return this->connected;
}

shared_ptr<MySQLResult> MySQLClient::read(string sql, PrepareStatement command)
{
    shared_ptr<MySQLResult> ret = nullptr;
    if(this->isOpen())
    {
        shared_ptr<MySQLResult> result = shared_ptr<MySQLResult>(new MySQLResult(mysql_stmt_init(this->mysql), true));
        MySQLBind *bind = new MySQLBind(result->getSTMT(), false);
        if(bind->getSTMT() != nullptr)
        {
            string error = "";
            const size_t length = strlen(sql.c_str());
            if(mysql_stmt_prepare(bind->getSTMT(), sql.c_str(), length) == 0)
            {
                if(command != nullptr)
                {
                    bind->bindsResize();
                    command(bind);
                }

                if(mysql_stmt_execute(bind->getSTMT()) == 0)
                {
                    ret = result;
                }
                else
                {
                    error = mysql_stmt_error(bind->getSTMT());
                }
            }
            else
            {
                error = mysql_stmt_error(bind->getSTMT());
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

uint64_t MySQLClient::query(string sql, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        MySQLBind *bind = new MySQLBind(mysql_stmt_init(this->mysql), true);
        if(bind->getSTMT() != nullptr)
        {
            string error = "";
            const size_t length = strlen(sql.c_str());
            if(mysql_stmt_prepare(bind->getSTMT(), sql.c_str(), length) == 0)
            {
                if(command != nullptr)
                {
                    bind->bindsResize();
                    command(bind);
                }

                if(mysql_stmt_execute(bind->getSTMT()) == 0)
                {
                    ret = static_cast<uint64_t>(mysql_stmt_affected_rows(bind->getSTMT()));
                }
                else
                {
                    error = mysql_stmt_error(bind->getSTMT());
                }
            }
            else
            {
                error = mysql_stmt_error(bind->getSTMT());
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

shared_ptr<MySQLResult> MySQLClient::select(string table, string columns, string wheres, string order, string group, string having, string limit, PrepareStatement command)
{
    shared_ptr<MySQLResult> ret = nullptr;
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

uint64_t MySQLClient::update(string table, string values, string wheres, PrepareStatement command)
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

uint64_t MySQLClient::insert(string table, string columns, string values, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "INSERT INTO " + table + " ( " + columns + " ) VALUES ( " + values + " )" ;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t MySQLClient::remove(string table, string wheres, PrepareStatement command)
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

uint64_t MySQLClient::create_table(string table, string items, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "CREATE TABLE IF NOT EXISTS " + table + " ( " + items + " )";
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t MySQLClient::drop_table(string table, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "DROP TABLE IF EXISTS " + table;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t MySQLClient::add_table_column(string table, string column, string type, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "ALERT TABLE " + table + " ADD COLUMN IF NOT EXISTS " + column + " " + type;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t MySQLClient::alter_table_column(string table, string column, string type, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "ALERT TABLE " + table + " MODIFY IF EXISTS " + column + " " + type;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t MySQLClient::drop_table_column(string table, string column, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "ALERT TABLE " + table + " DROP COLUMN IF EXISTS " + column;
        ret = this->query(sql, command);
    }
    return ret;
}

string MySQLClient::escape_string(string value)
{
    string ret = value;
    if(this->isOpen())
    {
        char *to = static_cast<char *>(calloc(value.length() * 2 + 1, sizeof(char)));
        mysql_real_escape_string(this->mysql, to, value.c_str(), value.length());
        ret = string(to);
        free(to);
        to = nullptr;
    }
    return ret;
}