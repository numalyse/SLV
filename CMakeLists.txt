cmake_minimum_required(VERSION 3.16)

project(SLV VERSION 1.0 LANGUAGES CXX)
set(PROJECT_STAGE "alpha")
set(APP_VERSION "${PROJECT_VERSION}-${PROJECT_STAGE}")

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
if(APPLE AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    option(ENABLE_MACDEPLOYQT "Run macdeployqt after building the app bundle on macOS" OFF)
endif()

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
# Python (embedded ou portable runtime)
# =========================================================
if(WIN32)
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Embed)
endif()

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
    Ressources/logo_ressources.qrc

    PlayerWidget.h PlayerWidget.cpp
    PlayerLayoutManager.h PlayerLayoutManager.cpp
    Media.h Media.cpp
    GlobalPlayerManager.h GlobalPlayerManager.cpp

    BlackOpacityWidget.h BlackOpacityWidget.cpp
    CompositionWidget.h CompositionWidget.cpp
    DrawingWidget.h DrawingWidget.cpp
    MediaLogoWidget.h MediaLogoWidget.cpp

    Toolbars/Toolbar.h Toolbars/Toolbar.cpp
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
    Timeline/TransitionManager.h Timeline/TransitionManager.cpp
    Timeline/BaseRangeManager.h

    HelperWidget.h HelperWidget.cpp
    AboutWidget.h AboutWidget.cpp

    Ressources/supported_formats_resources.qrc
    FileFormatManager.h FileFormatManager.cpp

    Tutorial.h Tutorial.cpp 
    TutoOpacityWidget.h TutoOpacityWidget.cpp
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
    VideoCaptureHelper.h
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
    AspectRatioPixmapLabel.h AspectRatioPixmapLabel.cpp

    # Timeline complet
    Timeline/TimelineMath.cpp Timeline/TimelineMath.h
    Timeline/SegmentationThread.cpp Timeline/SegmentationThread.h
    Timeline/ThumbnailWorker.cpp Timeline/ThumbnailWorker.h
    Timeline/ItemTypes.h

    Timeline/Items/ABMarkerItem.cpp Timeline/Items/ABMarkerItem.h
    Timeline/Items/CursorItem.cpp Timeline/Items/CursorItem.h
    Timeline/Items/RulerItem.cpp Timeline/Items/RulerItem.h
    Timeline/Items/ShotItem.cpp Timeline/Items/ShotItem.h
    Timeline/Items/AudioShotItem.h Timeline/Items/AudioShotItem.cpp
    Timeline/Items/AudioVisualizerItem.h Timeline/Items/AudioVisualizerItem.cpp
    Timeline/Items/TransitionMarkerItem.cpp Timeline/Items/TransitionMarkerItem.h
    Timeline/Items/RangeMarkerItem.cpp Timeline/Items/RangeMarkerItem.h

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
    ZoomHelper.h
    
    MediaInfoDialog.h MediaInfoDialog.cpp

)

if(APPLE)
    list(APPEND PROJECT_SOURCES
        MacMouseTracker.h
        MacMouseTracker.mm
    )
endif()

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
# Exécutable Qt multiplateforme
# =========================================================

if(QT_VERSION_MAJOR GREATER_EQUAL 6)

    qt_add_executable(SLV
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        Ressources/help_dialog_illustrations.qrc


    )

else()

    add_executable(SLV ${PROJECT_SOURCES})

endif()

# =========================================================
# Configuration par plateforme
# =========================================================

