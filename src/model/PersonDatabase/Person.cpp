//
// Created by 刘慧敏 on 2025/12/4.
//
#include "Person.h"
#include <iostream>

Person::Person(const std::string &dbPath)
    : storage(sqlite_orm::make_storage(dbPath,
          sqlite_orm::make_table("Persons",
              sqlite_orm::make_column("id", &Persons::id,
                  sqlite_orm::primary_key().autoincrement()),
              sqlite_orm::make_column("username", &Persons::username),
              sqlite_orm::make_column("password", &Persons::password))))
{
    //storage.sync_schema(true);
    try {
        storage.sync_schema(); // 尝试同步
    } catch (const std::system_error& e) {
        std::string msg = e.what();
        if (msg.find("already exists") != std::string::npos) {
            // 安全忽略“表已存在”错误
            return;
        }
        throw; // 其他错误仍要报错
    }
}

bool Person::verifyUser(const std::string &username, const std::string &password){
    try {
        auto users = storage.get_all<Persons>(
            sqlite_orm::where(
                sqlite_orm::c(&Persons::username) == username &&
                sqlite_orm::c(&Persons::password) == password
            )
        );
        return !users.empty(); // 找到至少一个匹配用户即成功
    } catch (const std::exception& e) {
        std::cerr << "验证用户失败: " << e.what() << std::endl;
        return false;
    }
}

int Person::insertPerson(const std::string& username, const std::string& password) {
    // 仅修复：移除多余的::作用域（原有代码的::Persons会导致编译错误）
    auto count = storage.count<Persons>(sqlite_orm::where(sqlite_orm::c(&Persons::username) == username));
    if (count > 0) {
        return -1;  // 用户名已存在
    }

    // 构造新用户对象
    // 仅修复：移除多余的::作用域
    Persons newPerson;
    newPerson.username = username;  // 字段名与结构体一致
    newPerson.password = password;

    try {
        // 插入数据库
        auto id = storage.insert(newPerson);
        return static_cast<int>(id);  // 返回插入的 ID
    } catch (const std::exception& e) {
        std::cerr << "用户插入失败: " << e.what() << std::endl;
        return 0;  // 数据库错误
    }
}

bool Person::updatePerson(int id, const std::string &newUsername, const std::string &newPassword) {
    try {
        storage.update_all(
            sqlite_orm::set(
                sqlite_orm::assign(&Persons::username, newUsername),
                sqlite_orm::assign(&Persons::password, newPassword)),
            sqlite_orm::where(sqlite_orm::c(&Persons::id) == id));
        return true;
    } catch (const std::exception& e) {
        std::cerr << "用户更新信息失败" << e.what() << std::endl;
        return false;
    }
}

Persons Person::getPersonById(int id) {
    try {
        return storage.get<Persons>(id); // 直接通过主键查询
    } catch (const std::exception& e) {
        std::cerr << "查询用户失败: " << e.what() << std::endl;
        return {}; // 返回空对象
    }
}

bool Person::deletePerson(int id) {
    try {
        storage.remove<Persons>(id);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "用户删除信息失败" << e.what() << std::endl;
        return false;
    }
}

int Person::getUserIdByUsername(const std::string &username) {
    try {
        auto users = storage.get_all<Persons>(
            sqlite_orm::where(sqlite_orm::c(&Persons::username) == username)
        );
        if (!users.empty()) {
            return users.front().id;
        }
    } catch (const std::exception& e) {
        std::cerr << "getUserIdByUsername error: " << e.what() << std::endl;
    }
    return -1; // 未找到
}