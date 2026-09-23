cmake_minimum_required(VERSION 3.19)
project(VoidTeX LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt_add_executable(VoidTeX
    WIN32 MACOSX_BUNDLE
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h

    src/editor.cpp
    src/editor.h
    src/latexhighlighter.cpp
    src/latexhighlighter.h

    src/thememanager.cpp
    src/thememanager.h
    src/settings.h
    resources/app.qrc
)

target_include_directories(VoidTeX PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(VoidTeX
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS VoidTeX
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET VoidTeX
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
