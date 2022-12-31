cmake_minimum_required(VERSION 3.5)

project(gta5cheat_qt VERSION 0.1 LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(QT_VERSION_MAJOR 6)
set(CMAKE_PREFIX_PATH "e:/Qt/6.3.2/mingw_64") # Qt Kit Dir
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -s")
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

set(PROJECT_SOURCES
    src/main.cpp
    src/app.ui
    src/app.h
    src/app.cpp
    src/staticdata.h
    src/staticdata.cpp
    src/gtav.h
    src/gtav.cpp
    src/res.rc
    src/memorycontrol.h
    src/offset.h
    src/memorycontrol.cpp
    src/work.h
    src/work.cpp
    src/keyboard.h
    src/keyboard.cpp
    resource/resource.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(gta5cheat_qt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET gta5cheat_qt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(gta5cheat_qt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(gta5cheat_qt
            ${PROJECT_SOURCES}
        )
        endif()
endif()

target_link_libraries(gta5cheat_qt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)
set_target_properties(gta5cheat_qt PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER ssyatelandisi.github.io
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS gta5cheat_qt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(gta5cheat_qt)
endif()
# add_custom_command(TARGET gta5cheat_qt POST_BUILD COMMAND strip ${PROJECT_NAME}.exe COMMAND upx ${PROJECT_NAME}.exe)
