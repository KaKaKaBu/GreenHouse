#ifndef SENSORVIEWMODEL_H
#define SENSORVIEWMODEL_H
// viewmodel/SensorViewModel.h
#pragma once
#include <QObject>
#include "../model/SensorData.h"

class SensorViewModel : public QObject {
    Q_OBJECT

public:
    explicit SensorViewModel(QObject* parent = nullptr);

    /**
     * @brief 将下位机的 6 字节 payload 解析为 SensorRecord
     * @param payload 必须是 6 字节（[airHum, tmpH, tmpL, soilHum, lightH, lightL]）
     * @return 解析成功的 SensorRecord（时间由调用者设置）
     */
    static SensorRecord parseFromPayload(const QByteArray& payload);

    signals:
        void sensorDataUpdated(const SensorRecord& record); // 可用于通知 UI 更新
};

#endif // SENSORVIEWMODEL_H
