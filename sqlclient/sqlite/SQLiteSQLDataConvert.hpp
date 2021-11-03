/**
 * author: guster
 * date: 2021/11/02
 * description: sqlite parameter convert
 */

#ifndef __SQLITE_SQL_DATA_CONVERT__
#define __SQLITE_SQL_DATA_CONVERT__

#include <iostream>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>
#include <netinet/in.h>

namespace ODBC
{
    #define SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_2(type) \
    static type sqlite_binder_element_setter_specialization_convert_ ## type (type value)\
    {\
        type ret = 0;\
        ret = static_cast<type>(static_cast<uint16_t>(value));\
        return ret;\
    }

    #define SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_4(type) \
    static type sqlite_binder_element_setter_specialization_convert_ ## type (type value)\
    {\
        type ret = 0;\
        uint64_t *in = static_cast<uint64_t *>(static_cast<void *>(&value));\
        uint32_t *out = static_cast<uint32_t *>(static_cast<void *>(&ret));\
        out[0] = static_cast<uint32_t>((*in));\
        return ret;\
    }

    #define SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_8(type) \
    static type sqlite_binder_element_setter_specialization_convert_ ## type (type value)\
    {\
        type ret = 0;\
        uint64_t *in = static_cast<uint64_t *>(static_cast<void *>(&value));\
        uint32_t *out = static_cast<uint32_t *>(static_cast<void *>(&ret));\
        int endia = 1;\
        bool littleEndian = *(char *)&endia == 1;\
        out[littleEndian ? 1 : 0] = static_cast<uint32_t>(((*in) >> 32));\
        out[littleEndian ? 0 : 1] = static_cast<uint32_t>((*in));\
        return ret;\
    }

    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_2(int8_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_2(uint8_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_2(int16_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_2(uint16_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_4(int32_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_4(uint32_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_8(int64_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_8(uint64_t)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_4(float)
    SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_8(double)

    #define SQLITE_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT(type, value) sqlite_binder_element_setter_specialization_convert_ ## type(value)
}
#endif