set(TS_FILES BusinessDashboard_de_DE.ts)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    sql_manager.cpp sql_manager.h
    kundenuebersicht.cpp kundenuebersicht.h kundenuebersicht.ui
    env_loader.cpp env_loader.h
    auftrag.h
    kundenHinzufuegenWindow.cpp kundenHinzufuegenWindow.h kundenHinzufuegenWindow.ui
    kundenbearbeitenwindow.cpp kundenbearbeitenwindow.h kundenbearbeitenwindow.ui
    ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BusinessDashboard MANUAL_FINALIZATION ${PROJECT_SOURCES}
        styleResources.qrc
        pluginManager.h pluginManager.cpp
        pythonPluginAdapter.h pythonPluginAdapter.cpp
        )
    qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(BusinessDashboard SHARED ${PROJECT_SOURCES})
    else()
        add_executable(BusinessDashboard ${PROJECT_SOURCES})
    endif()
    qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
endif()

target_include_directories(BusinessDashboard PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../interface)

# Verknüpfen
target_link_libraries(BusinessDashboard PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Sql
)

# Mac/Win Eigenschaften
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BusinessDashboard)
endif()
set_target_properties(BusinessDashboard PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

find_package(Python3 COMPONENTS Development REQUIRED)

target_sources(BusinessDashboard PRIVATE
    PythonBridge.cpp
    PythonBridge.h
    pythonPluginAdapter.cpp
    pythonPluginAdapter.h
)

target_include_directories(BusinessDashboard PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(BusinessDashboard PRIVATE Python3::Python)

# Install
include(GNUInstallDirs)
install(TARGETS BusinessDashboard
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BusinessDashboard)
endif()
