cmake_minimum_required(VERSION 3.16)

project(GreenhouseApp VERSION 1.0 LANGUAGES C CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_PREFIX_PATH "D:\\Qt\\qt-download\\5.15.0\\mingw81_64")

# Qt 自动化（仅对主程序生效）
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找 Qt5 模块
find_package(Qt5 COMPONENTS Core Gui Widgets Sql SerialPort Charts REQUIRED)

# 源文件
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
        src/widget/HistoryData/test.cpp
)

# UI 文件
set(FORMS
        src/widget/Login/login.ui
        src/widget/MainWindow/mainwindow.ui
)

# 创建 SQLite3 静态库（禁用Qt自动化工具）
add_library(sqlite3 STATIC
        third-party/sqlite3/sqlite3.c
)

# 设置 SQLite3 编译选项
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
    target_compile_definitions(sqlite3 PRIVATE SQLITE_API=)
endif()

set_target_properties(sqlite3 PROPERTIES
        AUTOMOC OFF          # 关键：禁用sqlite3的MOC
        AUTOUIC OFF          # 关键：禁用sqlite3的UIC
        AUTORCC OFF          # 关键：禁用sqlite3的RCC
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
        POSITION_INDEPENDENT_CODE ON
)

# SQLite3 包含目录
target_include_directories(sqlite3 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/sqlite3
)

# 添加可执行文件（移除HEADERS，不需要显式添加头文件）
add_executable(${PROJECT_NAME}
        ${SOURCES}
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

# 主程序包含头文件路径
target_include_directories(${PROJECT_NAME} PRIVATE
        src
        src/common
        src/model
        src/untils
        src/viewmodel
        src/widget
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/sqlite_orm
)
# 将sqlite3目录设为系统包含（解决<sqlite3.h>找不到的问题）
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/sqlite3
)