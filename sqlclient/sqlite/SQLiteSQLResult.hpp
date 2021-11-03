/**
 * author: guster
 * date: 2021/07/18
 * description: sqlite output parameters
 */
#ifndef __SQLITE_SQL_RESULT__
#define __SQLITE_SQL_RESULT__

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
#include <sqlite3.h>
#include "SQLiteSQLDataConvert.hpp"

using namespace std;

namespace ODBC
{
    typedef struct SQLITE_RESULT
    {
        char *buffer;
        int length;
        bool is_null;
    } SQLITE_RESULT;

    template<typename T>
    class ResultSetter<SQLITE_RESULT, T>
    {
    public:
        static void SetResult(const SQLITE_RESULT& bind, T* const value) 
        {
            if(!bind.is_null)
            {
                *value = boost::lexical_cast<T>(bind.buffer);
            }
        }
    };

    template<typename T>
    class ResultSetter<SQLITE_RESULT, shared_ptr<T>>
    {
    public:
        static void SetResult(const SQLITE_RESULT& bind, shared_ptr<T>* const value)
        {
            if(!bind.is_null)
            {
                T* obj = new T;
                ResultSetter<SQLITE_RESULT, T>::SetResult(bind, obj);
                *value = shared_ptr<T>(obj);
            }
            else
            {
                value->reset();
            }
        }
    };

    template<typename T>
    class ResultSetter<SQLITE_RESULT, unique_ptr<T>>
    {
    public:
        static void SetResult(const SQLITE_RESULT& bind, unique_ptr<T>* const value)
        {
            if(!bind.is_null)
            {
                T* obj = new T;
                ResultSetter<SQLITE_RESULT, T>::SetResult(bind, obj);
                *value = unique_ptr<T>(obj);
            }
            else
            {
                value->reset();
            }
        }
    };

    template<typename T>
    class ResultSetter<SQLITE_RESULT, T*>
    {
    public:
        static void SetResult(const SQLITE_RESULT& bind, T** const value)
        {
        }
    };

    template<>
    class ResultSetter<SQLITE_RESULT, string>
    {
    public:
        static void SetResult(const SQLITE_RESULT& bind, string* const value)
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

    #define SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION(type) \
    template<>\
    class ResultSetter<SQLITE_RESULT, type>\
    {\
    public:\
        static void SetResult(const SQLITE_RESULT& bind, type* const value)\
        {\
            if(!bind.is_null)\
            {\
                *value = *static_cast<const decltype(value)>(const_cast<void *>(static_cast<const void *>(bind.buffer)));\
                *value = SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT(type, *value);\
            }\
            else\
            {\
                *value = 0;\
            }\
        }\
    };

    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION(int8_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION(uint8_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION(int16_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION(uint16_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION(int32_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION(uint32_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION(int64_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION(uint64_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION(float)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION(double)

    class SQLiteSQLResult
    {
    private:
        sqlite3_stmt *stmt;
        bool is_free_stmt;
    private:
        template<typename T, int C>
        void setResult(T *const type, const vector<SQLITE_RESULT>& binds, ResultCount<C>)
        {
            ResultSetter<SQLITE_RESULT, typename tuple_element<C, T>::type>::SetResult(binds.at(C), &(get<C>(*type)));
            setResult(type, binds, ResultCount<C - 1>());
        }
        template<typename T>
        void setResult(T *const type, const vector<SQLITE_RESULT>& binds, ResultCount<-1>) { }    
    public:
        SQLiteSQLResult(sqlite3_stmt* const stmt, bool is_free_stmt = false)
        {
            this->stmt = stmt;
            this->is_free_stmt = is_free_stmt;
        }
        ~SQLiteSQLResult() 
        {
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

        sqlite3_stmt* getSTMT() { return this->stmt; }
        const size_t getField() { return sqlite3_column_count(this->stmt); }
        const size_t getParamter() { return sqlite3_data_count(this->stmt); }

        template<typename... Args>
        const size_t bind_param(vector<tuple<Args...>>* const results)
        {
            size_t ret = 0;
            size_t field = this->getField();
            size_t paramter = this->getParamter();
            
            // for(size_t row_number = 0; row_number < paramter; row_number++)
            {
                while(sqlite3_step(this->stmt) != SQLITE_DONE)
                {
                    vector<SQLITE_RESULT> binds(field);
                    for (size_t column_number = 0; column_number < field; column_number++)
                    {
                        SQLITE_RESULT &bind = binds.at(column_number);
                        memset(&bind, 0, sizeof(SQLITE_RESULT));
                        switch (sqlite3_column_type(this->stmt, column_number))
                        {
                        case SQLITE_INTEGER:
                        {
                            sqlite3_int64 i64 = sqlite3_column_int64(this->stmt, column_number);
                            bind.buffer = reinterpret_cast<char *>(&i64);
                        }
                            break;
                        case SQLITE_FLOAT:
                        {
                            double f64 = sqlite3_column_double(this->stmt, column_number);
                            bind.buffer = reinterpret_cast<char *>(&f64);
                        }
                            break;
                        case SQLITE_TEXT:
                        {
                            const uint8_t *text = sqlite3_column_text(this->stmt, column_number);
                            int textLength = sqlite3_column_bytes(this->stmt, column_number);
                            bind.buffer = reinterpret_cast<char *>(const_cast<uint8_t *>(text));
                            bind.length = textLength;
                        }
                            break;
                        case SQLITE_BLOB:
                            break;
                        default:
                            bind.is_null = true;
                            break;
                        }
                    }
                    tuple<Args...> row;
                    setResult(&row, binds, ResultCount<sizeof...(Args) - 1>());

                    results->push_back(move(row));
                    ret++;
                    binds.clear();
                }
            }

            return ret;
        }
    };
}

#endif