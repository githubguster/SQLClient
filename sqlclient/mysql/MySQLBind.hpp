/**
 * author: guster
 * date: 2021/06/26
 * description: mysql input parameters
 */

#ifndef __MY_SQL_BIND__
#define __MY_SQL_BIND__

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <mysql/mysql.h>

using namespace std;

namespace ODBC
{
    template <size_t N, typename... Args>
    class Binder<MYSQL_BIND, N, Args...>
    {
    public:
        static void BIND(vector<MYSQL_BIND>* const binds) {}
    };

    template <size_t N, typename T, typename... Args>
    class Binder<MYSQL_BIND, N, T, Args...>
    {
    public:
        static void BIND(vector<MYSQL_BIND>* const binds, const T& value, const Args&... args) {}
    };

    template <size_t N, typename... Args>
    class Binder<MYSQL_BIND, N, char*, Args...>
    {
    public:
        static void BIND(vector<MYSQL_BIND>* const binds, const char* const& value, const Args&... args)
        {
            if(binds->size() <= N)
            {
                binds->push_back(MYSQL_BIND());
            }
            MYSQL_BIND &parameter = binds->at(N);
            memset(&parameter, 0, sizeof(MYSQL_BIND));
            parameter.buffer_type = MYSQL_TYPE_STRING;
            parameter.buffer = const_cast<void *>(static_cast<const void *>(value));
            parameter.buffer_length = strlen(value);
            parameter.length = &parameter.buffer_length;
            parameter.is_unsigned = false;
            parameter.is_null = nullptr;
            Binder<MYSQL_BIND, N + 1, Args...>::BIND(binds, args...);
        }
    };

    template <size_t N, typename... Args>
    class Binder<MYSQL_BIND, N, const char*, Args...>
    {
    public:
        static void BIND(vector<MYSQL_BIND>* const binds, const char* const& value, const Args&... args)
        {
            Binder<MYSQL_BIND, N, char *, Args...>::BIND(binds, const_cast<char *>(value), args...);
        }
    };

    template <size_t N, typename... Args>
    class Binder<MYSQL_BIND, N, string, Args...>
    {
    public:
        static void BIND(vector<MYSQL_BIND>* const binds, const string& value, const Args&... args)
        {
            Binder<MYSQL_BIND, N, const char*, Args...>::BIND(binds, value.c_str(), args...);
        }
    };

    #define MYSQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(type, mysql_type, is_unsigned_type) \
    template <size_t N, typename... Args> \
    class Binder<MYSQL_BIND, N, type, Args...> \
    {\
    public:\
        static void BIND(vector<MYSQL_BIND>* const binds, const type& value, const Args&... args) \
        {\
            if(binds->size() <= N) \
            { \
                binds->push_back(MYSQL_BIND()); \
            } \
            MYSQL_BIND &parameter = binds->at(N); \
            memset(&parameter, 0, sizeof(MYSQL_BIND)); \
            parameter.buffer_type = mysql_type; \
            parameter.buffer = const_cast<void *>(static_cast<const void *>(&value)); \
            parameter.is_unsigned = is_unsigned_type; \
            parameter.is_null = nullptr; \
            Binder<MYSQL_BIND, N + 1, Args...>::BIND(binds, args...);\
        }\
    };

    MYSQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(int8_t, MYSQL_TYPE_TINY, false)
    MYSQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(uint8_t, MYSQL_TYPE_TINY, true)
    MYSQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(int16_t, MYSQL_TYPE_SHORT, false)
    MYSQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(uint16_t, MYSQL_TYPE_SHORT, true)
    MYSQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(int32_t, MYSQL_TYPE_LONG, false)
    MYSQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(uint32_t, MYSQL_TYPE_LONG, true)
    MYSQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(int64_t, MYSQL_TYPE_LONGLONG, false)
    MYSQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(uint64_t, MYSQL_TYPE_LONGLONG, true)

    #define MYSQL_BINDER_FLOATING_TYPE_SPECIALIZATION(type, mysql_type) \
    template <size_t N, typename... Args> \
    class Binder<MYSQL_BIND, N, type, Args...> \
    {\
    public:\
        static void BIND(vector<MYSQL_BIND>* const binds, const type& value, const Args&... args) \
        {\
            if(binds->size() <= N) \
            { \
                binds->push_back(MYSQL_BIND()); \
            } \
            MYSQL_BIND &parameter = binds->at(N); \
            memset(&parameter, 0, sizeof(MYSQL_BIND)); \
            parameter.buffer_type = mysql_type; \
            parameter.buffer = const_cast<void *>(static_cast<const void *>(&value)); \
            parameter.is_unsigned = false; \
            parameter.is_null = nullptr; \
            Binder<MYSQL_BIND, N + 1, Args...>::BIND(binds, args...);\
        }\
    };

    MYSQL_BINDER_FLOATING_TYPE_SPECIALIZATION(float, MYSQL_TYPE_FLOAT)
    MYSQL_BINDER_FLOATING_TYPE_SPECIALIZATION(double, MYSQL_TYPE_DOUBLE)

    class MySQLBind
    {
    private:
        vector<MYSQL_BIND> binds;
        MYSQL_STMT *stmt;
        bool is_free_stmt;
    public:
        MySQLBind(MYSQL_STMT* const stmt, size_t size, bool is_free_stmt = false) 
        { 
            this->stmt = stmt;
            this->bindsResize(size);
            this->is_free_stmt = is_free_stmt;
        }
        MySQLBind(MYSQL_STMT* const stmt, bool is_free_stmt = false) : MySQLBind(stmt, mysql_stmt_param_count(stmt), is_free_stmt) { }
        ~MySQLBind() 
        { 
            this->binds.clear(); 
            if(this->is_free_stmt)
            {
                if(this->stmt != nullptr)
                {
                    mysql_stmt_free_result(this->stmt);
                    mysql_stmt_close(this->stmt);
                }
            }
            this->is_free_stmt = false;
            this->stmt = nullptr;
        }
        void bindsResize(size_t size) { this->binds.resize(size); }
        void bindsResize() { this->binds.resize(mysql_stmt_param_count(this->stmt)); }

        const MYSQL_BIND* getBinds() { return this->binds.data(); }
        MYSQL_STMT* getSTMT() { return this->stmt; };
        
        template<typename... Args>
        void bind_param(const Args&... args)
        {
            if(stmt != nullptr)
            {
                Binder<MYSQL_BIND, 0, Args...>::BIND(&this->binds, args...);
                mysql_stmt_bind_param(this->stmt, this->binds.data());
            }
        }
    };
}

#endif