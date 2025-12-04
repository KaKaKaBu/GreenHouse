// Database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

// 👇 只在此处定义数据结构（全局唯一！）
struct Persons {
    int id = 0;
    std::string username;
    std::string password;
};

struct GreenData {
    int id = 0;
    std::string record_time;
    int air_temp;
    int air_humid;
    int soil_humid;
    int light_intensity;
};

class Database {
public:
    // GreenData 表操作
    static bool insert(const GreenData &data);
    static bool queryByTime(const std::string &startTime, const std::string &endTime, std::vector<GreenData>& outResults);
    static bool deleteByTime(const std::string &startTime, const std::string &endTime);

    // Persons 表操作
    static int insertPerson(const Persons& person);
    static bool updatePerson(int id, const std::string& username, const std::string& password);
    static bool deletePerson(int id);
    static bool queryPerson(const std::string& username, const std::string& password);

private:
    // 私有：外部不可访问 storage
    static void* getStorageImpl(); // 返回 void*，内部强转
};

#endif // DATABASE_H