cmake_minimum_required(VERSION 3.16)

project(SLV VERSION 0.1 LANGUAGES CXX)

if(MSVC)
    add_compile_options(/MP)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PYTHON_SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/pyScripts")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package( OpenCV REQUIRED )

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
        GlobalPlayerManager.h GlobalPlayerManager.cpp

        CompositionWidget.h CompositionWidget.cpp

        Toolbars/Toolbar.h
        Toolbars/GlobalToolbar.h Toolbars/GlobalToolbar.cpp
        Toolbars/AdvancedToolbar.h Toolbars/AdvancedToolbar.cpp
        Toolbars/SimpleToolbar.h Toolbars/SimpleToolbar.cpp
        Toolbars/ExtensionToolbar.h Toolbars/ExtensionToolbar.cpp
        Toolbars/TimeEdit.h Toolbars/TimeEdit.cpp
        Toolbars/TimeValidator.h Toolbars/TimeValidator.cpp

        ToolbarButtons/ToolbarButton.h ToolbarButtons/ToolbarButton.cpp
        ToolbarButtons/ToolbarToggleButton.h ToolbarButtons/ToolbarToggleButton.cpp
        ToolbarButtons/ToolbarPopupButton.h ToolbarButtons/ToolbarPopupButton.cpp
        ToolbarButtons/ToolbarToggleHoverButton.h ToolbarButtons/ToolbarToggleHoverButton.cpp

        PrefManager.h PrefManager.cpp
        TimeFormatter.h
        VlcParseHelper.h
        VlcInstance.h
        Shot.h
        Project/Project.h
        Project/ProjectManager.h Project/ProjectManager.cpp
        Project/ProjectFileHelper.h Project/ProjectFileHelper.cpp
        Project/ProjectExportHelper.h Project/ProjectExportHelper.cpp
        Project/ProjectExportThread.h Project/ProjectExportThread.cpp

        MediaWidget.h MediaWidget.cpp
        NavPanel.h NavPanel.cpp
        PlaylistItem.h PlaylistItem.cpp
        Playlist.h Playlist.cpp
        ShotDetail.h ShotDetail.cpp
        FormLineEditWidget.h FormLineEditWidget.cpp
        FormLineEdit.h FormLineEdit.cpp
        FormTextEditWidget.h FormTextEditWidget.cpp
        FormTextEdit.h FormTextEdit.cpp
        SignalManager.h

        Timeline/TimelineWidget.h Timeline/TimelineWidget.cpp
        Timeline/TimelineView.h Timeline/TimelineView.cpp
        Timeline/ABManager.h Timeline/ABManager.cpp
        Timeline/ShotManager.h Timeline/ShotManager.cpp
        Timeline/TimelineMath.h Timeline/TimelineMath.cpp
        Timeline/SegmentationThread.h Timeline/SegmentationThread.cpp
        Timeline/ThumbnailWorker.h Timeline/ThumbnailWorker.cpp

        Timeline/ItemTypes.h

        Timeline/Items/ABMarkerItem.h Timeline/Items/ABMarkerItem.cpp
        Timeline/Items/RulerItem.h Timeline/Items/RulerItem.cpp
        Timeline/Items/CursorItem.h Timeline/Items/CursorItem.cpp
        Timeline/Items/ShotItem.h Timeline/Items/ShotItem.cpp
        SequenceExtractionHelper.h
        VideoCaptureManager.h VideoCaptureManager.cpp

        FileCopyThread.h FileCopyThread.cpp

        GenericDialog.h
        GlobalScreenshotHelper.h
        ErrorsHelper.h

        DecodeThread.h DecodeThread.cpp
        TSQueue.h
        MediaTransformHelper.h

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
    ${OpenCV_LIBS}
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
target_link_libraries(SLV PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)
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

# Copier le .py pour le pptx
add_custom_command(TARGET SLV POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PYTHON_SCRIPTS_DIR}"
    "$<TARGET_FILE_DIR:SLV>/pyScripts"
    COMMENT "Copie des scripts Python vers le dossier de sortie..."
)

file(GLOB_RECURSE FFMPEG_DLL_LIST 
    "${OpenCV_DIR}/x64/*/bin/opencv_videoio_ffmpeg*.dll" 
    "${OpenCV_DIR}/bin/opencv_videoio_ffmpeg*.dll"
)

if(FFMPEG_DLL_LIST)
    list(GET FFMPEG_DLL_LIST 0 FFMPEG_DLL_PATH)
    message(STATUS "DLL FFMPEG FOUND : ${FFMPEG_DLL_PATH}")

    add_custom_command(TARGET SLV POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:SLV>
            $<TARGET_FILE_DIR:SLV>
            
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FFMPEG_DLL_PATH}"
            "$<TARGET_FILE_DIR:SLV>"
            
        COMMAND_EXPAND_LISTS
    )
else()
    message(FATAL_ERROR "Impossible de trouver la DLL FFmpeg dans ${OpenCV_DIR}.")
endif()


find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_FUNCTIONAL_DIR}/../../../bin")

if(WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET SLV POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --debug
                --no-compiler-runtime
                --verbose 1
                "$<TARGET_FILE:SLV>"
        COMMENT "Déploiement des dépendances Qt avec windeployqt..."
    )
endif()
