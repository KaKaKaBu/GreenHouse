//
// Created by 刘慧敏 on 2025/12/4.
//

#include "Person.h"
#include <iostream>
Person::Person(const string &dbPath):storage(sqlite_orm::make_storage(dbPath,
    sqlite_orm::make_table("Persons",
        sqlite_orm::make_column("id",&Persons::id,
            sqlite_orm::primary_key().autoincrement()),
            sqlite_orm::make_column("username",&Persons::username),
            sqlite_orm::make_column("password",&Persons::password)))) {
    storage.sync_schema();
};

int Person::insertPerson(const ::Persons &person) {
    try {
        return static_cast<int>(storage.insert(person));
    }catch (const exception& e) {
        std::cerr << "用户插入信息失败"<<e.what()<<endl;
        return 0;
    }
}

bool Person::updatePerson(int id, const string &newUsername, const string &newPassword) {
    try {
        storage.update_all(
            sqlite_orm::set(
                sqlite_orm::assign(&Persons::username,newUsername),
                sqlite_orm::assign(&Persons::password,newPassword)),
                sqlite_orm::where(sqlite_orm::c(&Persons::id)==id)
                );
        return true;
    }catch (const exception& e) {
        cerr<<"用户更新信息失败"<<e.what()<<endl;
        return false;
    }
}

bool Person::deletePerson(int id) {
    try {
        storage.remove<Persons>(id);
        return true;
    }catch (const exception& e) {
        cerr<<"用户删除信息失败"<<e.what()<<endl;
        return false;
    }
}
