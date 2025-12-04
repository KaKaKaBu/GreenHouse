#include "Database.h"
#include <iostream>

Database& Database::instance() {
    static Database inst;
    return inst;
}

Database::Database()
    : m_storage(makeAppStorage("greenhouse.db")) {
    m_storage.sync_schema();
}
bool Database::insert(const SensorRecord& data) {
    try {
        m_storage.insert(data);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "插入失败: " << e.what() << std::endl;
        return false;
    }
}

bool Database::queryByTime(const std::string& start, const std::string& end, std::vector<SensorRecord>& out) {
    try {
        out = m_storage.get_all<SensorRecord>(
            sqlite_orm::where(sqlite_orm::between(&SensorRecord::record_time, start, end))
        );
        return true;
    } catch (const std::exception& e) {
        std::cerr << "查询失败: " << e.what() << std::endl;
        return false;
    }
}

bool Database::deleteByTime(const std::string& start, const std::string& end) {
    try {

        m_storage.remove_all<SensorRecord>(
            sqlite_orm::where(sqlite_orm::between(&SensorRecord::record_time, start, end))
        );
        return true;
    } catch (const std::exception& e) {
        std::cerr << "删除失败: " << e.what() << std::endl;
        return false;
    }
}