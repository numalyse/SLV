cmake_minimum_required(VERSION 3.16)

project(SLV VERSION 0.1 LANGUAGES CXX)

if(MSVC)
    add_compile_options(/MP)
endif()

# =========================================================
# Options générales
# =========================================================
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PYTHON_SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/pyScripts")

# =========================================================
# Qt
# =========================================================
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Concurrent Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent Core)

# =========================================================
# OpenCV
# =========================================================
find_package(OpenCV REQUIRED)

# =========================================================
# Sources de base
# =========================================================
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    Ressources/icon_ressources.qrc
    Ressources/pref_ressources.qrc

    PlayerWidget.h PlayerWidget.cpp
    PlayerLayoutManager.h PlayerLayoutManager.cpp
    Media.h Media.cpp
    GlobalPlayerManager.h GlobalPlayerManager.cpp

    BlackOpacityWidget.h BlackOpacityWidget.cpp
    CompositionWidget.h CompositionWidget.cpp
    DrawingWidget.h DrawingWidget.cpp

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
    SignalManager.h

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

    Preference/PreferenceDialog.h Preference/PreferenceDialog.cpp
    Preference/BasePreferenceTab.h Preference/BasePreferenceTab.cpp

    Timeline/TimelineWidget.h Timeline/TimelineWidget.cpp
    Timeline/TimelineView.h Timeline/TimelineView.cpp
    Timeline/ABManager.h Timeline/ABManager.cpp
    Timeline/ShotManager.h Timeline/ShotManager.cpp
)

# =========================================================
# Fichiers additionnels (fusion des 2 versions)
# =========================================================
set(ADDITIONAL_SOURCES
    # UI / widgets
    FormLineEditWidget.cpp FormLineEditWidget.h
    FormTextEditWidget.cpp FormTextEditWidget.h
    FormLineEdit.cpp FormLineEdit.h
    FormTextEdit.cpp FormTextEdit.h

    GenericDialog.h
    GlobalScreenshotHelper.h
    ErrorsHelper.h
    ExtractSequenceWidget.cpp ExtractSequenceWidget.h
    AdjustmentsWidget.cpp AdjustmentsWidget.h
    TimeEditor.cpp TimeEditor.h

    # Media / backend
    DecodeThread.cpp DecodeThread.h
    FileCopyThread.cpp FileCopyThread.h
    VideoCaptureManager.cpp VideoCaptureManager.h
    TSQueue.h
    MediaTransformHelper.h
    SequenceExtractionHelper.h

    # Timeline complet
    Timeline/TimelineMath.cpp Timeline/TimelineMath.h
    Timeline/SegmentationThread.cpp Timeline/SegmentationThread.h
    Timeline/ThumbnailWorker.cpp Timeline/ThumbnailWorker.h
    Timeline/ItemTypes.h

    Timeline/Items/ABMarkerItem.cpp Timeline/Items/ABMarkerItem.h
    Timeline/Items/CursorItem.cpp Timeline/Items/CursorItem.h
    Timeline/Items/RulerItem.cpp Timeline/Items/RulerItem.h
    Timeline/Items/ShotItem.cpp Timeline/Items/ShotItem.h

    # Preferences complètes
    Preference/Tabs/InterfaceTab.cpp Preference/Tabs/InterfaceTab.h
    Preference/Tabs/ShortcutTab.cpp  Preference/Tabs/ShortcutTab.h
    Preference/Tabs/PathTab.cpp      Preference/Tabs/PathTab.h

    Preference/Frames/FormPathEditFrame.cpp Preference/Frames/FormPathEditFrame.h
    Preference/Frames/FormShortcutEditFrame.cpp Preference/Frames/FormShortcutEditFrame.h
    Preference/Frames/FormComboBoxFrame.cpp Preference/Frames/FormComboBoxFrame.h

    Preference/BasePreferenceFrame.cpp Preference/BasePreferenceFrame.h

    # Project extras si manquants
    SequenceExtractionHelper.h
)

# =========================================================
# Fusion sans doublons
# =========================================================
foreach(_src IN LISTS ADDITIONAL_SOURCES)
    list(FIND PROJECT_SOURCES "${_src}" _found)
    if(_found EQUAL -1)
        list(APPEND PROJECT_SOURCES "${_src}")
    endif()
endforeach()