# Windows → pas de console
if(WIN32)
    set_target_properties(SLV PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# macOS → bundle .app
if(APPLE)
    set_target_properties(SLV PROPERTIES MACOSX_BUNDLE TRUE)
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set_target_properties(SLV PROPERTIES
            BUILD_WITH_INSTALL_RPATH TRUE
            INSTALL_RPATH "@executable_path/../Frameworks"
        )
    endif()
endif()

# =========================================================
# NOM DE L'APP Exécutable Qt
# =========================================================
if(WIN32)
set_target_properties(SLV PROPERTIES
    OUTPUT_NAME "NumalysePlayer"
)
elseif(APPLE)
set_target_properties(SLV PROPERTIES
    OUTPUT_NAME "NumalysePlayer"
    MACOSX_BUNDLE_BUNDLE_NAME "NumalysePlayer"
)
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
# Python3
# =========================================================
# target_include_directories(SLV PRIVATE ${Python3_INCLUDE_DIRS})
# target_link_libraries(SLV PRIVATE ${Python3_LIBRARIES})
if(WIN32)
    target_link_libraries(SLV PRIVATE Python3::Python)
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

    # Expose the bundled ffmpeg executable path to the C++ code
    if(WIN32)
        set(FFMPEG_EXE_SUFFIX ".exe")
    else()
        set(FFMPEG_EXE_SUFFIX "")
    endif()

    set(FFMPEG_EXECUTABLE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/ffmpeg${FFMPEG_EXE_SUFFIX}")

    target_compile_definitions(SLV PRIVATE "FFMPEG_EXECUTABLE=\"${FFMPEG_EXECUTABLE_PATH}\"")

elseif(APPLE)

    # =====================================================
    # macOS (Homebrew - Apple Silicon vs Intel)
    # =====================================================

    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(HOMEBREW_PREFIX /opt/homebrew)
        
        set(FFMPEG_INCLUDE_DIR ${HOMEBREW_PREFIX}/include)
        set(FFMPEG_LIB_DIR ${HOMEBREW_PREFIX}/lib)

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
        #set(HOMEBREW_PREFIX /usr/local)
        set(HOMEBREW_PREFIX ${CMAKE_SOURCE_DIR}/ffmpeg/ffmpeg)
    endif()



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

    # =====================================================
    # Windows
    # =====================================================

# Includes
    target_include_directories(SLV PRIVATE "${VLC_SDK_PATH}/include")
    
# Librairies
    target_link_directories(SLV PRIVATE "${VLC_SDK_PATH}/lib")

    target_link_libraries(SLV PRIVATE
        libvlc.lib
        libvlccore.lib
    )

elseif(APPLE)

    # =====================================================
    # macOS (VLC - Apple Silicon vs Intel)
    # =====================================================

    set(VLC_APP "/Applications/VLC.app")

    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(HOMEBREW_PREFIX /opt/homebrew)
    else()
        set(HOMEBREW_PREFIX /usr/local)
    endif()

    target_include_directories(SLV PRIVATE
        "${VLC_APP}/Contents/MacOS/include"
        "${VLC_APP}/Contents/Resources/include"
    )

    find_library(VLC_LIB vlc HINTS
        "${VLC_APP}/Contents/MacOS/lib"
        "${HOMEBREW_PREFIX}/lib"
    )

    find_library(VLCCORE_LIB vlccore HINTS
        "${VLC_APP}/Contents/MacOS/lib"
        "${HOMEBREW_PREFIX}/lib"
    )

    if(NOT VLC_LIB OR NOT VLCCORE_LIB)
        message(FATAL_ERROR "VLC libs not found on macOS (arm64/intel)")
    endif()

    target_link_libraries(SLV PRIVATE
        ${VLC_LIB}
        ${VLCCORE_LIB}
        "-framework AppKit"
    )

    # =====================================================
    # macOS VLC EMBEDDED
    # =====================================================

    set(VLC_APP "/Applications/VLC.app")
    set(VLC_LIB_DIR "${VLC_APP}/Contents/MacOS/lib")
    set(VLC_PLUGIN_DIR "${VLC_APP}/Contents/MacOS/plugins")

    # Vérification
    if(NOT EXISTS "${VLC_LIB_DIR}/libvlc.dylib")
        message(FATAL_ERROR "libvlc.dylib not found")
    endif()

    if(NOT EXISTS "${VLC_LIB_DIR}/libvlccore.dylib")
        message(FATAL_ERROR "libvlccore.dylib not found")
    endif()

    # Include VLC headers
    target_include_directories(SLV PRIVATE
        "${VLC_APP}/Contents/MacOS/include"
        "${VLC_APP}/Contents/Resources/include"
    )

    # Link VLC directly from installed VLC.app
    target_link_libraries(SLV PRIVATE
        "${VLC_LIB_DIR}/libvlc.dylib"
        "${VLC_LIB_DIR}/libvlccore.dylib"
    )

    # =====================================================
    # Embed VLC inside .app bundle
    # =====================================================

    add_custom_command(TARGET SLV POST_BUILD

        # -----------------------------
        # Create directories
        # -----------------------------
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:SLV>/../Frameworks"

        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:SLV>/../Resources/vlc"

        # -----------------------------
        # Copy VLC dylibs
        # -----------------------------
        COMMAND ${CMAKE_COMMAND} -E copy
        "${VLC_LIB_DIR}/libvlc.dylib"
        "$<TARGET_FILE_DIR:SLV>/../Frameworks/"

        COMMAND ${CMAKE_COMMAND} -E copy
        "${VLC_LIB_DIR}/libvlccore.dylib"
        "$<TARGET_FILE_DIR:SLV>/../Frameworks/"

        # -----------------------------
        # Copy VLC plugins
        # ditto preserves symlinks
        # -----------------------------
        COMMAND ditto
        "${VLC_PLUGIN_DIR}"
        "$<TARGET_FILE_DIR:SLV>/../Resources/vlc/plugins"

        COMMENT "Embedding VLC into app bundle"
    )

    # =====================================================
    # RPATH
    # =====================================================

    set_target_properties(SLV PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "@executable_path/../Frameworks"
    )

else()

    # =====================================================
    # Linux
    # =====================================================

    target_link_libraries(SLV PRIVATE
        vlc
        vlccore
    )

endif()

# =========================================================
# Windows-only (DLL / deploy)
# =========================================================
if(WIN32)

    # =====================================================
    # DIST folder (portable app)
    # =====================================================
    set(DEPLOY_DIR "${CMAKE_BINARY_DIR}/dist")

    # =====================================================
    # FFmpeg detection
    # =====================================================
    file(GLOB_RECURSE FFMPEG_DLL_LIST
        "${OpenCV_DIR}/x64/*/bin/opencv_videoio_ffmpeg*.dll"
        "${OpenCV_DIR}/bin/opencv_videoio_ffmpeg*.dll"
    )

    # =====================================================
    # OpenCV world DLL detection
    # =====================================================
    file(GLOB OPENCV_WORLD_DLL
        "${OpenCV_DIR}/x64/*/bin/opencv_world*.dll"
        "${OpenCV_DIR}/bin/opencv_world*.dll"
    )

    # =====================================================
    # POST BUILD (DEBUG + RELEASE + DIST)
    # =====================================================
    add_custom_command(TARGET SLV POST_BUILD

        # -------------------------------------------------
        # - Copy runtime bin folder (build dir)
        # -------------------------------------------------
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/bin"
        "$<TARGET_FILE_DIR:SLV>/bin"

        # -------------------------------------------------
        # - VLC plugins (build dir)
        # -------------------------------------------------
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/vlc-sdk/plugins"
        "$<TARGET_FILE_DIR:SLV>/plugins"

        # -------------------------------------------------
        # - VLC DLLs (build dir)
        # -------------------------------------------------
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libvlc.dll"
        "$<TARGET_FILE_DIR:SLV>"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libvlccore.dll"
        "$<TARGET_FILE_DIR:SLV>"

        # -------------------------------------------------
        # - Python scripts (build dir)
        # -------------------------------------------------
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PYTHON_SCRIPTS_DIR}"
        "$<TARGET_FILE_DIR:SLV>/pyScripts"

        # =========================================================
        # PYTHON RUNTIME EMBEDDED
        # =========================================================

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/python_embedded"
        "$<TARGET_FILE_DIR:SLV>/python"
        

        # =================================================
        # - ALSO COPY TO DIST (portable version)
        # =================================================

        COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_DIR}"

        # exe
        COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:SLV>"
        "${DEPLOY_DIR}/"

        # bin
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/bin"
        "${DEPLOY_DIR}/bin"

        # VLC plugins
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/vlc-sdk/plugins"
        "${DEPLOY_DIR}/plugins"

        # VLC DLLs
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libvlc.dll"
        "${DEPLOY_DIR}/"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libvlccore.dll"
        "${DEPLOY_DIR}/"

        # Python scripts
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PYTHON_SCRIPTS_DIR}"
        "${DEPLOY_DIR}/pyScripts"

        # Python embedded
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/python_embedded"
        "${DEPLOY_DIR}/python"

        # FFmpeg/OpenCV
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${FFMPEG_DLL_LIST}
        "${DEPLOY_DIR}"

        # OpenCV world DLL (build dir)
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OPENCV_WORLD_DLL}
            "$<TARGET_FILE_DIR:SLV>"

        # OpenCV world DLL (dist)
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OPENCV_WORLD_DLL}
            "${DEPLOY_DIR}"

        COMMENT "📦 Copy vers build + dist (Debug/Release + portable)"
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
        message(FATAL_ERROR "Impossible de trouver la DLL FFmpeg dans ${FFMPEG_DLL_PATH}.")
    endif()

