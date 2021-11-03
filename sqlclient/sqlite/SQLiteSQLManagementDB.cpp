#include "SQLiteSQLManagementDB.hpp"

using namespace ODBC;

bool SQLiteSQLManagementDB::MUTEX_IS_INIT = false;
bool SQLiteSQLManagementDB::IS_INIT = false;
pthread_mutex_t SQLiteSQLManagementDB::MUTEX;
list<SQLManagement> SQLiteSQLManagementDB::MANAGEMENTS;

typedef tuple<string, float> _Tuple;

SQLiteSQLManagementDB::SQLiteSQLManagementDB(string database_name, SQLError error)
                    :SQLManagementDB("", 0, "", "", database_name, error)
{
    this->db = new SQLiteSQLClient(database_name, error);
}

SQLiteSQLManagementDB::SQLiteSQLManagementDB(SQLiteSQLClient *db)
                    :SQLManagementDB(db)
{
}

SQLiteSQLManagementDB::~SQLiteSQLManagementDB()
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

void SQLiteSQLManagementDB::init()
{
    if(!SQLiteSQLManagementDB::MUTEX_IS_INIT)
    {
        SQLiteSQLManagementDB::MUTEX_IS_INIT = true;
        pthread_mutex_init(&SQLiteSQLManagementDB::MUTEX, nullptr);
    }

    pthread_mutex_lock(&SQLiteSQLManagementDB::MUTEX);
    if(!SQLiteSQLManagementDB::IS_INIT)
    {
        SQLiteSQLManagementDB::IS_INIT = true;

        this->db->create_table(TABLE, CREATE_TABLE);

        SQLiteSQLManagementDB::MANAGEMENTS.push_back(SQLManagement(TABLE, SQLiteSQLManagementDB::ERROR_VERSION));
    }

    SQLManagement information = table_info();
    list<SQLManagement>::iterator management = find(SQLiteSQLManagementDB::MANAGEMENTS.begin(), SQLiteSQLManagementDB::MANAGEMENTS.end(), information);
    
    if(management == SQLiteSQLManagementDB::MANAGEMENTS.end())
    {
        SQLManagement sqlManagement = get(information.getName());
        if(sqlManagement.getVersion() != SQLiteSQLManagementDB::ERROR_VERSION)
        {
            SQLiteSQLManagementDB::MANAGEMENTS.push_front(sqlManagement);
            management = find(SQLiteSQLManagementDB::MANAGEMENTS.begin(), SQLiteSQLManagementDB::MANAGEMENTS.end(), sqlManagement);
        }
    }

    if(management == SQLiteSQLManagementDB::MANAGEMENTS.end())
    {
        if(create_table())
        {
            add(information);
            SQLiteSQLManagementDB::MANAGEMENTS.push_front(information);
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

    pthread_mutex_unlock(&SQLiteSQLManagementDB::MUTEX);
}

bool SQLiteSQLManagementDB::check(string name)
{
    bool ret = true;
    name = this->db->escape_string(name);
    shared_ptr<SQLiteSQLResult> reader = this->db->select(TABLE,
                                                          NAME + " , " + VERSION,
                                                          NAME + " = ?",
                                                          "",
                                                          "",
                                                          "",
                                                          "",
                                                          [&](SQLiteSQLBind *bind)
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

uint64_t SQLiteSQLManagementDB::add(SQLManagement management)
{
    uint64_t ret = 0;
    string name = this->db->escape_string(management.getName());
    float version = management.getVersion();

    if(check(management.getName()))
    {
        ret = this->db->insert(TABLE,
                                NAME + " , " + VERSION,
                                "? , ?",
                                [&](SQLiteSQLBind *bind)
                                {
                                    bind->bind_param(name, version);
                                });
    }
    return ret;
}

uint64_t SQLiteSQLManagementDB::update(SQLManagement management)
{
    uint64_t ret = 0;
    string name = this->db->escape_string(management.getName());
    float version = management.getVersion();

    ret = this->db->update(TABLE,
                            VERSION + " = ?",
                            NAME + " = ?",
                            [&](SQLiteSQLBind *bind)
                            {
                                bind->bind_param(version, name);
                            });
    return ret;
}

uint64_t SQLiteSQLManagementDB::remove(string name)
{
    uint64_t ret = 0;
    name = this->db->escape_string(name);
    ret = this->db->remove(TABLE,
                            NAME + " = ?",
                            [&](SQLiteSQLBind *bind)
                            {
                                bind->bind_param(name);
                            });
    return ret;
}

SQLManagement SQLiteSQLManagementDB::get(string name)
{
    SQLManagement ret("", SQLiteSQLManagementDB::ERROR_VERSION);
    name = this->db->escape_string(name);
    shared_ptr<SQLiteSQLResult> reader = this->db->select(TABLE,
                                                          NAME + " , " + VERSION,
                                                          NAME + " = ?",
                                                          "",
                                                          "",
                                                          "",
                                                          "",
                                                          [&](SQLiteSQLBind *bind)
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