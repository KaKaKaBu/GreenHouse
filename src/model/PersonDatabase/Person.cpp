// // Person.cpp
// #include "Person.h"
// #include "model/Database/Database.h" // 只需要包含 Database.h
//
// int Person::insertPerson(const Persons& person) {
//     return Database::insertPerson(person); // ✅ 简单调用
// }
//
// bool Person::updatePerson(int id, const std::string& username, const std::string& password) {
//     return Database::updatePerson(id, username, password);
// }
//
// bool Person::deletePerson(int id) {
//     return Database::deletePerson(id);
// }
//
// bool Person::queryPerson(const std::string& username, const std::string& password) {
//     return Database::queryPerson(username, password);
// }

/// model/Person/Person.cpp
#include "Person.h"
#include "model/Database/Database.h"
#include "model/SensorData.h"  // 👈 关键！否则 Persons 不可见
#include <iostream>

using namespace sqlite_orm;

int Person::registerUser(const Persons& person) {
    if (person.username.empty() || person.password.empty()) {
        return -1;
    }

    try {
        auto& storage = Database::instance().getStorage();
        auto existing = storage.get_all<Persons>(
            where(c(&Persons::username) == person.username)
        );
        if (!existing.empty()) {
            return -1;
        }
        int64_t id = storage.insert(person);
        return static_cast<int>(id);
    } catch (...) {
        return -1;
    }
}

bool Person::login(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) {
        return false;
    }
    try {
        auto& storage = Database::instance().getStorage();
        auto users = storage.get_all<Persons>(
            where(c(&Persons::username) == username && c(&Persons::password) == password)
        );
        return !users.empty();
    } catch (...) {
        return false;
    }
}

bool Person::updatePassword(const std::string& username, const std::string& newPassword) {
    if (username.empty() || newPassword.empty()) {
        return false;
    }

    try {
        auto& storage = Database::instance().getStorage();
        auto users = storage.get_all<Persons>(
            where(c(&Persons::username) == username)
        );

        if (users.empty()) {
            return false;
        }

        Persons user = users[0];  // 👈 改为值拷贝（避免引用失效）
        user.password = newPassword;
        storage.update(user);     // 更新到数据库

        return true;
    } catch (...) {
        return false;
    }
}