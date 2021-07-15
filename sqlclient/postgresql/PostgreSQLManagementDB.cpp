#include "PostgreSQLManagementDB.hpp"

using namespace ODBC;

bool PostgreManagementDB::MUTEX_IS_INIT = false;
bool PostgreManagementDB::IS_INIT = false;
pthread_mutex_t PostgreManagementDB::MUTEX;
list<SQLManagement> PostgreManagementDB::MANAGEMENTS;

typedef tuple<string, float> _Tuple;

PostgreManagementDB::PostgreManagementDB(string server_ip, uint16_t server_port, string user_name, string user_password, string database_name, SQLError error)
                    :SQLManagementDB(server_ip, server_port, user_name, user_password, database_name, error)
{
    this->db = new PostgreSQLClient(server_ip, server_port, user_name, user_password, database_name, error);
}

PostgreManagementDB::PostgreManagementDB(PostgreSQLClient *db)
                    :SQLManagementDB(db)
{
}

PostgreManagementDB::~PostgreManagementDB()
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

void PostgreManagementDB::init()
{
    if(!PostgreManagementDB::MUTEX_IS_INIT)
    {
        PostgreManagementDB::MUTEX_IS_INIT = true;
        pthread_mutex_init(&PostgreManagementDB::MUTEX, nullptr);
    }

    pthread_mutex_lock(&PostgreManagementDB::MUTEX);
    if(!PostgreManagementDB::IS_INIT)
    {
        PostgreManagementDB::IS_INIT = true;

        this->db->create_table(TABLE, CREATE_TABLE);

        PostgreManagementDB::MANAGEMENTS.push_back(SQLManagement(TABLE, PostgreManagementDB::ERROR_VERSION));
    }

    SQLManagement information = table_info();
    list<SQLManagement>::iterator management = find(PostgreManagementDB::MANAGEMENTS.begin(), PostgreManagementDB::MANAGEMENTS.end(), information);
    
    if(management == PostgreManagementDB::MANAGEMENTS.end())
    {
        SQLManagement sqlManagement = get(information.getName());

        if(sqlManagement.getVersion() != PostgreManagementDB::ERROR_VERSION)
        {
            PostgreManagementDB::MANAGEMENTS.push_front(sqlManagement);
            management = find(PostgreManagementDB::MANAGEMENTS.begin(), PostgreManagementDB::MANAGEMENTS.end(), sqlManagement);
        }
    }

    if(management == PostgreManagementDB::MANAGEMENTS.end())
    {
        if(create_table())
        {
            add(information);
            PostgreManagementDB::MANAGEMENTS.push_front(information);
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

    pthread_mutex_unlock(&PostgreManagementDB::MUTEX);
}

bool PostgreManagementDB::check(string name)
{
    bool ret = true;
    name = this->db->escape_string(name);
    shared_ptr<PostgreSQLResult> reader = this->db->select(TABLE,
                                                            NAME + " , " + VERSION,
                                                            NAME + " = $1",
                                                            "",
                                                            "",
                                                            "",
                                                            "",
                                                            [&](PostgreSQLBind *bind)
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

uint64_t PostgreManagementDB::add(SQLManagement management)
{
    uint64_t ret = 0;
    string name = this->db->escape_string(management.getName());
    float version = management.getVersion();

    if(check(management.getName()))
    {
        ret = this->db->insert(TABLE,
                                NAME + " , " + VERSION,
                                "$1 , $2",
                                [&](PostgreSQLBind *bind)
                                {
                                    bind->bind_param(name, version);
                                });
    }
    return ret;
}

uint64_t PostgreManagementDB::update(SQLManagement management)
{
    uint64_t ret = 0;
    string name = this->db->escape_string(management.getName());
    float version = management.getVersion();

    ret = this->db->update(TABLE,
                            VERSION + " = $1",
                            NAME + " = $2",
                            [&](PostgreSQLBind *bind)
                            {
                                bind->bind_param(version, name);
                            });
    return ret;
}

uint64_t PostgreManagementDB::remove(string name)
{
    uint64_t ret = 0;
    name = this->db->escape_string(name);
    ret = this->db->remove(TABLE,
                            NAME + " = $1",
                            [&](PostgreSQLBind *bind)
                            {
                                bind->bind_param(name);
                            });
    return ret;
}

SQLManagement PostgreManagementDB::get(string name)
{
    SQLManagement ret("", PostgreManagementDB::ERROR_VERSION);
    name = this->db->escape_string(name);
    shared_ptr<PostgreSQLResult> reader = this->db->select(TABLE,
                                                            NAME + " , " + VERSION,
                                                            NAME + " = $1",
                                                            "",
                                                            "",
                                                            "",
                                                            "",
                                                            [&](PostgreSQLBind *bind)
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