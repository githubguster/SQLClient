/**
 * author: guster
 * date: 2021/06/26
 * description: mysql output parameters
 */

#ifndef __MY_SQL_RESULT__
#define __MY_SQL_RESULT__

#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>
#include <boost/lexical_cast.hpp>
#include <mysql/mysql.h>
#include <vector>
#include <tuple>
#include <utility>
#include <type_traits>
#include <memory>
#include <functional>

using namespace std;

namespace ODBC
{
    template<typename T>
    class ParameterSetter<MYSQL_BIND, T>
    {
    public:
        static void SetParameter(MYSQL_BIND* const bind, vector<char> * const buffer, my_bool* const is_null_flag)
        {
            memset(bind, 0, sizeof(MYSQL_BIND));
            if(buffer->size() <= 0)
            {
                buffer->resize(sizeof(char));
            }

            bind->buffer_type = MYSQL_TYPE_STRING;
            bind->buffer = buffer->data();
            bind->buffer_length = buffer->size();
            bind->length = &bind->buffer_length;
            bind->is_null = is_null_flag;
        }
    };
    
    template<typename T>
    class ParameterSetter<MYSQL_BIND, shared_ptr<T>>
    {
    public:
        static void SetParameter(MYSQL_BIND* const bind, vector<char> * const buffer, my_bool* const is_null_flag)
        {
            ParameterSetter<MYSQL_BIND, T>::SetParameter(bind, buffer, is_null_flag);
        }
    };

    template<typename T>
    class ParameterSetter<MYSQL_BIND, unique_ptr<T>>
    {
    public:
        static void SetParameter(MYSQL_BIND* const bind, vector<char> * const buffer, my_bool* const is_null_flag)
        {
            ParameterSetter<MYSQL_BIND, T>::SetParameter(bind, buffer, is_null_flag);
        }
    };

    template<typename T>
    class ParameterSetter<MYSQL_BIND, T*>
    {
    public:
        static void SetParameter(MYSQL_BIND* const bind, vector<char> * const buffer, my_bool* const is_null_flag)
        {
        }
    };

    #define BINDER_PARAMETER_SETTER_SECIALIZATION(type, mysql_type, is_unsigned_type) \
    template<> \
    class ParameterSetter<MYSQL_BIND, type> \
    {\
    public:\
        static void SetParameter(MYSQL_BIND* const bind, vector<char> * const buffer, my_bool* const is_null_flag) \
        {\
            memset(bind, 0, sizeof(MYSQL_BIND));\
            buffer->resize(sizeof(type));\
            bind->buffer_type = mysql_type;\
            bind->buffer = buffer->data();\
            bind->is_unsigned = is_unsigned_type;\
            bind->is_null = is_null_flag;\
        }\
    };

    BINDER_PARAMETER_SETTER_SECIALIZATION(int8_t, MYSQL_TYPE_TINY, false)
    BINDER_PARAMETER_SETTER_SECIALIZATION(uint8_t, MYSQL_TYPE_TINY, true)
    BINDER_PARAMETER_SETTER_SECIALIZATION(int16_t, MYSQL_TYPE_SHORT, false)
    BINDER_PARAMETER_SETTER_SECIALIZATION(uint16_t, MYSQL_TYPE_SHORT, true)
    BINDER_PARAMETER_SETTER_SECIALIZATION(int32_t, MYSQL_TYPE_LONG, false)
    BINDER_PARAMETER_SETTER_SECIALIZATION(uint32_t, MYSQL_TYPE_LONG, true)
    BINDER_PARAMETER_SETTER_SECIALIZATION(int64_t, MYSQL_TYPE_LONGLONG, false)
    BINDER_PARAMETER_SETTER_SECIALIZATION(uint64_t, MYSQL_TYPE_LONGLONG, true)
    BINDER_PARAMETER_SETTER_SECIALIZATION(float, MYSQL_TYPE_FLOAT, false)
    BINDER_PARAMETER_SETTER_SECIALIZATION(double, MYSQL_TYPE_DOUBLE, false)

