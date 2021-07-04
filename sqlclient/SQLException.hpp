/**
 * author: guster
 * date: 2021/06/26
 * description: sql client exception
 */

#include <iostream>
#include <exception>
#include <string>

using namespace std;

namespace ODBC
{
    class SQLException : exception
    {
    private:
        string message;
    public:
        explicit SQLException(const string message) : message(message) {}
        ~SQLException() noexcept { message.clear(); }
        const char* what() const noexcept { return this->message.c_str(); }
    };
}