/**
 * author: guster
 * date: 2021/06/26
 * description: sql client interface
 */

#ifndef __SQL_CLIENT__
#define __SQL_CLIENT__

#include <iostream>
#include <string>
#include <cstring>
#include <functional>
#include "SQLException.hpp"

using namespace std;

namespace ODBC
{
    class SQLManagement
    {
    private:
        string name;
        float version;
    public:
        SQLManagement(string name, float version) : name(name), version(version) {}
        ~SQLManagement() { name.clear(); version = 0; }
        string getName() const { return name; }
        void setName(string name) { this->name = name; }
        float getVersion() const { return version; }
        void setVersion(float version) { this->version = version; };
        friend bool operator == (const SQLManagement &v1, const SQLManagement &v2)
        {
            return v1.getName() == v2.getName() && v1.getVersion() == v2.getVersion();
        }
        friend bool operator != (const SQLManagement &v1, const SQLManagement &v2)
        {
            return !(v1 == v2);
        }
    };

    template<typename DB>
    class SQLManagementDB
    {
    public:
        const string TABLE = "Sys_DB_Management";
        const string NAME = "management_name";
        const string VERSION = "management_version";

    protected:
        bool is_auto_close;
        DB *db;
    public:
        SQLManagementDB(string server_ip, uint16_t server_port, string user_name, string user_password, string database_name) 
                        :db(nullptr),
                        is_auto_close(true) {}
        SQLManagementDB(DB *db)
                        :db(db),
                        is_auto_close(false) {}
        virtual ~SQLManagementDB()
        {
        }

        virtual void init() = 0;
        virtual bool check(string name) = 0;
        virtual uint64_t add(SQLManagement management) = 0;
        virtual uint64_t update(SQLManagement management) = 0;
        virtual uint64_t remove(string name) = 0;
        virtual SQLManagement get(string name) = 0;

        virtual SQLManagement table_info() = 0;
        virtual bool create_table() = 0;
        virtual bool update_table(float version) = 0;
    };

    template <typename H, size_t N, typename... Args>
    class Binder
    {
    public:
        static void BIND(vector<H>* const binders) {}
    };

    template<typename H, typename T>
    class ParameterSetter
    {
    public:
        static void SetParameter(H* const parameters, vector<char> * const buffer, my_bool* const is_null_flag) {}
    };

    template<typename H, typename T>
    class ResultSetter
    {
    public:
        static void SetResult(const H& setters, T* const value);
    };

    template<typename Reader, typename Prepare>
    class SQLClient
    {
    public:
        typedef function<void(SQLException e)> SQLError;
        typedef function<void(Prepare prepare)> PrepareStatement;
    protected:
        string server_ip;
        uint16_t server_port;
        string user_name;
        string user_password;
        string database_name;
        SQLError error;
    public:
        SQLClient(string server_ip, uint16_t server_port, string user_name, string user_password, string database_name)
                :server_ip(server_ip),
                server_port(server_port),
                user_name(user_name),
                user_password(user_password),
                database_name(database_name) {}

        virtual ~SQLClient()
        {
            server_ip.clear();
            server_port = 0;
            user_name.clear();
            user_password.clear();
            database_name.clear();
        }

        void set_error(SQLError error)
        {
            this->error = error;   
        }

        virtual bool open(string source) = 0;
        virtual bool close() = 0;
        virtual bool isOpen() = 0;
        virtual Reader read(string sql, PrepareStatement command = nullptr) = 0;
        virtual uint64_t query(string sql, PrepareStatement command = nullptr) = 0;
        virtual Reader select(string table, string columns, string wheres, string order, string group, string having, string limit, PrepareStatement command = nullptr) = 0;
        virtual uint64_t update(string table, string values, string wheres, PrepareStatement command = nullptr) = 0;
        virtual uint64_t insert(string table, string columns, string values, PrepareStatement command = nullptr) = 0;
        virtual uint64_t remove(string table, string wheres, PrepareStatement command = nullptr) = 0;
        virtual uint64_t create_table(string table, string items, PrepareStatement command = nullptr) = 0;
        virtual uint64_t drop_table(string table, PrepareStatement command = nullptr) = 0;
        virtual uint64_t add_table_column(string table, string column, string type, PrepareStatement command = nullptr) = 0;
        virtual uint64_t alter_table_column(string table, string column, string type, PrepareStatement command = nullptr) = 0;
        virtual uint64_t drop_table_column(string table, string column, PrepareStatement command = nullptr) = 0;

        virtual string escape_string(string value) = 0;
    };
} 

#endif