    template<typename T>
    class ResultSetter<MYSQL_BIND, T>
    {
    public:
        static void SetResult(const MYSQL_BIND& bind, T* const value) 
        {
            if(!*bind.is_null)
            {
                *value = boost::lexical_cast<T>(static_cast<char *>(bind.buffer));
            }
        }
    };

    template<typename T>
    class ResultSetter<MYSQL_BIND, shared_ptr<T>>
    {
    public:
        static void SetResult(const MYSQL_BIND& bind, shared_ptr<T>* const value)
        {
            if(!*bind.is_null)
            {
                T* obj = new T;
                ResultSetter<MYSQL_BIND, T>::SetResult(bind, obj);
                *value = shared_ptr<T>(obj);
            }
            else
            {
                value->reset();
            }
        }
    };

    template<typename T>
    class ResultSetter<MYSQL_BIND, unique_ptr<T>>
    {
    public:
        static void SetResult(const MYSQL_BIND& bind, unique_ptr<T>* const value)
        {
            if(!*bind.is_null)
            {
                T* obj = new T;
                ResultSetter<MYSQL_BIND, T>::SetResult(bind, obj);
                *value = unique_ptr<T>(obj);
            }
            else
            {
                value->reset();
            }
        }
    };

    template<typename T>
    class ResultSetter<MYSQL_BIND, T*>
    {
    public:
        static void SetResult(const MYSQL_BIND& bind, T** const value)
        {
        }
    };

    template<>
    class ResultSetter<MYSQL_BIND, string>
    {
    public:
        static void SetResult(const MYSQL_BIND& bind, string* const value)
        {
            if(!*bind.is_null)
            {
                value->assign(static_cast<const char *>(bind.buffer), *(bind.length));
            }
            else
            {
                value->assign("");
            }
        }
    };

    #define BINDER_ELEMENT_SETTER_SPECIALIZATION(type) \
    template<>\
    class ResultSetter<MYSQL_BIND, type>\
    {\
    public:\
        static void SetResult(const MYSQL_BIND& bind, type* const value)\
        {\
            if(!*bind.is_null)\
            {\
                *value = *static_cast<const decltype(value)>(bind.buffer);\
            }\
            else\
            {\
                *value = 0;\
            }\
        }\
    };

    BINDER_ELEMENT_SETTER_SPECIALIZATION(int8_t)
    BINDER_ELEMENT_SETTER_SPECIALIZATION(uint8_t)
    BINDER_ELEMENT_SETTER_SPECIALIZATION(int16_t)
    BINDER_ELEMENT_SETTER_SPECIALIZATION(uint16_t)
    BINDER_ELEMENT_SETTER_SPECIALIZATION(int32_t)
    BINDER_ELEMENT_SETTER_SPECIALIZATION(uint32_t)
    BINDER_ELEMENT_SETTER_SPECIALIZATION(int64_t)
    BINDER_ELEMENT_SETTER_SPECIALIZATION(uint64_t)
    BINDER_ELEMENT_SETTER_SPECIALIZATION(float)
    BINDER_ELEMENT_SETTER_SPECIALIZATION(double)

    template<int I> struct ResultCount {};

    //typedef typename remove_reference<decltype(*declval<typename remove_reference<decltype(declval<MYSQL_BIND>().length)>::type>())>::type mysql_bind_length_t;