endif()

# =========================================================
# macOS-only (bundle / dylib / resources)
# =========================================================
if(APPLE)

    # Base paths (bundle safe)
    set(MAC_RESOURCES_DIR "$<TARGET_BUNDLE_CONTENT_DIR:SLV>/Resources")

    # Copy embedded Python runtime into the app bundle if present
    if(EXISTS "${CMAKE_SOURCE_DIR}/python_embedded")
        message(STATUS "python_embedded path = ${CMAKE_SOURCE_DIR}/python_embedded")

        add_custom_command(TARGET SLV POST_BUILD

            COMMAND ${CMAKE_COMMAND} -E echo
            "Copy embedded Python..."

            COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:SLV>/../Resources/python"

            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/python_embedded"
            "$<TARGET_FILE_DIR:SLV>/../Resources/python"

            COMMENT "Copy python_embedded into bundle Resources/python"
        )
    else()
        message(STATUS "⚠️ Embedded Python runtime not found -> skip copy to bundle")
    endif()

    # Copy Python scripts into the app bundle Resources (pyScripts)
    if(EXISTS "${PYTHON_SCRIPTS_DIR}")
        message(STATUS "PyScripts path = ${PYTHON_SCRIPTS_DIR}")
        add_custom_command(TARGET SLV POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo
            "Copy pyScripts into bundle..."

            COMMAND ${CMAKE_COMMAND} -E make_directory 
            "${MAC_RESOURCES_DIR}/pyScripts"
            
            COMMAND ${CMAKE_COMMAND} -E copy_directory 
            "${PYTHON_SCRIPTS_DIR}"
            "${MAC_RESOURCES_DIR}/pyScripts"

            COMMENT "Copy Python scripts into macOS app bundle Resources/pyScripts"
        )
    else()
        message(STATUS "⚠️ pyScripts not found -> skip copy to bundle")
    endif()

    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        if(OpenCV_DIR)
            if(OpenCV_LIB_DIR)
                set(OpenCV_LIBRARY_DIR "${OpenCV_LIB_DIR}")
            else()
                get_filename_component(OpenCV_LIBRARY_DIR "${OpenCV_DIR}/../.." REALPATH)
            endif()

            if(OpenCV_VERSION_MAJOR AND OpenCV_VERSION_MINOR)
                set(OpenCV_DYLIB_VERSION_SUFFIX "${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}")
            else()
                set(OpenCV_DYLIB_VERSION_SUFFIX "*")
            endif()

            file(GLOB OpenCV_BUNDLE_LIBS
                "${OpenCV_LIBRARY_DIR}/libopencv_*.${OpenCV_DYLIB_VERSION_SUFFIX}.dylib"
            )

            if(OpenCV_BUNDLE_LIBS)
                message(STATUS "OpenCV bundle libs: ${OpenCV_BUNDLE_LIBS}")

                add_custom_command(TARGET SLV POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory
                    "$<TARGET_FILE_DIR:SLV>/../Frameworks"
                    COMMENT "Ensure macOS app bundle Frameworks directory exists"
                )

                foreach(_opencv_lib IN LISTS OpenCV_BUNDLE_LIBS)
                    add_custom_command(TARGET SLV POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            "${_opencv_lib}"
                            "$<TARGET_FILE_DIR:SLV>/../Frameworks"
                        COMMENT "Copy ${_opencv_lib} into macOS app bundle Frameworks"
                    )
                endforeach()

                # Clean up duplicate OpenCV dylibs (keep only .414.dylib versions)
                add_custom_command(TARGET SLV POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_SOURCE_DIR}/cleanup_opencv_bundle.cmake"
                    COMMENT "Clean up duplicate OpenCV dylibs from app bundle"
                )
            endif()
        endif()
    endif()

    # Copy ffmpeg into Resources/ffmpeg
    if(EXISTS "${CMAKE_SOURCE_DIR}/ffmpeg")
        message(STATUS "ffmpeg path = ${CMAKE_SOURCE_DIR}/ffmpeg")

        add_custom_command(TARGET SLV POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo
            "Copy ffmpeg into bundle..."

            COMMAND ${CMAKE_COMMAND} -E make_directory
            ${MAC_RESOURCES_DIR}/ffmpeg

            COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            ${CMAKE_SOURCE_DIR}/ffmpeg/ffmpeg
            ${MAC_RESOURCES_DIR}/ffmpeg/

            COMMAND chmod +x
            ${MAC_RESOURCES_DIR}/ffmpeg/ffmpeg
        )
    endif()

