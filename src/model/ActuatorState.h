#ifndef ACTUATORSTATE_H
#define ACTUATORSTATE_H
#include <string>

#include "sqlite_orm.h"

//0表示关，1表示开
struct State {
    int id=0;
    int fan=0;//风扇
    int water_pump=0;//水泵
    int light_bulb=0;//灯泡
    std::string StartTime;
    std::string EndTime;
};
class ActuatorState {
public:
    explicit ActuatorState(const std::string& dbPath);
    //增加
    bool insertState(const State& state);
    //查询
    std::vector<State> QueryInRange(const std::string& startTime,const std::string& endTime);
    //删除
    bool deleteInRange(const std::string& startTime,const std::string& endTime);
private:
    using StorageType=decltype(sqlite_orm::make_storage(
        "",
        sqlite_orm::make_table("actuator_states",
            sqlite_orm::make_column("id",&State::id,sqlite_orm::primary_key().autoincrement()),
            sqlite_orm::make_column("fan",&State::fan),
            sqlite_orm::make_column("water_pump",&State::water_pump),
            sqlite_orm::make_column("light_bulb",&State::light_bulb),
            sqlite_orm::make_column("StartTime",&State::StartTime),
            sqlite_orm::make_column("EndTime",&State::EndTime)
            )));
    StorageType storage;
};
#endif // ACTUATORSTATE_H
