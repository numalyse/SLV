cmake_minimum_required(VERSION 3.16)

project(SLV VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        Ressources/icon_ressources.qrc
        Ressources/translation_ressources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SLV
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        PlayerWidget.h
        PlayerWidget.cpp
        PlayerLayoutManager.h PlayerLayoutManager.cpp
        Media.h Media.cpp
        PlaylistManager.h PlaylistManager.cpp
        GlobalPlayerManager.h GlobalPlayerManager.cpp

        Toolbars/Toolbar.h
        Toolbars/GlobalToolbar.h Toolbars/GlobalToolbar.cpp
        Toolbars/AdvancedToolbar.h Toolbars/AdvancedToolbar.cpp
        Toolbars/SimpleToolbar.h Toolbars/SimpleToolbar.cpp
        Toolbars/ExtensionToolbar.h Toolbars/ExtensionToolbar.cpp

        ToolbarButtons/ToolbarButton.h ToolbarButtons/ToolbarButton.cpp
        ToolbarButtons/ToolbarToggleButton.h ToolbarButtons/ToolbarToggleButton.cpp
        ToolbarButtons/ToolbarPopupButton.h ToolbarButtons/ToolbarPopupButton.cpp
        ToolbarButtons/ToolbarToggleHoverButton.h ToolbarButtons/ToolbarToggleHoverButton.cpp

        TextManager.h TextManager.cpp
        TimeFormatter.h
        VlcParseHelper.h
        VlcInstance.h
        Shot.h
        Project.h
        ProjectManager.h ProjectManager.cpp
        Timeline.h Timeline.cpp

        MediaWidget.h MediaWidget.cpp
        NavPanel.h NavPanel.cpp


    )


# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SLV APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(SLV SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SLV
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# -----------------------------
# Pour pouvoir avoir des includes non relatifs
# -----------------------------
target_include_directories(SLV PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# -----------------------------
# Ajout FFMPEG
# -----------------------------
set(FFMPEG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg")

#Includes
target_include_directories(SLV PRIVATE "${FFMPEG_DIR}/include")


#Librairies
target_link_libraries(SLV PRIVATE
    Qt6::Widgets
    ${FFMPEG_DIR}/lib/avcodec.lib
    ${FFMPEG_DIR}/lib/avformat.lib
    ${FFMPEG_DIR}/lib/avutil.lib
    ${FFMPEG_DIR}/lib/swscale.lib
)
# -----------------------------

# -----------------------------
# Ajout libVLC
# -----------------------------

set(VLC_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vlc-sdk")

# Includes
target_include_directories(SLV PRIVATE "${VLC_SDK_PATH}/include")

# Librairies
target_link_directories(SLV PRIVATE "${VLC_SDK_PATH}/lib")
target_link_libraries(SLV PRIVATE libvlc.lib libvlccore.lib)
# -----------------------------

target_link_libraries(SLV PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
# -----------------------------
target_link_libraries(SLV PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(SLV PRIVATE Qt${QT_VERSION_MAJOR}::Core)
# -----------------------------

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SLV)
endif()
set_target_properties(SLV PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SLV
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SLV)
endif()

# ============================================
# Copier bin
# ============================================

add_custom_command(TARGET SLV POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/bin"
    "$<TARGET_FILE_DIR:SLV>/bin"
)

add_custom_command(TARGET SLV POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/vlc-sdk/plugins"
    "$<TARGET_FILE_DIR:SLV>/plugins"
)


# Copier libvlc.dll
add_custom_command(TARGET SLV POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/libvlc.dll"
    "$<TARGET_FILE_DIR:SLV>"
)

# Copier libvlccore.dll
add_custom_command(TARGET SLV POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/libvlccore.dll"
    "$<TARGET_FILE_DIR:SLV>"
)
