# cleanup_opencv_bundle.cmake
# This script removes duplicate OpenCV dylib files from the macOS app bundle Frameworks,
# keeping only the .414.dylib versions that match the app's @rpath dependencies.

# Get the build directory from environment or fallback
if(NOT DEFINED CMAKE_BINARY_DIR)
    set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_LIST_DIR}/build")
endif()

set(FRAMEWORKS_DIR "${CMAKE_BINARY_DIR}/NumalysePlayer.app/Contents/Frameworks")

if(NOT EXISTS "${FRAMEWORKS_DIR}")
    message(STATUS "Frameworks directory does not exist yet: ${FRAMEWORKS_DIR}")
    return()
endif()

message(STATUS "Cleaning up duplicate OpenCV dylibs in: ${FRAMEWORKS_DIR}")

# Find all OpenCV dylib files
file(GLOB ALL_OPENCV_DYLIBS "${FRAMEWORKS_DIR}/libopencv_*.dylib")

foreach(_dylib IN LISTS ALL_OPENCV_DYLIBS)
    get_filename_component(_filename "${_dylib}" NAME)
    
    # Keep only .414.dylib versions
    if(NOT _filename MATCHES "\.414\.dylib$")
        if(EXISTS "${_dylib}")
            message(STATUS "Removing duplicate: ${_filename}")
            file(REMOVE "${_dylib}")
        endif()
    endif()
endforeach()

message(STATUS "OpenCV bundle cleanup complete")
