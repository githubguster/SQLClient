#include "PostgreSQLClient.hpp"

using namespace ODBC;

PostgreSQLClient::PostgreSQLClient(string server_ip, uint16_t server_port, string user_name, string user_password, string database_name, SQLError error)
                 :SQLClient(server_ip, server_port, user_name, user_password, database_name, error)
{
    this->open();
}

PostgreSQLClient::~PostgreSQLClient()
{
    this->close();
}

bool PostgreSQLClient::open()
{
    return this->open("");
}

bool PostgreSQLClient::open(string source)
{
    bool ret = true;
    this->close();
    this->postgresql = PQsetdbLogin(this->server_ip.c_str(), to_string(this->server_port).c_str(), nullptr, nullptr, this->database_name.c_str(), this->user_name.c_str(), this->user_password.c_str());
    if(!this->isOpen())
    {
        if(this->error != nullptr)
        {
            this->error(SQLException(PQerrorMessage(this->postgresql)));
        }
        ret = false;
    }
    return ret;
}

bool PostgreSQLClient::close()
{
    bool ret = true;
    if(this->isOpen())
    {
        PQfinish(this->postgresql);
    }
    return ret;
}

bool PostgreSQLClient::isOpen()
{
    return (this->postgresql != nullptr && PQstatus(this->postgresql) == CONNECTION_OK);
}

shared_ptr<PostgreSQLResult> PostgreSQLClient::read(string sql, PrepareStatement command)
{
    shared_ptr<PostgreSQLResult> ret = nullptr;
    if(this->isOpen())
    {
        string error = "";
        PostgreSQLBind *bind = new PostgreSQLBind();

        if(command != nullptr)
        {
            command(bind);
        }

        PGresult *result = PQexecParams(this->postgresql,
                                        sql.c_str(),
                                        bind->getSize(),
                                        bind->getOids(),
                                        bind->getParams(),
                                        bind->getLengths(),
                                        bind->getFormats(),
                                        1);
        ExecStatusType status = PQresultStatus(result);
        if(result != nullptr)
        {
            if((status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK))
            {
                ret = shared_ptr<PostgreSQLResult>(new PostgreSQLResult(result, true)); 
            }
            else
            {
                error = PQerrorMessage(this->postgresql);
            }
        }
        else
        {
            error = PQerrorMessage(this->postgresql);
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

uint64_t PostgreSQLClient::query(string sql, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string error = "";
        PostgreSQLBind *bind = new PostgreSQLBind();

        if(command != nullptr)
        {
            command(bind);
        }

        PGresult *result = PQexecParams(this->postgresql,
                                        sql.c_str(),
                                        bind->getSize(),
                                        bind->getOids(),
                                        bind->getParams(),
                                        bind->getLengths(),
                                        bind->getFormats(),
                                        1);
        ExecStatusType status = PQresultStatus(result);
        if(result != nullptr)
        {
            if((status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK))
            {
                char *affected_rows = PQcmdTuples(result);
                if(affected_rows != nullptr)
                {
                    if(!string(affected_rows).empty())
                    {
                        ret = static_cast<uint64_t>(atol(affected_rows));
                    }
                    else
                    {
                        ret = 1;
                    }
                }            
            }
            else
            {
                error = PQerrorMessage(this->postgresql);
            }
            PQclear(result);
        }
        else
        {
            error = PQerrorMessage(this->postgresql);
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

shared_ptr<PostgreSQLResult> PostgreSQLClient::select(string table, string columns, string wheres, string order, string group, string having, string limit, PrepareStatement command)
{
    shared_ptr<PostgreSQLResult> ret = nullptr;
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

uint64_t PostgreSQLClient::update(string table, string values, string wheres, PrepareStatement command)
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

uint64_t PostgreSQLClient::insert(string table, string columns, string values, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "INSERT INTO " + table + " ( " + columns + " ) VALUES ( " + values + " )" ;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t PostgreSQLClient::remove(string table, string wheres, PrepareStatement command)
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

uint64_t PostgreSQLClient::create_table(string table, string items, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "CREATE TABLE IF NOT EXISTS " + table + " ( " + items + " )";
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t PostgreSQLClient::drop_table(string table, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "DROP TABLE IF EXISTS " + table;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t PostgreSQLClient::add_table_column(string table, string column, string type, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "ALERT TABLE " + table + " ADD COLUMN IF NOT EXISTS " + column + " " + type;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t PostgreSQLClient::alter_table_column(string table, string column, string type, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "ALERT TABLE " + table + " MODIFY IF EXISTS " + column + " " + type;
        ret = this->query(sql, command);
    }
    return ret;
}

uint64_t PostgreSQLClient::drop_table_column(string table, string column, PrepareStatement command)
{
    uint64_t ret = 0;
    if(this->isOpen())
    {
        string sql = "ALERT TABLE " + table + " DROP COLUMN IF EXISTS " + column;
        ret = this->query(sql, command);
    }
    return ret;
}

string PostgreSQLClient::escape_string(string value)
{
    string ret = value;
    if(this->isOpen())
    {
        int error = 0;
        char *to = static_cast<char *>(calloc(value.length() * 2 + 1, sizeof(char)));
        PQescapeStringConn(this->postgresql, to, value.c_str(), value.length(), &error);
        ret = string(to);
        free(to);
        to = nullptr;
    }
    return ret;
}