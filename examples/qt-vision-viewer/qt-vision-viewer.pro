QT += core gui widgets

CONFIG += c++17 warn_on thread
TEMPLATE = app
TARGET = qt-vision-viewer

INCLUDEPATH += $$PWD/src

CROSS_COMPILE_PREFIX = $$(CROSS_COMPILE)
isEmpty(CROSS_COMPILE_PREFIX) {
    CROSS_COMPILE_PREFIX = $$CROSS_COMPILE
}
!isEmpty(CROSS_COMPILE_PREFIX) {
    QMAKE_CC = $${CROSS_COMPILE_PREFIX}gcc
    QMAKE_CXX = $${CROSS_COMPILE_PREFIX}g++
    QMAKE_LINK = $$QMAKE_CXX
    QMAKE_LINK_SHLIB = $$QMAKE_CXX
    QMAKE_LINK_C = $$QMAKE_CC
    QMAKE_LINK_C_SHLIB = $$QMAKE_CC
    QMAKE_AR = $${CROSS_COMPILE_PREFIX}ar cqs
    QMAKE_OBJCOPY = $${CROSS_COMPILE_PREFIX}objcopy
    QMAKE_NM = $${CROSS_COMPILE_PREFIX}nm -P
    QMAKE_STRIP = $${CROSS_COMPILE_PREFIX}strip
}

SOURCES += \
    src/display_router.cpp \
    src/ffmpeg_stream_source.cpp \
    src/main.cpp \
    src/main_window.cpp \
    src/synthetic_frame_source.cpp \
    src/v4l2_camera_source.cpp \
    src/vision_canvas.cpp

HEADERS += \
    src/display_router.h \
    src/ffmpeg_stream_source.h \
    src/frame_source.h \
    src/main_window.h \
    src/synthetic_frame_source.h \
    src/v4l2_camera_source.h \
    src/vision_canvas.h

EDGE_VIEWER_ENABLE_FFMPEG = $$(EDGE_VIEWER_ENABLE_FFMPEG)
contains(EDGE_VIEWER_ENABLE_FFMPEG, ^(1|ON|on|true)$) {
    FFMPEG_ROOT = $$(FFMPEG_ROOT)
    isEmpty(FFMPEG_ROOT) {
        error("FFMPEG_ROOT is required when EDGE_VIEWER_ENABLE_FFMPEG is enabled")
    }

    FFMPEG_INCLUDE_DIR = $$FFMPEG_ROOT/include
    FFMPEG_LIB_DIR = $$FFMPEG_ROOT/lib

    !exists($$FFMPEG_INCLUDE_DIR/libavformat/avformat.h) {
        error("FFmpeg headers not found under $$FFMPEG_INCLUDE_DIR")
    }

    DEFINES += EDGE_VIEWER_HAS_FFMPEG=1
    INCLUDEPATH += $$FFMPEG_INCLUDE_DIR
    QMAKE_LFLAGS += -Wl,-rpath-link,$$FFMPEG_LIB_DIR
    LIBS += -L$$FFMPEG_LIB_DIR
    LIBS += -lavformat -lavcodec -lswscale -lswresample -lavutil -lx264 -lx265
    LIBS += -ldl -lpthread -lm
}

EDGE_VIEWER_ENABLE_V4L2 = $$(EDGE_VIEWER_ENABLE_V4L2)
contains(EDGE_VIEWER_ENABLE_V4L2, ^(1|ON|on|true)$) {
    DEFINES += EDGE_VIEWER_HAS_V4L2=1
}
