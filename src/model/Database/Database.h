#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "model/SensorData.h"
#include "sqlite_orm.h"

// 先声明 makeAppStorage
inline auto makeAppStorage(const std::string& filename);

class Database {
public:
    static Database& instance();

    bool insert(const SensorRecord& data);
    bool queryByTime(const std::string& start, const std::string& end, std::vector<SensorRecord>& out);
    bool deleteByTime(const std::string& start, const std::string& end);
    auto& getStorage() { return m_storage; }

private:
    Database();
    ~Database() = default;

    // ✅ 正确方式：使用 makeAppStorage 返回的具体类型
    using StorageType = decltype(makeAppStorage(std::string{}));
    StorageType m_storage;
};

// 现在定义 makeAppStorage（必须在 Database 类之后，因为用了 Persons 和 SensorRecord）
inline auto makeAppStorage(const std::string& filename) {
    using namespace sqlite_orm;
    return make_storage(
        filename,
        make_table("sensor_data",
            make_column("id", &SensorRecord::id, primary_key().autoincrement()),
            make_column("record_time", &SensorRecord::record_time),
            make_column("air_temp", &SensorRecord::air_temp),
            make_column("air_humid", &SensorRecord::air_humid),
            make_column("soil_humid", &SensorRecord::soil_humid),
            make_column("light_intensity", &SensorRecord::light_intensity)
        ),
        make_table("persons",
            make_column("id", &Persons::id, primary_key().autoincrement()),
            make_column("username", &Persons::username),
            make_column("password", &Persons::password)
        )
    );
}

#endif // DATABASE_H

// class Database {
// public:
//     static Database& instance(){
//         static Database db;
//         return db;
//     }
//     auto& getStorage(){return m_storage;}
//     bool insert(const SensorRecord& data);//插入
//     //查询指定时间范围的数据
//     bool queryByTime(const std::string& startTime,const std::string& endTime,std::vector<SensorRecord>& outResults);
//     bool deleteByTime(const std::string& startTime,const std::string&  endTime);
// private:
//     Database() {
//         m_storage.sync_schema();
//         std::cout<<"数据库初始化成功"<<std::endl;
//     };
//     ~Database()=default;
//     // //sqlite_orm的核心对象
//     // decltype(sqlite_orm::make_storage("green-house.db",
//     //     sqlite_orm::make_table("green_data",
//     //     sqlite_orm::make_column("id",&SensorRecord::id,sqlite_orm::primary_key().autoincrement()),
//     //     sqlite_orm::make_column("record_time",&SensorRecord::record_time),
//     //     sqlite_orm::make_column("air_temp",&SensorRecord::air_temp),
//     //     sqlite_orm::make_column("air_humid",&SensorRecord::air_humid),
//     //     sqlite_orm::make_column("soil_humid",&SensorRecord::soil_humid),
//     //     sqlite_orm::make_column("light_intensity",&SensorRecord::light_intensity))))
//     // m_storage=sqlite_orm::make_storage(
//     //     "green-house.db",
//     //     sqlite_orm::make_table("green_data",
//     //     sqlite_orm::make_column("id",&SensorRecord::id,sqlite_orm::primary_key().autoincrement()),
//     //     sqlite_orm::make_column("record_time",&SensorRecord::record_time),
//     //   sqlite_orm::make_column("air_temp",&SensorRecord::air_temp),
//     //   sqlite_orm::make_column("air_humid",&SensorRecord::air_humid),
//     //   sqlite_orm::make_column("soil_humid",&SensorRecord::soil_humid),
//     //   sqlite_orm::make_column("light_intensity",&SensorRecord::light_intensity)));
//
//
// };

//#endif // DATABASE_H