    class MySQLResult
    {
    private:
        MYSQL_STMT *stmt;
        bool is_free_stmt;
    private:
        template<typename T, int C>
        void bindParameters(const T& type, vector<MYSQL_BIND>* const binds, vector<vector<char>>* const buffers, vector<my_bool>* const null_flags, ResultCount<C>) 
        {
            ParameterSetter<MYSQL_BIND, typename tuple_element<C, T>::type>::SetParameter(&binds->at(C), &buffers->at(C), &null_flags->at(C));
            bindParameters(type, binds, buffers, null_flags, ResultCount<C - 1>());
        }
        template<typename T>
        void bindParameters(const T& type, vector<MYSQL_BIND>* const binds, vector<vector<char>>* const buffers, vector<my_bool>* const null_flags, ResultCount<-1>) {}
        template<typename T, int C>
        void setResult(T *const type, const vector<MYSQL_BIND>& binds, ResultCount<C>)
        {
            ResultSetter<MYSQL_BIND, typename tuple_element<C, T>::type>::SetResult(binds.at(C), &(get<C>(*type)));
            setResult(type, binds, ResultCount<C - 1>());
        }
        template<typename T>
        void setResult(T *const type, const vector<MYSQL_BIND>& binds, ResultCount<-1>) { }
        void refetch(vector<MYSQL_BIND>* const binds, vector<vector<char>>* const buffers, vector<uint64_t>* const lengths)
        {
            vector<tuple<uint32_t, uint64_t>> columns;
            for (size_t count = 0; count < lengths->size(); ++count) 
            {
                vector<char>& buffer = buffers->at(count);
                const size_t length = lengths->at(count);
                if (length > buffer.size()) 
                {
                    const size_t retrieved = buffer.size();
                    columns.push_back(tuple<size_t, size_t>(count, retrieved));
                    buffer.resize(length + 1);
                    MYSQL_BIND& bind = binds->at(count);
                    bind.buffer = &buffer.at(retrieved);
                    bind.buffer_length = buffer.size() - retrieved - 1;
                }
            }

            if (columns.empty()) 
            {
                return;
            }

            for (size_t count = 0; count < columns.size(); count++) 
            {
                tuple<uint32_t, uint64_t> columnTuple = columns.at(count);
                const uint32_t column = get<0>(columnTuple);
                const uint64_t offset = get<1>(columnTuple);
                MYSQL_BIND& bind = binds->at(column);
                
                if(!mysql_stmt_fetch_column(this->stmt, &bind, column, offset))
                {
                    vector<char>& buffer = buffers->at(column);
                    bind.buffer = buffer.data();
                    bind.buffer_length = buffer.size();
                }
            }

            if(!mysql_stmt_bind_result(this->stmt, binds->data()))
            {
            }
        }
    public:
        MySQLResult(MYSQL_STMT* const stmt, bool is_free_stmt = false)
        {
            this->stmt = stmt;
            this->is_free_stmt = is_free_stmt;
        }
        ~MySQLResult() 
        {
            if(this->is_free_stmt)
            {
                mysql_stmt_free_result(this->stmt);
                mysql_stmt_close(this->stmt);
            }
            this->is_free_stmt = false;
            this->stmt = nullptr;
        }

        MYSQL_STMT* getSTMT() { return this->stmt; }
        const size_t getField() { return mysql_stmt_field_count(this->stmt); }
        const size_t getParamter() { return mysql_stmt_param_count(this->stmt); }

        template<typename... Args>
        const size_t bind_param(vector<tuple<Args...>>* const results)
        {
            size_t ret = 0;
            size_t field = this->getField();
            
            vector<MYSQL_BIND> binds(field);
            vector<vector<char>> buffers(field);
            vector<uint64_t> lengths(field);
            vector<my_bool> null_floats(field);

            tuple<Args...> unsed;
            bindParameters(unsed, &binds, &buffers, &null_floats, ResultCount<sizeof...(Args) - 1>());

            for(size_t count = 0; count < field; count++)
            {
                binds.at(count).length = &lengths.at(count);
            }

            mysql_stmt_bind_result(this->stmt, binds.data());
            mysql_stmt_store_result(this->stmt);

            int fetch_status = mysql_stmt_fetch(this->stmt);
            while(fetch_status == 0 || fetch_status == MYSQL_DATA_TRUNCATED)
            {
                if(fetch_status == MYSQL_DATA_TRUNCATED)
                {
                    refetch(&binds, &buffers, &lengths);
                }
                tuple<Args...> row;
                setResult(&row, binds, ResultCount<sizeof...(Args) - 1>());

                results->push_back(move(row));
                ret++;
                fetch_status = mysql_stmt_fetch(this->stmt);
            }
            return ret;
        }
    };
}

#endif