cmake_minimum_required(VERSION 3.16)

project(GreenhouseApp VERSION 1.0 LANGUAGES C CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_PREFIX_PATH "D:\\ProgramFile\\qt\\5.15.2\\mingw81_64")
# Qt 自动化
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找 Qt5 模块
find_package(Qt5 COMPONENTS Core Gui Widgets Sql SerialPort Charts REQUIRED)

# SQLite3 编译定义 - 修复这里的问题
# 移除之前的 add_definitions，改用 target_compile_definitions

# 源文件（移除 sqlite3.c，我们将单独编译它）
set(SOURCES
        src/main.cpp
        src/common/ProtocolParser.cpp
        src/common/appconfig.cpp
        src/model/Database/Database.cpp
        src/untils/Log.cpp
        src/viewmodel/ChartViewModel.cpp
        src/viewmodel/ControlViewModel.cpp
        src/viewmodel/SensorViewModel.cpp
        src/viewmodel/SerialViewModel.cpp
        src/viewmodel/SettingViewModel.cpp
        src/widget/Login/login.cpp
        src/widget/MainWindow/mainwindow.cpp
)

# 头文件
set(HEADERS
        src/common/Enum.h
        src/common/Global.h
        src/common/Protocol.h
        src/common/ProtocolParser.h
        src/common/Types.h
        src/common/appconfig.h
        src/model/ActuatorState.h
        src/model/Database/Database.h
        src/model/SensorData.h
        src/model/UserSetting.h
        src/untils/ByteArrayUtil.h
        src/untils/CRC16.h
        src/untils/Log.h
        src/untils/SettingsUtil.h
        src/untils/TimerUtil.h
        src/viewmodel/ChartViewModel.h
        src/viewmodel/ControlViewModel.h
        src/viewmodel/SensorViewModel.h
        src/viewmodel/SerialViewModel.h
        src/viewmodel/SettingViewModel.h
        src/widget/Login/login.h
        src/widget/MainWindow/mainwindow.h
        third-party/sqlite3/sqlite3.h
        third-party/sqlite_orm/sqlite_orm.h
)

# UI 文件
set(FORMS
        src/widget/Login/login.ui
        src/widget/MainWindow/mainwindow.ui
)

# 创建 SQLite3 静态库
add_library(sqlite3 STATIC
        third-party/sqlite3/sqlite3.c
)

# 设置 SQLite3 编译选项 - 使用正确的语法
target_compile_definitions(sqlite3 PRIVATE
        SQLITE_ENABLE_COLUMN_METADATA
        SQLITE_ENABLE_FTS3
        SQLITE_ENABLE_FTS4
        SQLITE_ENABLE_JSON1
        SQLITE_ENABLE_RTREE
        SQLITE_THREADSAFE=1
)

# 在 Windows 上设置 SQLITE_API 宏
if(WIN32)
    # 对于静态库，通常不需要 dllexport，设为空即可
    target_compile_definitions(sqlite3 PRIVATE SQLITE_API=)
endif()

set_target_properties(sqlite3 PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
        POSITION_INDEPENDENT_CODE ON
)

# 包含目录
target_include_directories(sqlite3 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/sqlite3
)

# 添加可执行文件
add_executable(${PROJECT_NAME}
        ${SOURCES}
        ${HEADERS}
        ${FORMS}
)

# 链接 Qt5 模块和 SQLite3
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Sql
        Qt5::SerialPort
        Qt5::Charts
        sqlite3
)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 包含头文件路径
target_include_directories(${PROJECT_NAME} PRIVATE
        src
        src/common
        src/model
        src/untils
        src/viewmodel
        src/widget
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/sqlite3
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/sqlite_orm
)

# 自动处理 UI 文件
qt5_wrap_ui(UI_HEADERS ${FORMS})
target_sources(${PROJECT_NAME} PRIVATE ${UI_HEADERS})