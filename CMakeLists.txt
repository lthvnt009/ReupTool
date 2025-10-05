# Vị trí: /CMakeLists.txt
# Phiên bản: 5.1 (Sửa triệt để lỗi Build do gõ nhầm tên file)

cmake_minimum_required(VERSION 3.16)

project(ReupTool LANGUAGES CXX)

# Thêm dòng này để báo cho QXlsx biết phiên bản Qt
set(QT_VERSION_MAJOR 6)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS 
    ${CMAKE_CURRENT_SOURCE_DIR}/ui 
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/widgets/settings
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/dialogs
)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql Network Concurrent)

# Tích hợp thư viện QXlsx
add_subdirectory(third-party/QXlsx)

add_executable(${PROJECT_NAME} WIN32
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    ui/mainwindow.ui
    src/addchanneldialog.h
    src/addchanneldialog.cpp
    ui/addchanneldialog.ui
    src/appsettings.h
    src/appsettings.cpp
    
    # Models
    src/models.h
    src/models/FilterCriteria.h
    src/models/downloadqueuemodel.h
    src/models/downloadqueuemodel.cpp
    src/models/channelmodel.h
    src/models/channelmodel.cpp
    src/models/videolibrarymodel.h
    src/models/videolibrarymodel.cpp
    src/models/apikeymodel.h
    src/models/apikeymodel.cpp
    src/models/proxymodel.h
    src/models/proxymodel.cpp

    # Repositories (Interfaces)
    src/repositories/ichannelrepository.h
    src/repositories/ivideorepository.h

    # Repositories (Implementations)
    src/repositories/channelrepository.h
    src/repositories/channelrepository.cpp
    src/repositories/videorepository.h
    src/repositories/videorepository.cpp

    # Core
    src/databasemanager.h
    src/databasemanager.cpp

    # Delegates
    src/delegates/libraryitemdelegate.h
    src/delegates/libraryitemdelegate.cpp

    # Widgets
    src/widgets/channelmanagerwidget.h
    src/widgets/channelmanagerwidget.cpp
    ui/widgets/channelmanagerwidget.ui
    src/widgets/channeltoolbarwidget.h
    src/widgets/channeltoolbarwidget.cpp
    ui/widgets/channeltoolbarwidget.ui
    src/widgets/channellistwidget.h
    src/widgets/channellistwidget.cpp
    ui/widgets/channellistwidget.ui
    src/widgets/channelstatisticswidget.h
    src/widgets/channelstatisticswidget.cpp
    ui/widgets/channelstatisticswidget.ui
    src/widgets/datainputwidget.h
    src/widgets/datainputwidget.cpp
    ui/widgets/datainputwidget.ui
    src/widgets/datainputtoolbarwidget.h
    src/widgets/datainputtoolbarwidget.cpp
    ui/widgets/datainputtoolbarwidget.ui
    src/widgets/downloadwidget.h
    src/widgets/downloadwidget.cpp
    ui/widgets/downloadwidget.ui
    src/widgets/downloadtoolbarwidget.h
    src/widgets/downloadtoolbarwidget.cpp
    ui/widgets/downloadtoolbarwidget.ui
    src/widgets/downloadcontrolbarwidget.h
    src/widgets/downloadcontrolbarwidget.cpp
    ui/widgets/downloadcontrolbarwidget.ui
    src/widgets/downloadoptionswidget.h
    src/widgets/downloadoptionswidget.cpp
    ui/widgets/downloadoptionswidget.ui
    src/widgets/manualuploadwidget.h
    src/widgets/manualuploadwidget.cpp
    ui/widgets/manualuploadwidget.ui
    src/widgets/librarywidget.h
    src/widgets/librarywidget.cpp
    ui/widgets/librarywidget.ui
    src/widgets/librarytoolbarwidget.h
    src/widgets/librarytoolbarwidget.cpp
    ui/widgets/librarytoolbarwidget.ui
    src/widgets/librarypaginationwidget.h
    src/widgets/librarypaginationwidget.cpp
    ui/widgets/librarypaginationwidget.ui
    src/widgets/channeldatawidget.h
    src/widgets/channeldatawidget.cpp
    ui/widgets/channeldatawidget.ui
    src/widgets/clearablelineedit.h
    src/widgets/clearablelineedit.cpp
    src/widgets/settingswidget.h
    src/widgets/settingswidget.cpp
    ui/widgets/settingswidget.ui

    # Settings Widgets
    src/widgets/settings/generalsettingswidget.h
    src/widgets/settings/generalsettingswidget.cpp
    ui/widgets/settings/generalsettingswidget.ui
    src/widgets/settings/appearancesettingswidget.h
    src/widgets/settings/appearancesettingswidget.cpp
    ui/widgets/settings/appearancesettingswidget.ui
    src/widgets/settings/datasettingswidget.h
    src/widgets/settings/datasettingswidget.cpp
    ui/widgets/settings/datasettingswidget.ui
    src/widgets/settings/DataSourceWidget.h
    src/widgets/settings/DataSourceWidget.cpp
    ui/widgets/settings/DataSourceWidget.ui
    src/widgets/settings/networksettingswidget.h
    src/widgets/settings/networksettingswidget.cpp
    ui/widgets/settings/networksettingswidget.ui
    src/widgets/settings/securitysettingswidget.h
    src/widgets/settings/securitysettingswidget.cpp
    ui/widgets/settings/securitysettingswidget.ui
    src/widgets/settings/updatesettingswidget.h
    src/widgets/settings/updatesettingswidget.cpp
    ui/widgets/settings/updatesettingswidget.ui

    # Dialogs
    src/dialogs/datainputsettingsdialog.h
    src/dialogs/datainputsettingsdialog.cpp
    ui/dialogs/datainputsettingsdialog.ui
    src/dialogs/texteditordialog.h
    src/dialogs/texteditordialog.cpp
    ui/dialogs/texteditordialog.ui
    src/dialogs/playlistimportdialog.h
    src/dialogs/playlistimportdialog.cpp
    ui/dialogs/playlistimportdialog.ui
    src/dialogs/channelsettingsdialog.h
    src/dialogs/channelsettingsdialog.cpp
    ui/dialogs/channelsettingsdialog.ui
    src/dialogs/downloadsettingsdialog.h
    src/dialogs/downloadsettingsdialog.cpp
    ui/dialogs/downloadsettingsdialog.ui
    src/dialogs/batchedittagsdialog.h
    src/dialogs/batchedittagsdialog.cpp
    ui/dialogs/batchedittagsdialog.ui
    src/dialogs/librarysettingsdialog.h
    src/dialogs/librarysettingsdialog.cpp
    ui/dialogs/librarysettingsdialog.ui
    src/dialogs/batchchangechanneldialog.h
    src/dialogs/batchchangechanneldialog.cpp
    ui/dialogs/batchchangechanneldialog.ui
    src/dialogs/editvideodialog.h
    src/dialogs/editvideodialog.cpp
    ui/dialogs/editvideodialog.ui
    src/dialogs/databasemaintenancedialog.h
    src/dialogs/databasemaintenancedialog.cpp
    ui/dialogs/databasemaintenancedialog.ui
    src/dialogs/apikeydialog.h
    src/dialogs/apikeydialog.cpp
    ui/dialogs/apikeydialog.ui
    src/dialogs/proxydialog.h
    src/dialogs/proxydialog.cpp
    ui/dialogs/proxydialog.ui

    # Controllers
    src/controllers/channelcontroller.h
    src/controllers/channelcontroller.cpp
    src/controllers/datainputcontroller.h
    src/controllers/datainputcontroller.cpp
    src/controllers/downloadcontroller.h
    src/controllers/downloadcontroller.cpp
    src/controllers/manualuploadcontroller.h
    src/controllers/manualuploadcontroller.cpp
    src/controllers/librarycontroller.h
    src/controllers/librarycontroller.cpp
    src/controllers/settingscontroller.h
    src/controllers/settingscontroller.cpp

    # Services
    src/services/downloadmanager.h
    src/services/downloadmanager.cpp
    src/services/downloadtask.h
    src/services/downloadtask.cpp
    src/services/taskbarmanager.h
    src/services/taskbarmanager.cpp
    src/services/downloadqueueservice.h
    src/services/downloadqueueservice.cpp
    src/services/metadatafetcher.h
    src/services/metadatafetcher.cpp
    src/services/datainputdataservice.h
    src/services/datainputdataservice.cpp
    src/services/fileimportservice.h
    src/services/fileimportservice.cpp
    src/services/libraryservice.h
    src/services/libraryservice.cpp
    src/services/settingsservice.h
    src/services/settingsservice.cpp

    # UI Managers
    src/ui_managers/datainputuimanager.h
    src/ui_managers/datainputuimanager.cpp
    src/ui_managers/downloaduimanager.h
    src/ui_managers/downloaduimanager.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/settings
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/repositories
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_managers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/delegates
    ${CMAKE_CURRENT_SOURCE_DIR}/src/controllers
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Network
    Qt6::Concurrent
    QXlsx::QXlsx
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ole32)
    
    set(APP_ICON_RESOURCE_WINDOWS "resources.rc")
    set_source_files_properties(${APP_ICON_RESOURCE_WINDOWS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(${PROJECT_NAME} PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
endif()

