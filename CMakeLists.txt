cmake_minimum_required(VERSION 3.19)
project(IRKdebug LANGUAGES CXX)

set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Svg Network)
find_package(KDDockWidgets-qt6 REQUIRED)
find_package(qlementine REQUIRED)

qt_standard_project_setup()

qt_add_executable(IRKdebug
    WIN32 MACOSX_BUNDLE
    main.cpp
    terminalwidget.h terminalwidget.cpp terminalwidget.ui
    IRKdebug.qrc
    connectionmanager.h connectionmanager.cpp
    irkviewfactory.h irkviewfactory.cpp
    irkerror.h
)

target_link_libraries(IRKdebug
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Svg
        Qt6::Network
        KDAB::kddockwidgets
        qlementine
)

add_compile_options(-Wno-padded)

include(GNUInstallDirs)

install(TARGETS IRKdebug
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET IRKdebug
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
