#ifndef DATABASE_H
#define DATABASE_H
#include <iostream>
#include <string>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <vector>

#include "sqlite_orm.h"
#include "model/SensorData.h"
using namespace sqlite_orm;

struct GreenData {
    int id=0;
    std::string record_time;   //采集数据时间
    int air_temp=0.0f; //空气温度
    int air_humid=0.0f;//  空气湿度
    int soil_humid=0.0f;//  土壤湿度
    int light_intensity=0.0f;//   光照强度
};
class Database {
public:
    static Database& instance(){
        static Database db;
        return db;
    }
    auto& getStorage(){return m_storage;}
    bool insert(const SensorRecord& data);//插入
    //查询指定时间范围的数据
    bool queryByTime(const std::string& startTime,const std::string& endTime,std::vector<SensorRecord>& outResults);
    bool deleteByTime(const std::string& startTime,const std::string&  endTime);
private:
    Database() {
        m_storage.sync_schema();
        std::cout<<"数据库初始化成功"<<std::endl;
    };
    ~Database()=default;
    //sqlite_orm的核心对象
    decltype(sqlite_orm::make_storage("green-house.db",
        sqlite_orm::make_table("green_data",
        sqlite_orm::make_column("id",&SensorRecord::id,sqlite_orm::primary_key().autoincrement()),
        sqlite_orm::make_column("record_time",&SensorRecord::record_time),
        sqlite_orm::make_column("air_temp",&SensorRecord::air_temp),
        sqlite_orm::make_column("air_humid",&SensorRecord::air_humid),
        sqlite_orm::make_column("soil_humid",&SensorRecord::soil_humid),
        sqlite_orm::make_column("light_intensity",&SensorRecord::light_intensity))))
    m_storage=sqlite_orm::make_storage(
        "green-house.db",
        sqlite_orm::make_table("green_data",
        sqlite_orm::make_column("id",&SensorRecord::id,sqlite_orm::primary_key().autoincrement()),
        sqlite_orm::make_column("record_time",&SensorRecord::record_time),
      sqlite_orm::make_column("air_temp",&SensorRecord::air_temp),
      sqlite_orm::make_column("air_humid",&SensorRecord::air_humid),
      sqlite_orm::make_column("soil_humid",&SensorRecord::soil_humid),
      sqlite_orm::make_column("light_intensity",&SensorRecord::light_intensity)));


};

#endif // DATABASE_H
