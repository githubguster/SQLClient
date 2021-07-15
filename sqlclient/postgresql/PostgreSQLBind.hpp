/**
 * author: guster
 * date: 2021/07/07
 * description: postgresql input parameters
 */

#ifndef __POSTGRE_SQL_BIND__
#define __POSTGRE_SQL_BIND__

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <postgresql/libpq-fe.h>
#include <catalog/pg_type_d.h>

namespace ODBC
{
    #define POSTGRESQL_BIN_TYPE_STRING 0
    #define POSTGRESQL_BIN_TYPE_BINARY 1

    typedef struct POSTGRESQL_BIND
    {
        Oid oid;
        const char *param;
        int length;
        int format;
    } POSTGRESQL_BIND;

    template <size_t N, typename... Args>
    class Binder<POSTGRESQL_BIND, N, Args...>
    {
    public:
        static void BIND(vector<POSTGRESQL_BIND>* const binds) {}
    };

    template <size_t N, typename T, typename... Args>
    class Binder<POSTGRESQL_BIND, N, T, Args...>
    {
    public:
        static void BIND(vector<POSTGRESQL_BIND>* const binds, const T& value, const Args&... args) {}
    };

    template <size_t N, typename... Args>
    class Binder<POSTGRESQL_BIND, N, char*, Args...>
    {
    public:
        static void BIND(vector<POSTGRESQL_BIND>* const binds, const char* const& value, const Args&... args)
        {
            if(binds->size() <= N)
            {
                binds->push_back(POSTGRESQL_BIND());
            }
            POSTGRESQL_BIND &parameter = binds->at(N);
            memset(&parameter, 0, sizeof(POSTGRESQL_BIND));
            parameter.oid = 0;
            parameter.param = static_cast<const char *>(const_cast<void *>(static_cast<const void *>(value)));
            parameter.length = strlen(value);
            parameter.format = POSTGRESQL_BIN_TYPE_STRING;
            Binder<POSTGRESQL_BIND, N + 1, Args...>::BIND(binds, args...);
        }
    };

    template <size_t N, typename... Args>
    class Binder<POSTGRESQL_BIND, N, const char*, Args...>
    {
    public:
        static void BIND(vector<POSTGRESQL_BIND>* const binds, const char* const& value, const Args&... args)
        {
            Binder<POSTGRESQL_BIND, N, char *, Args...>::BIND(binds, const_cast<char *>(value), args...);
        }
    };

    template <size_t N, typename... Args>
    class Binder<POSTGRESQL_BIND, N, string, Args...>
    {
    public:
        static void BIND(vector<POSTGRESQL_BIND>* const binds, const string& value, const Args&... args)
        {
            Binder<POSTGRESQL_BIND, N, const char*, Args...>::BIND(binds, value.c_str(), args...);
        }
    };

    #define POSTGRESQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(type, postgresql_type) \
    template <size_t N, typename... Args> \
    class Binder<POSTGRESQL_BIND, N, type, Args...> \
    {\
    public:\
        static void BIND(vector<POSTGRESQL_BIND>* const binds, const type& value, const Args&... args) \
        {\
            if(binds->size() <= N) \
            { \
                binds->push_back(POSTGRESQL_BIND()); \
            } \
            POSTGRESQL_BIND &parameter = binds->at(N); \
            memset(&parameter, 0, sizeof(POSTGRESQL_BIND)); \
            type *convert = const_cast<type *>(static_cast<const type *>(&value));\
            *convert = POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT(type, value);\
            parameter.oid = postgresql_type; \
            parameter.param =  static_cast<const char *>(const_cast<void *>(static_cast<const void *>(&value))); \
            parameter.length = sizeof(value); \
            parameter.format = POSTGRESQL_BIN_TYPE_BINARY; \
            Binder<POSTGRESQL_BIND, N + 1, Args...>::BIND(binds, args...);\
        }\
    };

    POSTGRESQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(int8_t, CHAROID)
    POSTGRESQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(uint8_t, CHAROID)
    POSTGRESQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(int16_t, INT2OID)
    POSTGRESQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(uint16_t, INT2OID)
    POSTGRESQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(int32_t, INT4OID)
    POSTGRESQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(uint32_t, INT4OID)
    POSTGRESQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(int64_t, INT8OID)
    POSTGRESQL_BINDER_INTERGAL_TYPE_SPECIALIZATION(uint64_t, INT8OID)

    #define POSTGRESQL_BINDER_FLOATING_TYPE_SPECIALIZATION(type, postgresql_type) \
    template <size_t N, typename... Args> \
    class Binder<POSTGRESQL_BIND, N, type, Args...> \
    {\
    public:\
        static void BIND(vector<POSTGRESQL_BIND>* const binds, const type& value, const Args&... args) \
        {\
            if(binds->size() <= N) \
            { \
                binds->push_back(POSTGRESQL_BIND()); \
            } \
            POSTGRESQL_BIND &parameter = binds->at(N); \
            memset(&parameter, 0, sizeof(POSTGRESQL_BIND)); \
            type *convert = const_cast<type *>(static_cast<const type *>(&value));\
            *convert = POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT(type, value);\
            parameter.oid = postgresql_type; \
            parameter.param =  static_cast<const char *>(const_cast<void *>(static_cast<const void *>(&value))); \
            parameter.length = sizeof(value); \
            parameter.format = POSTGRESQL_BIN_TYPE_BINARY; \
            Binder<POSTGRESQL_BIND, N + 1, Args...>::BIND(binds, args...);\
        }\
    };

    POSTGRESQL_BINDER_FLOATING_TYPE_SPECIALIZATION(float, FLOAT4OID)
    POSTGRESQL_BINDER_FLOATING_TYPE_SPECIALIZATION(double, FLOAT8OID)

    class PostgreSQLBind
    {
    private:
        vector<POSTGRESQL_BIND> binds;
        vector<Oid> oids;
        vector<const char *> params;
        vector<int> lengths;
        vector<int> formats;
    public:
        PostgreSQLBind()
        {
        }

        ~PostgreSQLBind()
        {
            this->binds.clear();
            this->oids.clear();
            this->params.clear();
            this->lengths.clear();
            this->formats.clear();
        }

        const Oid* getOids() { return this->oids.size() > 0 ? this->oids.data() : nullptr;  }
        const char** getParams() { return this->params.size() > 0 ? this->params.data() : nullptr; }
        const int* getLengths() { return this->lengths.size() > 0 ? this->lengths.data() : nullptr;  }
        const int* getFormats() { return this->formats.size() > 0 ? this->formats.data() : nullptr; }
        const size_t getSize() { return this->binds.size(); }

        template<typename... Args>
        void bind_param(const Args&... args)
        {
            Binder<POSTGRESQL_BIND, 0, Args...>::BIND(&this->binds, args...);
            size_t size = this->binds.size();
            
            for (size_t count = 0; count < size; count++)
            {
                this->oids.push_back(this->binds.at(count).oid);
                this->params.push_back(this->binds.at(count).param);
                this->lengths.push_back(this->binds.at(count).length);
                this->formats.push_back(this->binds.at(count).format);
            }
        }
    };
}

#endif