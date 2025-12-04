// Person.cpp
#include "Person.h"
#include "model/Database/Database.h" // 只需要包含 Database.h

int Person::insertPerson(const Persons& person) {
    return Database::insertPerson(person); // ✅ 简单调用
}

bool Person::updatePerson(int id, const std::string& username, const std::string& password) {
    return Database::updatePerson(id, username, password);
}

bool Person::deletePerson(int id) {
    return Database::deletePerson(id);
}

bool Person::queryPerson(const std::string& username, const std::string& password) {
    return Database::queryPerson(username, password);
}