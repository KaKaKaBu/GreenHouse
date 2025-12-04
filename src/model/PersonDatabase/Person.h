// // Person.h
// #ifndef PERSON_H
// #define PERSON_H
//
// #include <string>
// #include "model/Database/Database.h"
//
// class Person {
// public:
//     // 所有函数都是 static，不需要创建 Person 对象！
//     static int insertPerson(const Persons& person);
//     static bool updatePerson(int id, const std::string& newUsername, const std::string& newPassword);
//     static bool deletePerson(int id);
//     static bool queryPerson(const std::string& username, const std::string& password); // 小写 q
// };
//
// #endif // PERSON_H

// model/Person/Person.h
#ifndef PERSON_H
#define PERSON_H

#include <string>
#include "model/Database/Database.h"
#include "model/SensorData.h"
class Person {
public:
    // // 注册：插入新用户（返回 true 表示成功）
    // static bool registerUser(const std::string& username, const std::string& password);
    //
    // // 登录：验证用户名+密码是否正确
    // static bool login(const std::string& username, const std::string& password);
    //
    // // 修改：更新指定用户的用户名和密码（需提供原用户名用于定位）
    // static bool updateUserInfo(
    //     const std::string& oldUsername,
    //     const std::string& newUsername,
    //     const std::string& newPassword
    // );
    static int registerUser(const Persons& person);
    static bool login(const std::string& username, const std::string& password);
    static bool updatePassword(
      const std::string& username,
      const std::string& newPassword
  );
};

#endif // PERSON_H