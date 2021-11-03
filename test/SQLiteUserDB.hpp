#ifndef __SQLITE_USER_DB__
#define __SQLITE_USER_DB__
#ifdef ENABLE_SQLITE

#include <iostream>
#include <string>
#include <vector>
#include "sqlite/SQLiteSQLManagementDB.hpp"

using namespace ODBC;

namespace Test_SQLiteSQL
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

    class UserDB : public SQLiteSQLManagementDB
    {
    public:
        static const string TABLE;
        static const string ID;
        static const string ACCOUNT;
    private:
        static const string CREATE_TABLE;
        static const float VERSION;
    public:
        UserDB(string database_name, SQLError error = nullptr);
        UserDB(SQLiteSQLClient *db);
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
#endif