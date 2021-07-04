#include <iostream>
#include "UserDB.hpp"

using namespace std;
using namespace Test;

static void show_help(string program)
{
    cout << program << " [-H server ip] [-P server port] [-u username] [-p password] [-D database]" << endl;
    exit(0);
}

int main(int argc, char const *argv[])
{
    string server = "127.0.0.1";
    uint16_t port = 3306;
    string username = "";
    string userpassword = "";
    string db = "";

    int c = 0;
	while ((c = getopt (argc, const_cast<char *const *>(argv), "h" "H:P:u:p:D:")) != -1)
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

    UserDB *mysql = new UserDB(server, port, username, userpassword, db);
    mysql->add(User(0, "USER1"));

    vector<User> users = mysql->getAll();

    cout << "NEW" << endl;
    for (size_t count = 0; count < users.size(); ++count)
    {
        User user = users.at(count);
        cout << user << endl;
        mysql->update(User(user.getId(), user.getAccount() + "_update"));
    }
    users.clear();
    users = mysql->getAll();
    cout << "UPDATE" << endl;
    for (size_t count = 0; count < users.size(); ++count)
    {
        User user = users.at(count);
        cout << user << endl;
        mysql->remove(user);
    }
    cout << "DELETE" << endl;
    mysql->drop();
    cout << "DROP" << endl;
    
    delete mysql;
    return 0;
}
