------

# 🌱 **Embedded Greenhouse System – README（上位机项目）**

基于 **Qt5 + MVVM + sqlite_orm + Arduino（串口通信）** 的温室大棚监控系统。

本 README 用于说明项目整体架构、文件目录功能、通信协议与关键示例代码，让开发者快速上手项目结构。

------

# 📁 **项目结构**

```
/project-root
│
├── common/
│   ├── Protocol.h
│   ├── ProtocolParser.h
│   ├── ProtocolParser.cpp
│   ├── AppConfig.h
│   ├── AppConfig.cpp
│   ├── Types.h
│   ├── Enums.h
│   └── Global.h
│
├── model/
│   ├── SensorData.h
│   ├── ActuatorState.h
│   ├── UserSetting.h
│   └── database/
│       ├── Database.h
│       └── Database.cpp
│
├── viewmodel/
│   ├── SerialViewModel.h
│   ├── SerialViewModel.cpp
│   ├── SensorViewModel.h
│   ├── SensorViewModel.cpp
│   ├── ControlViewModel.h
│   ├── ControlViewModel.cpp
│   ├── ChartViewModel.h
│   └── ChartViewModel.cpp
│
├── widget/
│   ├── MainWindow.ui
│   ├── MainWindow.cpp
│   ├── SerialWidget.ui
│   ├── SerialWidget.cpp
│   ├── SensorPanel.ui
│   ├── SensorPanel.cpp
│   ├── ControlPanel.ui
│   ├── ControlPanel.cpp
│   ├── ChartWidget.ui
│   └── ChartWidget.cpp
│
└── utils/
    ├── Log.h
    ├── Log.cpp
    ├── ByteArrayUtil.h
    ├── TimerUtil.h
    ├── SettingsUtil.h
    └── CRC16.h
```

------

# 📦 **目录与文件详细说明**

------

# 🟦 **1. common — 通用基础模块**

这里放置全局都需要引用的内容，**不允许依赖 UI 或数据库**。

------

## **1.1 Protocol.h — 通信协议常量与指令号**

存放下位机与上位机约定的消息类型、帧头常量等。

**示例：**

```cpp
namespace Protocol {
    const uint8_t STX = 0xAA;

    enum MsgType : uint8_t {
        MSG_SENSOR   = 0x01,
        MSG_CONTROL  = 0x02,
        MSG_SETTING  = 0x03,
        MSG_HEARTBEAT = 0x04
    };
}
```

------

## **1.2 ProtocolParser.h / .cpp — 串口帧解析器**

负责：

✔ 检查帧头
 ✔ 长度验证
 ✔ CRC 校验
 ✔ 字节流 → 数据结构
 ✔ 数据结构 → 发送帧

**示例：解析传感器帧**

```cpp
SensorFrame ProtocolParser::parseSensorFrame(const QByteArray &bytes) {
    SensorFrame f;
    f.temperature = qFromLittleEndian<float>((uchar*)bytes.constData() + 3);
    f.humidity = qFromLittleEndian<float>((uchar*)bytes.constData() + 7);
    f.soil = qFromLittleEndian<float>((uchar*)bytes.constData() + 11);
    return f;
}
```

------

## **1.3 AppConfig — 全局配置类**

提供程序启动时加载的配置：

- 串口参数
- 自动重连
- 图表刷新速率

------

## **1.4 Types.h / Enums.h**

存放通用结构体、枚举类型。

------

## **1.5 Global.h**

存放全局单例、全局信号（如 EventBus）等。

------

------

# 🟩 **2. model — 数据模型层（与数据库绑定）**

此层只描述数据，不写任何业务逻辑。

------

## **2.1 SensorData.h — 传感器数据表**

```cpp
struct SensorData {
    int id;
    double temperature;
    double humidity;
    double soil;
    int64_t timestamp;
};
```

------

## **2.2 ActuatorState.h — 执行器状态记录**

如风扇、水泵、补光灯。

------

## **2.3 UserSetting.h — 用户配置**

如自动控制阈值：

```cpp
struct UserSetting {
    int id;
    double tempMin;
    double tempMax;
    double humidityMin;
    double humidityMax;
};
```

------

## **2.4 database/Database.h / .cpp — sqlite_orm 封装**

使用 sqlite_orm 构建数据库表与 CRUD：

```cpp
inline auto createStorage() {
    using namespace sqlite_orm;

    return make_storage("greenhouse.db",
        make_table("sensor",
            make_column("id", &SensorData::id, autoincrement(), primary_key()),
            make_column("temp", &SensorData::temperature),
            make_column("hum", &SensorData::humidity),
            make_column("soil", &SensorData::soil),
            make_column("ts", &SensorData::timestamp)
        )
    );
}
```

------

# 🟨 **3. viewmodel — MVVM 的核心业务逻辑**

负责处理：

✔ 串口收发
 ✔ 数据校验
 ✔ 写入数据库
 ✔ 将数据分发给 UI
 ✔ 封装控制指令

UI 不直接访问 Model 或串口。

------

## **3.1 SerialViewModel — 串口与协议桥梁**

- 打开/关闭串口
- 调用 ProtocolParser 解析收到的帧
- 转发为 Qt 信号

```cpp
emit sigSensorReceived(frame);
```

------

## **3.2 SensorViewModel — 传感器数据逻辑**

- 接收 SerialViewModel 的数据
- 更新 UI
- 写入 SQLite（model/database）

```cpp
connect(serial, &SerialViewModel::sigSensorReceived,
        this, &SensorViewModel::onSensorUpdated);
```

------

## **3.3 ControlViewModel — 发送控制指令**

- 发送开/关风扇
- 自动控制逻辑
- 保存用户设置

```cpp
sendControlFrame(Protocol::CTRL_FAN, true);
```

------

## **3.4 ChartViewModel — 历史图表数据提供者**

从数据库读取时间区间数据并转为 UI 可绘制的序列。

------

# 🟥 **4. widget — 全部 UI（只负责展示，不含逻辑）**

每个 UI 页面都只操作 ViewModel，不直接访问串口或数据库。

------

## **4.1 MainWindow**

负责管理整个界面

------

## **4.2 SerialWidget**

用于选择串口、显示连接状态。

------

## **4.3 SensorPanel**

显示实时温湿度、土壤湿度。

------

## **4.4 ControlPanel**

按钮控制水泵/风扇。

------

## **4.5 ChartWidget**

绘制历史曲线图。

------

# 🟧 **5. utils — 工具类**

实用工具，不属于 MVVM 任何一层。

------

## **5.1 Log — 日志系统**

```cpp
Log::i("Serial connected");
```

------

## **5.2 ByteArrayUtil — 字节数组格式化工具**

------

## **5.3 CRC16 — 校验工具**

------

## **5.4 SettingsUtil — QSettings 封装**

------

## **5.5 TimerUtil — QTimer 简易封装**

------

# 📡 **通信协议示例**

**上位机 → 下位机 开风扇**

```
AA 02 01 01 CRC
```

**下位机 → 上位机 发送传感器数据**

```
AA 01 0C [温度4字节] [湿度4字节] [土壤湿度4字节] CRC
```

------

# 🚀 **如何启动项目**

1. 编译 Qt 工程
2. 连接 Arduino
3. 打开软件 → 选择串口 → 点击连接
4. 自动显示实时数据
5. 控制执行器，查看历史日志与图表

