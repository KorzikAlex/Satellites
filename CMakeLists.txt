cmake_minimum_required(VERSION 3.16)

project(Satellites VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Network Svg Gui)
include_directories("include" , "src")

set(PROJECT_SOURCES
    src/main.cpp
    src/UI/MainWindow.cpp
    src/UI/MainWindow.hpp
    src/UI/MainWindow.ui
    src/Utils/TleParser.hpp
    src/Utils/TleParser.cpp
    src/Utils/TleRecord.hpp
    src/Utils/TleStatistics.hpp
    resources/icons/window.qrc
    resources/icons/icon.rc
    src/UI/InfoWindow.cpp
    src/UI/InfoWindow.hpp
    src/UI/InfoWindow.ui
)
# Отключаем сборку shared-библиотек для всех
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Satellites
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Путь к UCRT64-MSYS2 Qt6
set(CMAKE_PREFIX_PATH "C:/msys64/ucrt64/qt6-static" CACHE PATH "Qt6 install prefix")

# Опции линковщика для полного статик‑линка
if(MINGW)
    # статические либы рантайма libgcc/libstdc++
    target_link_options(Satellites PRIVATE -static -static-libgcc -static-libstdc++)
endif()

# qt_add_translations(Satellites TS_FILES Satellites_en_US.ts)
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Satellites APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else ()
    if (ANDROID)
        add_library(Satellites SHARED
            ${PROJECT_SOURCES}
        )
    # Define properties for Android with Qt 5 after find_package() calls as:
    #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
else ()
    add_executable(Satellites
        ${PROJECT_SOURCES}
    )
endif ()
endif ()

target_link_libraries(Satellites PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Gui)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if (${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Satellites)
endif ()
set_target_properties(Satellites PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Satellites
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Satellites)
endif ()


find_package(Doxygen)


