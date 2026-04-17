cmake_minimum_required(VERSION 3.16)

project(SLV VERSION 0.1 LANGUAGES CXX)

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
# Source files
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

# Add any .cpp files that were not explicitly listed above (helps when files
# were added/renamed without updating this list). This keeps the CMake file
# robust during development on multiple platforms.
## Explicit additional sources (avoid automated globs to keep AUTOMOC stable)
set(ADDITIONAL_SOURCES
    AdjustmentsWidget.cpp
    DecodeThread.cpp
    FileCopyThread.cpp
    VideoCaptureManager.cpp
    ExtractSequenceWidget.cpp

    Timeline/SegmentationThread.cpp
    Timeline/ThumbnailWorker.cpp
    Timeline/TimelineMath.cpp
    Timeline/Items/CursorItem.cpp
    Timeline/Items/ABMarkerItem.cpp
    Timeline/Items/ShotItem.cpp
    Timeline/Items/RulerItem.cpp

    Preference/Tabs/InterfaceTab.cpp
    Preference/Tabs/PathTab.cpp
    Preference/Tabs/ShortcutTab.cpp
    Preference/Frames/FormShortcutEditFrame.cpp
    Preference/Frames/FormComboBoxFrame.cpp
    Preference/Frames/FormPathEditFrame.cpp

    FormLineEditWidget.cpp
    FormTextEditWidget.cpp
    FormLineEdit.cpp
    FormTextEdit.cpp
    TimeEditor.cpp
    Preference/BasePreferenceFrame.cpp
)

foreach(_src IN LISTS ADDITIONAL_SOURCES)
    list(FIND PROJECT_SOURCES "${_src}" _found)
    if(_found EQUAL -1)
        list(APPEND PROJECT_SOURCES "${_src}")
    endif()
endforeach()

# =========================================================
# Executable
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
    ${OpenCV_INCLUDE_DIRS}
)

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

    target_link_directories(SLV PRIVATE
        "${FFMPEG_DIR}/lib"
    )

    target_link_libraries(SLV PRIVATE
        avcodec
        avformat
        avutil
        swscale
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

    add_custom_command(TARGET SLV POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libvlc.dll"
        "$<TARGET_FILE_DIR:SLV>"
    )

    add_custom_command(TARGET SLV POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libvlccore.dll"
        "$<TARGET_FILE_DIR:SLV>"
    )

    # FFmpeg/OpenCV dll detection (Windows only)
    file(GLOB_RECURSE FFMPEG_DLL_LIST
        "${OpenCV_DIR}/x64/*/bin/opencv_videoio_ffmpeg*.dll"
        "${OpenCV_DIR}/bin/opencv_videoio_ffmpeg*.dll"
    )

    if(FFMPEG_DLL_LIST)
        list(GET FFMPEG_DLL_LIST 0 FFMPEG_DLL_PATH)

        add_custom_command(TARGET SLV POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:SLV>
            $<TARGET_FILE_DIR:SLV>

            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FFMPEG_DLL_PATH}"
            "$<TARGET_FILE_DIR:SLV>"

            COMMAND_EXPAND_LISTS
        )
    endif()

    find_program(WINDEPLOYQT_EXECUTABLE windeployqt)

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET SLV POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --debug
                --no-compiler-runtime
                "$<TARGET_FILE:SLV>"
        )
    endif()

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