/**
 * author: guster
 * date: 2021/07/07
 * description: postgresql output parameters
 */

#ifndef __POSTGRE_SQL_RESULT__
#define __POSTGRE_SQL_RESULT__

#include <iostream>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <tuple>
#include <utility>
#include <type_traits>
#include <memory>
#include <functional>
#include <postgresql/libpq-fe.h>
#include "PostgreSQLDataConvert.hpp"

namespace ODBC
{
    typedef struct POSTGRESQL_RESULT
    {
        char *buffer;
        int length;
        int is_null;
    } POSTGRESQL_RESULT;

    template<typename T>
    class ResultSetter<POSTGRESQL_RESULT, T>
    {
    public:
        static void SetResult(const POSTGRESQL_RESULT& bind, T* const value) 
        {
            if(!bind.is_null)
            {
                *value = boost::lexical_cast<T>(bind.buffer);
            }
        }
    };

    template<typename T>
    class ResultSetter<POSTGRESQL_RESULT, shared_ptr<T>>
    {
    public:
        static void SetResult(const POSTGRESQL_RESULT& bind, shared_ptr<T>* const value)
        {
            if(!bind.is_null)
            {
                T* obj = new T;
                ResultSetter<POSTGRESQL_RESULT, T>::SetResult(bind, obj);
                *value = shared_ptr<T>(obj);
            }
            else
            {
                value->reset();
            }
        }
    };

    template<typename T>
    class ResultSetter<POSTGRESQL_RESULT, unique_ptr<T>>
    {
    public:
        static void SetResult(const POSTGRESQL_RESULT& bind, unique_ptr<T>* const value)
        {
            if(!bind.is_null)
            {
                T* obj = new T;
                ResultSetter<POSTGRESQL_RESULT, T>::SetResult(bind, obj);
                *value = unique_ptr<T>(obj);
            }
            else
            {
                value->reset();
            }
        }
    };

    template<typename T>
    class ResultSetter<POSTGRESQL_RESULT, T*>
    {
    public:
        static void SetResult(const POSTGRESQL_RESULT& bind, T** const value)
        {
        }
    };

    template<>
    class ResultSetter<POSTGRESQL_RESULT, string>
    {
    public:
        static void SetResult(const POSTGRESQL_RESULT& bind, string* const value)
        {
            if(!bind.is_null)
            {
                value->assign(static_cast<const char *>(const_cast<void *>(static_cast<const void *>(bind.buffer))), bind.length);
            }
            else
            {
                value->assign("");
            }
        }
    };
    
    #define POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION(type) \
    template<>\
    class ResultSetter<POSTGRESQL_RESULT, type>\
    {\
    public:\
        static void SetResult(const POSTGRESQL_RESULT& bind, type* const value)\
        {\
            if(!bind.is_null)\
            {\
                *value = *static_cast<const decltype(value)>(const_cast<void *>(static_cast<const void *>(bind.buffer)));\
                *value = POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT(type, *value);\
            }\
            else\
            {\
                *value = 0;\
            }\
        }\
    };

    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION(int8_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION(uint8_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION(int16_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION(uint16_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION(int32_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION(uint32_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION(int64_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION(uint64_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION(float)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION(double)

    class PostgreSQLResult
    {
    private:
        PGresult *result;
        bool is_free_result;
    private:
        template<typename T, int C>
        void setResult(T *const type, const vector<POSTGRESQL_RESULT>& binds, ResultCount<C>)
        {
            ResultSetter<POSTGRESQL_RESULT, typename tuple_element<C, T>::type>::SetResult(binds.at(C), &(get<C>(*type)));
            setResult(type, binds, ResultCount<C - 1>());
        }
        template<typename T>
        void setResult(T *const type, const vector<POSTGRESQL_RESULT>& binds, ResultCount<-1>) { }    
    public:
        PostgreSQLResult(PGresult *result, bool is_free_result = true)
        {
            this->result = result;
            this->is_free_result = is_free_result;
        }
        
        ~PostgreSQLResult()
        {
            if(this->is_free_result)
            {
                PQclear(this->result);
            }
            this->is_free_result = false;
            this->result = nullptr;
        }

        const size_t getField() { return PQnfields(this->result); }
        const size_t getParamter() { return PQntuples(this->result); }

        template<typename... Args>
        const size_t bind_param(vector<tuple<Args...>>* const results)
        {
            size_t ret = 0;
            size_t field = this->getField();
            size_t paramter = this->getParamter();
            
            for(size_t row_number = 0; row_number < paramter; row_number++)
            {
                vector<POSTGRESQL_RESULT> binds(field);
                for (size_t column_number = 0; column_number < field; column_number++)
                {
                    POSTGRESQL_RESULT &bind = binds.at(column_number);
                    memset(&bind, 0, sizeof(POSTGRESQL_RESULT));
                    bind.is_null = PQgetisnull(this->result, row_number, column_number);
                    if(bind.is_null == 0)
                    {
                        bind.buffer = PQgetvalue(this->result, row_number, column_number);
                        bind.length = PQgetlength(this->result, row_number, column_number);
                    }
                }
                tuple<Args...> row;
                setResult(&row, binds, ResultCount<sizeof...(Args) - 1>());

                results->push_back(move(row));
                ret++;
                binds.clear();
            }

            return ret;
        }
    };
}

#endif