// Person.h
#ifndef PERSON_H
#define PERSON_H

#include <string>
#include "model/Database/Database.h"

class Person {
public:
    // 所有函数都是 static，不需要创建 Person 对象！
    static int insertPerson(const Persons& person);
    static bool updatePerson(int id, const std::string& newUsername, const std::string& newPassword);
    static bool deletePerson(int id);
    static bool queryPerson(const std::string& username, const std::string& password); // 小写 q
};

#endif // PERSON_H