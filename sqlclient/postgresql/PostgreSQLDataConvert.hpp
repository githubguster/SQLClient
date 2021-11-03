/**
 * author: guster
 * date: 2021/07/07
 * description: postgresql parameter convert
 */

#ifndef __POSTGRE_SQL_DATA_CONVERT__
#define __POSTGRE_SQL_DATA_CONVERT__

#include <iostream>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>
#include <netinet/in.h>

namespace ODBC
{
    #define POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_2(type) \
    static type postgresql_binder_element_setter_specialization_convert_ ## type (type value)\
    {\
        type ret = 0;\
        ret = static_cast<type>(htons(static_cast<uint16_t>(value)));\
        return ret;\
    }

    #define POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_4(type) \
    static type postgresql_binder_element_setter_specialization_convert_ ## type (type value)\
    {\
        type ret = 0;\
        uint64_t *in = static_cast<uint64_t *>(static_cast<void *>(&value));\
        uint32_t *out = static_cast<uint32_t *>(static_cast<void *>(&ret));\
        out[0] = htonl(static_cast<uint32_t>((*in)));\
        return ret;\
    }

    #define POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_8(type) \
    static type postgresql_binder_element_setter_specialization_convert_ ## type (type value)\
    {\
        type ret = 0;\
        uint64_t *in = static_cast<uint64_t *>(static_cast<void *>(&value));\
        uint32_t *out = static_cast<uint32_t *>(static_cast<void *>(&ret));\
        out[0] = htonl(static_cast<uint32_t>(((*in) >> 32)));\
        out[1] = htonl(static_cast<uint32_t>((*in)));\
        return ret;\
    }

    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_2(int8_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_2(uint8_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_2(int16_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_2(uint16_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_4(int32_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_4(uint32_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_8(int64_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_8(uint64_t)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_4(float)
    POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT_8(double)

    #define POSTGRESQL_BINDER_ELEMENT_SETTER_SPECIALIZATION_CONVERT(type, value) postgresql_binder_element_setter_specialization_convert_ ## type(value)
}
#endif