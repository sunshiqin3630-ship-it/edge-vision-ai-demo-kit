cmake_minimum_required(VERSION 3.16)

project(qt_vision_viewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

set(EDGE_VIEWER_QT_MAJOR "auto" CACHE STRING "Qt major version to use: auto, 5, or 6")
set_property(CACHE EDGE_VIEWER_QT_MAJOR PROPERTY STRINGS auto 5 6)
option(EDGE_VIEWER_ENABLE_FFMPEG "Enable FFmpeg stream capture backend" OFF)
option(EDGE_VIEWER_ENABLE_V4L2 "Enable Linux V4L2 UVC capture backend" OFF)

if(EDGE_VIEWER_QT_MAJOR STREQUAL "5")
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
    set(QT_VERSION_MAJOR 5)
elseif(EDGE_VIEWER_QT_MAJOR STREQUAL "6")
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
    set(QT_VERSION_MAJOR 6)
else()
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)
endif()

add_executable(qt-vision-viewer
    src/display_router.cpp
    src/display_router.h
    src/ffmpeg_stream_source.cpp
    src/ffmpeg_stream_source.h
    src/frame_source.h
    src/main.cpp
    src/main_window.cpp
    src/main_window.h
    src/synthetic_frame_source.cpp
    src/synthetic_frame_source.h
    src/v4l2_camera_source.cpp
    src/v4l2_camera_source.h
    src/vision_canvas.cpp
    src/vision_canvas.h
)

target_link_libraries(qt-vision-viewer PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)

if(EDGE_VIEWER_ENABLE_FFMPEG)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
        libavcodec
        libavformat
        libavutil
        libswscale
    )
    target_compile_definitions(qt-vision-viewer PRIVATE EDGE_VIEWER_HAS_FFMPEG=1)
    target_link_libraries(qt-vision-viewer PRIVATE PkgConfig::FFMPEG)
endif()

if(EDGE_VIEWER_ENABLE_V4L2)
    include(CheckIncludeFileCXX)
    check_include_file_cxx("linux/videodev2.h" EDGE_VIEWER_HAS_LINUX_VIDEODEV2)
    if(NOT EDGE_VIEWER_HAS_LINUX_VIDEODEV2)
        message(FATAL_ERROR "linux/videodev2.h is required for EDGE_VIEWER_ENABLE_V4L2=ON")
    endif()
    target_compile_definitions(qt-vision-viewer PRIVATE EDGE_VIEWER_HAS_V4L2=1)
endif()