endif()



# =========================
# QT DEPLOY TOOL
# =========================
if(APPLE AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS
        "${CMAKE_PREFIX_PATH}/bin"
        "${CMAKE_PREFIX_PATH}/../bin"
    )
endif()

find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_FUNCTIONAL_DIR}/../../../bin")

# =========================
# DIST FOLDER (PORTABLE APP)
# =========================
if(WIN32)
    set(DEPLOY_DIR "${CMAKE_BINARY_DIR}/dist")
endif()

if(APPLE AND MACDEPLOYQT_EXECUTABLE AND ENABLE_MACDEPLOYQT)
    add_custom_command(TARGET SLV POST_BUILD
        COMMAND "${MACDEPLOYQT_EXECUTABLE}"
            "$<TARGET_BUNDLE_DIR:SLV>"
        COMMENT "📦 Deploy Qt frameworks into the macOS app bundle"
    )
endif()

# =========================
# POST BUILD PACKAGING
# =========================
if(WIN32 AND WINDEPLOYQT_EXECUTABLE)

    add_custom_command(TARGET SLV POST_BUILD

        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            $<$<CONFIG:Debug>:--debug>
            $<$<CONFIG:Release>:--release>
            --no-compiler-runtime
            --dir "$<TARGET_FILE_DIR:SLV>"
            "$<TARGET_FILE:SLV>"

        COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_DIR}"

        COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:SLV>"
            "${DEPLOY_DIR}/"

        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            $<$<CONFIG:Debug>:--debug>
            $<$<CONFIG:Release>:--release>
            --no-compiler-runtime
            --dir "${DEPLOY_DIR}"
            "$<TARGET_FILE:SLV>"

        COMMENT "📦 Génération du package portable Qt dans /dist"
    )

endif()

if(WIN32)
set_target_properties(SLV PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)
endif()

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# =========================================================
# Python scripts copy (cross-platform)
# =========================================================
if(NOT APPLE)
    add_custom_command(TARGET SLV POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PYTHON_SCRIPTS_DIR}"
        "$<TARGET_FILE_DIR:SLV>/pyScripts"
    )
endif()

# =========================================================
# Install rules
# =========================================================
include(GNUInstallDirs)

install(TARGETS SLV
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# =========================
# WINDOWS LOGO
# =========================
if(WIN32)
    target_sources(SLV PRIVATE appicon.rc)
endif()

# =========================
# MACOS LOGO
# =========================
if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE numalyse_player_logo.icns)

    set_source_files_properties(Ressources/logo/numalyse_player_logo.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")

    target_sources(SLV PRIVATE Ressources/logo/numalyse_player_logo.icns)
endif()

# =========================================================
# Qt finalization
# =========================================================
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SLV)
endif()