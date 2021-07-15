#include <iostream>
#include "MySQLUserDB.hpp"
#include "PostgreSQLUserDB.hpp"

using namespace std;

typedef  enum _DATABASE_TYPE
{
    DATABASE_TYPE_MYSQL = 0,
    DATABASE_TYPE_POSTGRESQL,
} DATABASE_TYPE;

static void show_help(string program)
{
    cout << program << " [-H server ip] [-P server port] [-u username] [-p password] [-D database] [-T 0|1 (mysql|postgresql)]" << endl;
    exit(0);
}

int main(int argc, char const *argv[])
{
    string server = "127.0.0.1";
    uint16_t port = 3306;
    string username = "";
    string userpassword = "";
    string db = "";
    DATABASE_TYPE dbtype = DATABASE_TYPE_MYSQL;

    int c = 0;
	while ((c = getopt (argc, const_cast<char *const *>(argv), "h" "H:P:u:p:D:T:")) != -1)
    {
        switch (c)
        {
        case 'H':
            server = optarg;
            break;
        case 'P':
            port = static_cast<uint16_t>(atoi(optarg));
            break;
        case 'u':
            username = optarg;
            break;
        case 'p':
            userpassword = optarg;
            break;
        case 'D':
            db = optarg;
            break;
        case 'T':
            dbtype = static_cast<DATABASE_TYPE>(atoi(optarg));
            break;
        case 'h':
            show_help(argv[0]);
            break;
        default:
            break;
        }
    }

    if(username.empty() || userpassword.empty() || db.empty())
    {
        show_help(argv[0]);
    }

    #ifdef ENABLE_MYSQL
    if(dbtype == DATABASE_TYPE_MYSQL)
    {
        Test_MySQL::UserDB *mysql = new Test_MySQL::UserDB(server, port, username, userpassword, db, [&](SQLException e) {
            cout << "MYSQL ERROR: " << e.what() << endl;
        });
        
        mysql->add(Test_MySQL::User(0, "USER1"));

        vector<Test_MySQL::User> users = mysql->getAll();

        cout << "NEW" << endl;
        for (size_t count = 0; count < users.size(); ++count)
        {
            Test_MySQL::User user = users.at(count);
            cout << user << endl;
            mysql->update(Test_MySQL::User(user.getId(), user.getAccount() + "_update"));
        }
        users.clear();
        users = mysql->getAll();
        cout << "UPDATE" << endl;
        for (size_t count = 0; count < users.size(); ++count)
        {
            Test_MySQL::User user = users.at(count);
            cout << user << endl;
            mysql->remove(user);
        }
        cout << "DELETE" << endl;
        mysql->drop();
        cout << "DROP" << endl;    
        delete mysql;
    }
    #endif

    #ifdef ENABLE_POSTGRESQL
    if(dbtype == DATABASE_TYPE_POSTGRESQL)
    {
        Test_PostgreSQL::UserDB *postgresql = new Test_PostgreSQL::UserDB(server, port, username, userpassword, db, [&](SQLException e) {
            cout << "POSTGRESQL ERROR: " << e.what() << endl;
        });
        postgresql->add(Test_PostgreSQL::User(0, "USER1"));
        vector<Test_PostgreSQL::User> users = postgresql->getAll();

        cout << "NEW" << endl;
        for (size_t count = 0; count < users.size(); ++count)
        {
            Test_PostgreSQL::User user = users.at(count);
            cout << user << endl;
            postgresql->update(Test_PostgreSQL::User(user.getId(), user.getAccount() + "_update"));
        }
        users.clear();
        users = postgresql->getAll();
        cout << "UPDATE" << endl;
        for (size_t count = 0; count < users.size(); ++count)
        {
            Test_PostgreSQL::User user = users.at(count);
            cout << user << endl;
            postgresql->remove(user);
        }
        cout << "DELETE" << endl;
        postgresql->drop();
        cout << "DROP" << endl;
        delete postgresql;
    }
    return 0;
    #endif
}
