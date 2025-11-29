QT       += core gui sql serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/common/ProtocolParser.cpp \
    src/common/appconfig.cpp \
    src/main.cpp \
    src/model/Database/Database.cpp \
    src/untils/Log.cpp \
    src/viewmodel/ChartViewModel.cpp \
    src/viewmodel/ControlViewModel.cpp \
    src/viewmodel/SensorViewModel.cpp \
    src/viewmodel/SerialViewModel.cpp \
    src/viewmodel/SettingViewModel.cpp \
    src/widget/Login/login.cpp \
    src/widget/MainWindow/mainwindow.cpp \
    third-party/sqlite3/sqlite3.c

HEADERS += \
    src/common/Enum.h \
    src/common/Global.h \
    src/common/Protocol.h \
    src/common/ProtocolParser.h \
    src/common/Types.h \
    src/common/appconfig.h \
    src/model/ActuatorState.h \
    src/model/Database/Database.h \
    src/model/SensorData.h \
    src/model/UserSetting.h \
    src/untils/ByteArrayUtil.h \
    src/untils/CRC16.h \
    src/untils/Log.h \
    src/untils/SettingsUtil.h \
    src/untils/TimerUtil.h \
    src/viewmodel/ChartViewModel.h \
    src/viewmodel/ControlViewModel.h \
    src/viewmodel/SensorViewModel.h \
    src/viewmodel/SerialViewModel.h \
    src/viewmodel/SettingViewModel.h \
    src/widget/Login/login.h \
    src/widget/MainWindow/mainwindow.h \
    third-party/sqlite3/sqlite3.h \
    third-party/sqlite_orm/sqlite_orm.h

FORMS += \
    src/widget/Login/login.ui \
    src/widget/MainWindow/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
