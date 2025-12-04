//
// Created by 刘慧敏 on 2025/12/4.
//

#ifndef GREENHOUSEAPP_PERSON_H
#define GREENHOUSEAPP_PERSON_H
#include <string>
#include "sqlite_orm.h"
using namespace std;
struct Persons {
    int id=0;
    string username;
    string password;
};

class Person {
public:
    //构造函数，指定数据库文件路径
    explicit Person(const string& dbPath);
    //增加
    int insertPerson(const Persons& person);
    //修改
    bool updatePerson(int id,const string& newUsername,const string& newPassword);
    //删除用户
    bool deletePerson(int id);
private:
    using StorageType=decltype(sqlite_orm::make_storage(
    "",
    sqlite_orm::make_table("persons",
                           sqlite_orm::make_column("id",&Persons::id,sqlite_orm::primary_key().autoincrement()),
                               sqlite_orm::make_column("username",&Persons::username),
                               sqlite_orm::make_column("password",&Persons::password)
                           ))
    );
    StorageType storage;
};


#endif //GREENHOUSEAPP_PERSON_H