# =========================================================
# Exécutable Qt
# =========================================================
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(SLV MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    add_executable(SLV ${PROJECT_SOURCES})
endif()

# =========================================================
# Includes
# =========================================================
target_include_directories(SLV PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# =========================================================
# OpenCV (optionnel mais conservé)
# =========================================================
if(OpenCV_INCLUDE_DIRS)
    target_include_directories(SLV PRIVATE ${OpenCV_INCLUDE_DIRS})
endif()

# =========================================================
# Qt linking
# =========================================================
target_link_libraries(SLV PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Core
)

# =========================================================
# OpenCV linking
# =========================================================
target_link_libraries(SLV PRIVATE ${OpenCV_LIBS})

# =========================================================
# FFmpeg cross-platform
# =========================================================

set(FFMPEG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg")

if(WIN32)

    # =====================================================
    # Windows (libs locales .lib)
    # =====================================================
    target_include_directories(SLV PRIVATE
        "${FFMPEG_DIR}/include"
    )

    target_link_libraries(SLV PRIVATE
        "${FFMPEG_DIR}/lib/avcodec.lib"
        "${FFMPEG_DIR}/lib/avformat.lib"
        "${FFMPEG_DIR}/lib/avutil.lib"
        "${FFMPEG_DIR}/lib/swscale.lib"
    )

elseif(APPLE)

    # =====================================================
    # macOS (Homebrew)
    # =====================================================
    set(FFMPEG_INCLUDE_DIR /opt/homebrew/include)
    set(FFMPEG_LIB_DIR /opt/homebrew/lib)

    target_include_directories(SLV PRIVATE
        ${FFMPEG_INCLUDE_DIR}
    )

    target_link_directories(SLV PRIVATE
        ${FFMPEG_LIB_DIR}
    )

    target_link_libraries(SLV PRIVATE
        avcodec
        avformat
        avutil
        swscale
    )

else()

    # =====================================================
    # Linux (system packages)
    # =====================================================
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
        libavcodec
        libavformat
        libavutil
        libswscale
    )

    target_link_libraries(SLV PRIVATE PkgConfig::FFMPEG)

endif()

# =========================================================
# VLC cross-platform
# =========================================================

set(VLC_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vlc-sdk")

if(WIN32)
# Includes
    target_include_directories(SLV PRIVATE "${VLC_SDK_PATH}/include")
    
# Librairies
    target_link_directories(SLV PRIVATE "${VLC_SDK_PATH}/lib")

    target_link_libraries(SLV PRIVATE
        libvlc.lib
        libvlccore.lib
    )

elseif(APPLE)

    set(VLC_APP "/Applications/VLC.app")

    target_include_directories(SLV PRIVATE
        "${VLC_APP}/Contents/MacOS/include"
        "${VLC_APP}/Contents/Resources/include"
    )

    find_library(VLC_LIB vlc HINTS
        "${VLC_APP}/Contents/MacOS/lib"
        /opt/homebrew/lib
    )

    find_library(VLCCORE_LIB vlccore HINTS
        "${VLC_APP}/Contents/MacOS/lib"
        /opt/homebrew/lib
    )

    if(NOT VLC_LIB OR NOT VLCCORE_LIB)
        message(FATAL_ERROR "VLC libs not found on macOS")
    endif()

    target_link_libraries(SLV PRIVATE
        ${VLC_LIB}
        ${VLCCORE_LIB}
    )

else()

    target_link_libraries(SLV PRIVATE
        vlc
        vlccore
    )

endif()

# =========================================================
# Windows-only (DLL / deploy)
# =========================================================
if(WIN32)

    # copy runtime DLLs
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

# FFmpeg/OpenCV dll detection (Windows only)
file(GLOB_RECURSE FFMPEG_DLL_LIST
    "${OpenCV_DIR}/x64/*/bin/opencv_videoio_ffmpeg*.dll"
    "${OpenCV_DIR}/bin/opencv_videoio_ffmpeg*.dll"
)

# =========================================================
# macOS-only (bundle / dylib / resources)
# =========================================================
if(APPLE)

    # -----------------------------------------------------
    # Copy runtime resources into .app bundle
    # -----------------------------------------------------
    add_custom_command(TARGET SLV POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:SLV>/../Resources/bin"
    )

    add_custom_command(TARGET SLV POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/bin"
        "$<TARGET_FILE_DIR:SLV>/../Resources/bin"
    )

    # -----------------------------------------------------
    # VLC plugins (mac version = dylib/plugins)
    # -----------------------------------------------------
    add_custom_command(TARGET SLV POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/vlc-sdk/plugins"
        "$<TARGET_FILE_DIR:SLV>/../Resources/plugins"
    )

    # -----------------------------------------------------
    # VLC dynamic libraries (mac: .dylib au lieu de .dll)
    # -----------------------------------------------------
    add_custom_command(TARGET SLV POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libvlc.dylib"
        "$<TARGET_FILE_DIR:SLV>"
    )

    add_custom_command(TARGET SLV POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libvlccore.dylib"
        "$<TARGET_FILE_DIR:SLV>"
    )

    # -----------------------------------------------------
    # Python scripts
    # -----------------------------------------------------
    add_custom_command(TARGET SLV POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PYTHON_SCRIPTS_DIR}"
        "$<TARGET_FILE_DIR:SLV>/../Resources/pyScripts"
        COMMENT "Copy Python scripts into macOS app bundle..."
    )

endif()

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

endif()

set_target_properties(SLV PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# =========================================================
# Python scripts copy (cross-platform)
# =========================================================
add_custom_command(TARGET SLV POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PYTHON_SCRIPTS_DIR}"
    "$<TARGET_FILE_DIR:SLV>/pyScripts"
)

# =========================================================
# Install rules
# =========================================================
include(GNUInstallDirs)

install(TARGETS SLV
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# =========================================================
# Qt finalization
# =========================================================
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SLV)
endif()