//
// Created by 刘慧敏 on 2025/12/4.
//

#ifndef GREENHOUSEAPP_PERSON_H
#define GREENHOUSEAPP_PERSON_H
#include "model/SensorData.h"
#include <string>
#include "sqlite_orm.h"

using namespace std;


class Person {
public:
    // 构造函数，指定数据库文件路径
    explicit Person(const string& dbPath);
    //验证密码和账号
    bool verifyUser(const std::string& username, const std::string& password);
    // 增加
    int insertPerson(const std::string& username, const std::string& password);

    // 修改
    bool updatePerson(int id, const string& newUsername, const string& newPassword);

    // 查询用户信息（返回 Persons 对象，若不存在则返回空对象）
    Persons getPersonById(int id);

    // 删除用户
    bool deletePerson(int id);

private:
    using StorageType = decltype(sqlite_orm::make_storage(
        "",
        sqlite_orm::make_table("persons",
            sqlite_orm::make_column("id", &Persons::id, sqlite_orm::primary_key().autoincrement()),
            sqlite_orm::make_column("username", &Persons::username),
            sqlite_orm::make_column("password", &Persons::password)
        )
    ));
    StorageType storage;
};

#endif // GREENHOUSEAPP_PERSON_H