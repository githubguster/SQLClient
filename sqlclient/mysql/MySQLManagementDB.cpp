#include "MySQLManagementDB.hpp"

using namespace ODBC;

bool MySQLManagementDB::MUTEX_IS_INIT = false;
bool MySQLManagementDB::IS_INIT = false;
pthread_mutex_t MySQLManagementDB::MUTEX;
list<SQLManagement> MySQLManagementDB::MANAGEMENTS;

typedef tuple<string, float> _Tuple;

MySQLManagementDB::MySQLManagementDB(string server_ip, uint16_t server_port, string user_name, string user_password, string database_name, SQLError error)
                  :SQLManagementDB(server_ip, server_port, user_name, user_password, database_name, error)
{
    this->db = new MySQLClient(server_ip, server_port, user_name, user_password, database_name, error);
}

MySQLManagementDB::MySQLManagementDB(MySQLClient *db)
                  :SQLManagementDB(db)
{
}

MySQLManagementDB::~MySQLManagementDB()
{
    if(this->is_auto_close)
    {
        if(this->db != nullptr)
        {
            this->db->close();
            delete this->db;
        }
    }
    this->db = nullptr;
    this->is_auto_close = false;
}

void MySQLManagementDB::init()
{
    if(!MySQLManagementDB::MUTEX_IS_INIT)
    {
        MySQLManagementDB::MUTEX_IS_INIT = true;
        pthread_mutex_init(&MySQLManagementDB::MUTEX, nullptr);
    }

    pthread_mutex_lock(&MySQLManagementDB::MUTEX);
    if(!MySQLManagementDB::IS_INIT)
    {
        MySQLManagementDB::IS_INIT = true;

        this->db->create_table(TABLE, CREATE_TABLE);

        MySQLManagementDB::MANAGEMENTS.push_back(SQLManagement(TABLE, MySQLManagementDB::ERROR_VERSION));
    }

    SQLManagement information = table_info();
    list<SQLManagement>::iterator management = find(MySQLManagementDB::MANAGEMENTS.begin(), MySQLManagementDB::MANAGEMENTS.end(), information);
    
    if(management == MySQLManagementDB::MANAGEMENTS.end())
    {
        SQLManagement sqlManagement = get(information.getName());

        if(sqlManagement.getVersion() != MySQLManagementDB::ERROR_VERSION)
        {
            MySQLManagementDB::MANAGEMENTS.push_front(sqlManagement);
            management = find(MySQLManagementDB::MANAGEMENTS.begin(), MySQLManagementDB::MANAGEMENTS.end(), sqlManagement);
        }
    }

    if(management == MySQLManagementDB::MANAGEMENTS.end())
    {
        if(create_table())
        {
            add(information);
            MySQLManagementDB::MANAGEMENTS.push_front(information);
        }
    }
    else if(information.getVersion() > management->getVersion())
    {
        if(update_table(management->getVersion()))
        {
            update(information);
            management->setVersion(information.getVersion());
        }
    }

    pthread_mutex_unlock(&MySQLManagementDB::MUTEX);
}

bool MySQLManagementDB::check(string name)
{
    bool ret = true;
    name = this->db->escape_string(name);
    shared_ptr<MySQLResult> reader = this->db->select(TABLE,
                                                    NAME + " , " + VERSION,
                                                    NAME + " = ?",
                                                    "",
                                                    "",
                                                    "",
                                                    "",
                                                    [&](MySQLBind *bind)
                                                    {
                                                        bind->bind_param(name);
                                                    });
    if(reader != nullptr)
    {
        vector<_Tuple> list;
        ret = (reader->bind_param(&list) == 0);
        list.clear();
    }
    return ret;
}

uint64_t MySQLManagementDB::add(SQLManagement management)
{
    uint64_t ret = 0;
    string name = this->db->escape_string(management.getName());
    float version = management.getVersion();

    if(check(management.getName()))
    {
        ret = this->db->insert(TABLE,
                                NAME + " , " + VERSION,
                                "? , ?",
                                [&](MySQLBind *bind)
                                {
                                    bind->bind_param(name, version);
                                });
    }
    return ret;
}

uint64_t MySQLManagementDB::update(SQLManagement management)
{
    uint64_t ret = 0;
    string name = this->db->escape_string(management.getName());
    float version = management.getVersion();

    ret = this->db->update(TABLE,
                            VERSION + " = ?",
                            NAME + " = ?",
                            [&](MySQLBind *bind)
                            {
                                bind->bind_param(version, name);
                            });
    return ret;
}

uint64_t MySQLManagementDB::remove(string name)
{
    uint64_t ret = 0;
    name = this->db->escape_string(name);
    ret = this->db->remove(TABLE,
                            NAME + " = ?",
                            [&](MySQLBind *bind)
                            {
                                bind->bind_param(name);
                            });
    return ret;
}

SQLManagement MySQLManagementDB::get(string name)
{
    SQLManagement ret("", MySQLManagementDB::ERROR_VERSION);
    name = this->db->escape_string(name);
    shared_ptr<MySQLResult> reader = this->db->select(TABLE,
                                                    NAME + " , " + VERSION,
                                                    NAME + " = ?",
                                                    "",
                                                    "",
                                                    "",
                                                    "",
                                                    [&](MySQLBind *bind)
                                                    {
                                                        bind->bind_param(name);
                                                    });
    if(reader != nullptr)
    {
        vector<_Tuple> list;
        if(reader->bind_param(&list) == 1)
        {
            _Tuple tuple = list.at(0);
            ret.setName(std::get<0>(tuple));
            ret.setVersion(std::get<1>(tuple));
        }
        list.clear();
    }

    return ret;
}