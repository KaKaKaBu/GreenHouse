cmake_minimum_required(VERSION 3.30)
set(APP_NAME GreenHouseApp)
project(${APP_NAME})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(PROJECT_VERSION 1.0)

set(CMAKE_PREFIX_PATH "D:/ProgramFile/qt/5.15.2/mingw81_64")


find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        SerialPort
        Charts
        Sql
        REQUIRED)

#----- 源文件收集 -----
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.ui"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.qrc")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SRC_FILES})

#----- 可执行目标 -----
add_executable(${APP_NAME} ${SRC_FILES}
        src/model/PersonDatabase/Person.cpp
        src/model/PersonDatabase/Person.h
        src/model/ActuatorState.cpp)
target_link_libraries(${APP_NAME} PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::SerialPort
        Qt5::Charts
        Qt5::Sql
        sqlite3
)
target_include_directories(${APP_NAME}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_BINARY_DIR}"
)

set_target_properties(${APP_NAME} PROPERTIES
        OUTPUT_NAME_DEBUG "${APP_NAME}_${PROJECT_VERSION}_debug"
        OUTPUT_NAME_RELEASE "${APP_NAME}_${PROJECT_VERSION}_release"
        OUTPUT_NAME_RELWITHDEBINFO "${APP_NAME}_${PROJECT_VERSION}_release"
        OUTPUT_NAME_MINSIZEREL "${APP_NAME}_${PROJECT_VERSION}_release"
)

set_target_properties(${APP_NAME} PROPERTIES
        WIN32_EXECUTABLE $<CONFIG:Release>
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
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

target_include_directories(${APP_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/sqlite3
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/qt_toast
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/sqlite_orm
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#-----------------------------------------------------------
# 自动复制 AUTOUIC 生成的 ui_xxxxx.h 到 include 目录
#-----------------------------------------------------------
set(UIC_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}_autogen/include")

add_custom_command(
        TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CMAKE_SOURCE_DIR}/include"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${UIC_OUTPUT_DIR}"
        "${CMAKE_SOURCE_DIR}/include"
        COMMENT "Copying generated ui_*.h files to include/"
)

# --- qt_toast 组件库 ---
add_library(qttoast STATIC
        third-party/qt_toast/Toast.cpp

)

target_include_directories(qttoast PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/qt_toast
)

target_link_libraries(qttoast PUBLIC Qt5::Widgets Qt5::Gui)

# 主程序链接
target_link_libraries(${APP_NAME} PRIVATE qttoast)









