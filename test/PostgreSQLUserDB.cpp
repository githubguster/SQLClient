#ifdef ENABLE_POSTGRESQL
#include "PostgreSQLUserDB.hpp"

using namespace Test_PostgreSQL;

const string UserDB::TABLE = "Test_User_DB";
const string UserDB::ID = "user_id";
const string UserDB::ACCOUNT = "user_account";

const string UserDB::CREATE_TABLE = UserDB::ID + " BIGSERIAL NOT NULL , " +
                                    UserDB::ACCOUNT + " varchar(255) NOT NULL , " +
                                    "PRIMARY KEY (" + UserDB::ID + ") , " +
                                    "UNIQUE (" + UserDB::ACCOUNT + ")";
const float UserDB::VERSION = 1.0F;

typedef tuple<uint64_t, string> _UserTuple;

UserDB::UserDB(string server_ip, uint16_t server_port, string user_name, string user_password, string database_name, SQLError error)
              :PostgreManagementDB(server_ip, server_port, user_name, user_password, database_name, error) 
{
    this->init();
}
UserDB::UserDB(PostgreSQLClient *db)
        :PostgreManagementDB(db) 
{
    this->init();
}

UserDB::~UserDB() 
{
}

uint64_t UserDB::add(User user)
{
    uint64_t ret = 0;
    uint64_t id = user.getId();
    string account = this->db->escape_string(user.getAccount());

    ret = this->db->insert(TABLE,
                            ACCOUNT,
                            " $1 ",
                            [&](PostgreSQLBind *bind)
                            {
                                bind->bind_param(account);
                            });

    return ret;
}

uint64_t UserDB::update(User user)
{
    uint64_t ret = 0;
    uint64_t id = user.getId();
    string account = this->db->escape_string(user.getAccount());

    ret = this->db->update(TABLE,
                            ACCOUNT + " = $1 ",
                            ID + " = $2 ",
                            [&](PostgreSQLBind *bind)
                            {
                                bind->bind_param(account, id);
                            });

    return ret;
}

uint64_t UserDB::remove(User user)
{
    uint64_t ret = 0;
    uint64_t id = user.getId();
    string account = this->db->escape_string(user.getAccount());

    ret = this->db->remove(TABLE,
                            ID + " = $1 ",
                            [&](PostgreSQLBind *bind)
                            {
                                bind->bind_param(id);
                            });
    return ret;
}

vector<User> UserDB::getAll()
{
    vector<User> ret;
    shared_ptr<PostgreSQLResult> reader = this->db->select(TABLE,
                                                            ID + " , " + ACCOUNT,
                                                            "",
                                                            "",
                                                            "",
                                                            "",
                                                            "");

    if(reader != nullptr)
    {
        vector<_UserTuple> _list;
        reader->bind_param(&_list);
        
        for (size_t count = 0; count < _list.size(); ++count) 
        {
            _UserTuple _tuple = _list.at(count);
            User user(std::get<0>(_tuple), std::get<1>(_tuple));
            ret.push_back(user);
        }
        _list.clear();
    }
    return ret;
}

bool UserDB::drop()
{
    bool ret = true;
    this->db->drop_table(TABLE);
    PostgreManagementDB::remove(TABLE);
    return ret;
}

SQLManagement UserDB::table_info()
{
    return SQLManagement(TABLE, VERSION);
}

bool UserDB::create_table()
{
    return this->db->create_table(TABLE, CREATE_TABLE);
}

bool UserDB::update_table(float version)
{
    return true;
}
#endif