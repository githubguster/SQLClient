/**
 * author: guster
 * date: 2021/11/02
 * description: sqlite input parameters
 */

#ifndef __SQLITE_SQL_BIND__
#define __SQLITE_SQL_BIND__

#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>
#include <vector>
#include <sqlite3.h>

using namespace std;

namespace ODBC
{
    enum SQLITESQL_BIND_TYPE
    {
        SQLITESQL_BIND_TYPE_INT,
        SQLITESQL_BIND_TYPE_INT64,
        SQLITESQL_BIND_TYPE_DOUBLE,
        SQLITESQL_BIND_TYPE_STRING,
        // SQLITESQL_BIND_TYPE_BINARY,
    };

    typedef struct SQLITESQL_BIND
    {
        SQLITESQL_BIND_TYPE buffer_type;
        void *buffer;
        int length;
    } SQLITESQL_BIND;

    template <size_t N, typename... Args>
    class Binder<SQLITESQL_BIND, N, Args...>
    {
    public:
        static void BIND(vector<SQLITESQL_BIND>* const binds) {}
    };

    template <size_t N, typename T, typename... Args>
    class Binder<SQLITESQL_BIND, N, T, Args...>
    {
    public:
        static void BIND(vector<SQLITESQL_BIND>* const binds, const T& value, const Args&... args) {}
    };

    template <size_t N, typename... Args>
    class Binder<SQLITESQL_BIND, N, char*, Args...>
    {
    public:
        static void BIND(vector<SQLITESQL_BIND>* const binds, const char* const& value, const Args&... args)
        {
            if(binds->size() <= N)
            {
                binds->push_back(SQLITESQL_BIND());
            }
            SQLITESQL_BIND &parameter = binds->at(N);
            memset(&parameter, 0, sizeof(SQLITESQL_BIND));
            parameter.buffer_type = SQLITESQL_BIND_TYPE_STRING;
            parameter.buffer = const_cast<void *>(static_cast<const void *>(value));
            parameter.length = strlen(value);
            Binder<SQLITESQL_BIND, N + 1, Args...>::BIND(binds, args...);
        }
    };

    template <size_t N, typename... Args>
    class Binder<SQLITESQL_BIND, N, const char*, Args...>
    {
    public:
        static void BIND(vector<SQLITESQL_BIND>* const binds, const char* const& value, const Args&... args)
        {
            Binder<SQLITESQL_BIND, N, char *, Args...>::BIND(binds, const_cast<char *>(value), args...);
        }
    };

    template <size_t N, typename... Args>
    class Binder<SQLITESQL_BIND, N, string, Args...>
    {
    public:
        static void BIND(vector<SQLITESQL_BIND>* const binds, const string& value, const Args&... args)
        {
            Binder<SQLITESQL_BIND, N, const char*, Args...>::BIND(binds, value.c_str(), args...);
        }
    };

    #define SQLITE_BINDER_INTERGAL_TYPE_SPECIALIZATION(type, sqlite_type) \
    template <size_t N, typename... Args> \
    class Binder<SQLITESQL_BIND, N, type, Args...> \
    {\
    public:\
        static void BIND(vector<SQLITESQL_BIND>* const binds, const type& value, const Args&... args) \
        {\
            if(binds->size() <= N) \
            { \
                binds->push_back(SQLITESQL_BIND()); \
            } \
            SQLITESQL_BIND &parameter = binds->at(N); \
            memset(&parameter, 0, sizeof(SQLITESQL_BIND)); \
            parameter.buffer_type = sqlite_type; \
            parameter.buffer = const_cast<void *>(static_cast<const void *>(&value)); \
            Binder<SQLITESQL_BIND, N + 1, Args...>::BIND(binds, args...);\
        }\
    };

