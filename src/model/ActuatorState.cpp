#include "ActuatorState.h"
#include <iostream>

ActuatorState::ActuatorState(const std::string &dbPath)
:storage(sqlite_orm::make_storage(dbPath,
        sqlite_orm::make_table("actuator_states",
            sqlite_orm::make_column("id",&State::id,sqlite_orm::primary_key().autoincrement()),
            sqlite_orm::make_column("fan",&State::fan),
            sqlite_orm::make_column("water_pump",&State::water_pump),
            sqlite_orm::make_column("light_bulb",&State::light_bulb),
            sqlite_orm::make_column("StartTime",&State::StartTime),
            sqlite_orm::make_column("EndTime",&State::EndTime)))){
    storage.sync_schema();
}

bool ActuatorState::insertState(const State &state) {
    try {
        storage.insert(state);
        return true;
    }catch (const std::exception& e) {
        std::cerr << "电机状态插入失败"<<e.what()<<std::endl;
        return false;
    }
}

std::vector<State> ActuatorState::QueryInRange(const std::string &startTime, const std::string &endTime) {
    try {
        storage.get_all<State>(
            sqlite_orm::where(
        sqlite_orm::c(&State::StartTime)>=startTime &&
                sqlite_orm::c(&State::StartTime)<=endTime),
                sqlite_orm::order_by(&State::StartTime)
        );

    }catch (const std::exception& e) {
        std::cerr << "查询电机状态失败"<<e.what()<<std::endl;
        return {};
    }
}

bool ActuatorState::deleteInRange(const std::string &startTime, const std::string &endTime) {
    try {
            storage.remove_all<State>(
                sqlite_orm::where(
                    sqlite_orm::c(&State::StartTime)<=endTime &&
                    startTime<=sqlite_orm::c(&State::EndTime)));
        return true;
    }catch (const std::exception& e) {
        std::cerr << "电机状态删除失败"<<e.what()<<std::endl;
        return false;
    }
}
