// Database.cpp
#include "Database.h"
#include "sqlite_orm.h" // 必须在此包含！
#include <iostream>
#include <vector>
#include <QDebug>

// 👇 在 .cpp 中统一构造 storage —— 确保类型唯一
namespace {
    auto makeAppStorage() {
        return sqlite_orm::make_storage(
            "green-house.db",
            sqlite_orm::make_table("green_data",
                sqlite_orm::make_column("id", &GreenData::id, sqlite_orm::primary_key().autoincrement()),
                sqlite_orm::make_column("record_time", &GreenData::record_time),
                sqlite_orm::make_column("air_temp", &GreenData::air_temp),
                sqlite_orm::make_column("air_humid", &GreenData::air_humid),
                sqlite_orm::make_column("soil_humid", &GreenData::soil_humid),
                sqlite_orm::make_column("light_intensity", &GreenData::light_intensity)
            ),
            sqlite_orm::make_table("persons",
                sqlite_orm::make_column("id", &Persons::id, sqlite_orm::primary_key().autoincrement()),
                sqlite_orm::make_column("username", &Persons::username),
                sqlite_orm::make_column("password", &Persons::password)
            )
        );
    }

    using StorageType = decltype(makeAppStorage());

    StorageType& getGlobalStorage() {
        static StorageType* s_storage = nullptr;
        static bool inited = false;
        if (!inited) {
            s_storage = new StorageType(makeAppStorage()); // 永不 delete
            s_storage->sync_schema();
            std::cout << "数据库初始化成功" << std::endl;
            inited = true;
        }
        return *s_storage;
    }
} // anonymous namespace

// 实现接口函数
bool Database::insert(const GreenData &data) {
    try {
        auto& db = getGlobalStorage();
        db.insert(data);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "插入 GreenData 失败: " << e.what() << std::endl;
        return false;
    }
}

bool Database::queryByTime(const std::string &startTime, const std::string &endTime, std::vector<GreenData>& outResults) {
    try {
        auto& db = getGlobalStorage();
        outResults = db.get_all<GreenData>(
            sqlite_orm::where(
                sqlite_orm::between(&GreenData::record_time, startTime, endTime)
            )
        );
        return true;
    } catch (const std::exception& e) {
        qDebug() << "查询 GreenData 异常:" << e.what();
        outResults.clear();
        return false;
    }
}

bool Database::deleteByTime(const std::string &startTime, const std::string &endTime) {
    try {
        auto& db = getGlobalStorage();
        db.remove_all<GreenData>(
            sqlite_orm::where(
                sqlite_orm::between(&GreenData::record_time, startTime, endTime)
            )
        );
        return true;
    } catch (const std::exception& e) {
        std::cerr << "删除 GreenData 失败: " << e.what() << std::endl;
        return false;
    }
}

int Database::insertPerson(const Persons& person) {
    try {
        auto& db = getGlobalStorage();
        return db.insert(person);
    } catch (const std::exception& e) {
        std::cerr << "插入用户失败: " << e.what() << std::endl;
        return -1;
    }
}

bool Database::updatePerson(int id, const std::string& username, const std::string& password) {
    try {
        auto& db = getGlobalStorage();
        auto personOpt = db.get_pointer<Persons>(id);
        if (!personOpt) return false;
        personOpt->username = username;
        personOpt->password = password;
        db.update(*personOpt);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "更新用户失败: " << e.what() << std::endl;
        return false;
    }
}

bool Database::deletePerson(int id) {
    try {
        auto& db = getGlobalStorage();
        db.remove<Persons>(id); // 即使返回 void，不抛异常即成功
        return true;
    } catch (const std::exception& e) {
        std::cerr << "删除用户失败: " << e.what() << std::endl;
        return false;
    }
}

bool Database::queryPerson(const std::string& username, const std::string& password) {
    try {
        auto& db = getGlobalStorage();
        auto persons = db.get_all<Persons>(
            sqlite_orm::where(
                sqlite_orm::eq(&Persons::username, username) &&
                sqlite_orm::eq(&Persons::password, password)
            )
        );
        return !persons.empty();
    } catch (const std::exception& e) {
        std::cerr << "查询用户失败: " << e.what() << std::endl;
        return false;
    }
}