    SQLITE_BINDER_INTERGAL_TYPE_SPECIALIZATION(int8_t, SQLITESQL_BIND_TYPE_INT)
    SQLITE_BINDER_INTERGAL_TYPE_SPECIALIZATION(uint8_t, SQLITESQL_BIND_TYPE_INT)
    SQLITE_BINDER_INTERGAL_TYPE_SPECIALIZATION(int16_t, SQLITESQL_BIND_TYPE_INT)
    SQLITE_BINDER_INTERGAL_TYPE_SPECIALIZATION(uint16_t, SQLITESQL_BIND_TYPE_INT)
    SQLITE_BINDER_INTERGAL_TYPE_SPECIALIZATION(int32_t, SQLITESQL_BIND_TYPE_INT)
    SQLITE_BINDER_INTERGAL_TYPE_SPECIALIZATION(uint32_t, SQLITESQL_BIND_TYPE_INT)
    SQLITE_BINDER_INTERGAL_TYPE_SPECIALIZATION(int64_t, SQLITESQL_BIND_TYPE_INT64)
    SQLITE_BINDER_INTERGAL_TYPE_SPECIALIZATION(uint64_t, SQLITESQL_BIND_TYPE_INT64)

    #define SQLITE_BINDER_FLOATING_TYPE_SPECIALIZATION(type, sqlite_type) \
    template <size_t N, typename... Args> \
    class Binder<SQLITESQL_BIND, N, type, Args...> \
    {\
    public:\
        static void BIND(vector<SQLITESQL_BIND>* const binds, const type& value, const Args&... args) \
        {\
            if(binds->size() <= N) \
            { \
                binds->push_back(SQLITESQL_BIND()); \
            } \
            SQLITESQL_BIND &parameter = binds->at(N); \
            memset(&parameter, 0, sizeof(SQLITESQL_BIND)); \
            parameter.buffer_type = sqlite_type; \
            parameter.buffer = const_cast<void *>(static_cast<const void *>(&value)); \
            Binder<SQLITESQL_BIND, N + 1, Args...>::BIND(binds, args...);\
        }\
    };

    SQLITE_BINDER_FLOATING_TYPE_SPECIALIZATION(float, SQLITESQL_BIND_TYPE_DOUBLE)
    SQLITE_BINDER_FLOATING_TYPE_SPECIALIZATION(double, SQLITESQL_BIND_TYPE_DOUBLE)

    class SQLiteSQLBind
    {
    private:
        vector<SQLITESQL_BIND> binds;
        sqlite3_stmt *stmt;
        bool is_free_stmt;
    public:
        SQLiteSQLBind(sqlite3_stmt* const stmt, size_t size, bool is_free_stmt = false) 
        { 
            this->stmt = stmt;
            this->bindsResize(size);
            this->is_free_stmt = is_free_stmt;
        }
        SQLiteSQLBind(sqlite3_stmt* const stmt, bool is_free_stmt = false) : SQLiteSQLBind(stmt, sqlite3_bind_parameter_count(stmt), is_free_stmt) { }
        ~SQLiteSQLBind() 
        { 
            this->binds.clear();
            if(this->is_free_stmt)
            {
                if(this->stmt != nullptr)
                {
                    sqlite3_finalize(this->stmt);
                }
            }
            this->is_free_stmt = false;
            this->stmt = nullptr;
        }
        void bindsResize(size_t size) { this->binds.resize(size); }
        void bindsResize() { this->binds.resize(sqlite3_bind_parameter_count(this->stmt)); }

        const SQLITESQL_BIND* getBinds() { return this->binds.data(); }
        sqlite3_stmt* getSTMT() { return this->stmt; };

        template<typename... Args>
        void bind_param(const Args&... args)
        {
            if(stmt != nullptr)
            {
                Binder<SQLITESQL_BIND, 0, Args...>::BIND(&this->binds, args...);
                size_t size = this->binds.size();
            
                for (size_t count = 0; count < size; count++)
                {
                    switch(this->binds.at(count).buffer_type)
                    {
                        case SQLITESQL_BIND_TYPE_INT:
                            sqlite3_bind_int(this->stmt, count + 1, *static_cast<int *>(this->binds.at(count).buffer));
                        break;
                        case SQLITESQL_BIND_TYPE_INT64:
                            sqlite3_bind_int64(this->stmt, count + 1, *static_cast<sqlite3_int64 *>(this->binds.at(count).buffer));
                        break;
                        case SQLITESQL_BIND_TYPE_DOUBLE:
                            sqlite3_bind_double(this->stmt, count + 1, *static_cast<float *>(this->binds.at(count).buffer));
                        break;
                        case SQLITESQL_BIND_TYPE_STRING:
                            sqlite3_bind_text(this->stmt, count + 1, static_cast<char *>(this->binds.at(count).buffer), this->binds.at(count).length, SQLITE_TRANSIENT);
                        break;
                    }
                }
            }
        }
    };
}

#endif