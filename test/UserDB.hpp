#ifndef __USER_DB__
#define __USER_DB__

#include <iostream>
#include <string>
#include <vector>
#include "mysql/MySQLManagementDB.hpp"

using namespace ODBC;

namespace Test
{
    class User
    {
    private:
        uint64_t id;
        string account;
    public:
        User(uint64_t id, string account) : id(id), account(account) {}
        ~User() { this->id = 0; this->account.clear(); }
        const uint64_t getId() { return this->id; }
        const string getAccount() { return this->account; }

        friend ostream& operator<<(ostream &out, User user)
        {
            out << "User ==> ID: " + std::to_string(user.getId()) + " Account: " + user.getAccount();
            return out;
        }
    };

    class UserDB : public MySQLManagementDB
    {
    public:
        static const string TABLE;
        static const string ID;
        static const string ACCOUNT;
    private:
        static const string CREATE_TABLE;
        static const float VERSION;
    public:
        UserDB(string server_ip, uint16_t server_port, string user_name, string user_password, string database_name);
        UserDB(MySQLClient *db);
        ~UserDB();

        uint64_t add(User user);
        uint64_t update(User user);
        uint64_t remove(User user);
        vector<User> getAll();
        bool drop();

        SQLManagement table_info();
        bool create_table();
        bool update_table(float version);
    };
}


#endif