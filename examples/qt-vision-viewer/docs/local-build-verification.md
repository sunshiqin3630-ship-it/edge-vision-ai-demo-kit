# Local Build Verification

Status: completed on maintainer local machine on 2026-06-01.

The Qt viewer was compile-tested with a private embedded Linux board SDK using
only the public example sources in this repository. The public record is
intentionally sanitized and does not include SDK paths, board credentials,
internal project names, lab hosts, generated model artifacts, or runtime
libraries.

Verified build shape:

- Qt 5.15.x, verified with Qt 5.15.11.
- qmake 3.1 with the `devices/linux-buildroot-g++` mkspec.
- aarch64 cross compiler selected through a generic `CROSS_COMPILE` prefix.
- `EDGE_VIEWER_ENABLE_FFMPEG=1`.
- `EDGE_VIEWER_ENABLE_V4L2=1`.
- FFmpeg and V4L2 capture code compiled and linked into the viewer.

Generic command shape:

```bash
env CROSS_COMPILE=/path/to/aarch64-toolchain/bin/aarch64-linux-gnu- \
  EDGE_VIEWER_ENABLE_FFMPEG=1 \
  EDGE_VIEWER_ENABLE_V4L2=1 \
  FFMPEG_ROOT=/path/to/board-sdk/deps/ffmpeg \
  /path/to/qt-host/bin/qmake \
  -o Makefile \
  ../../examples/qt-vision-viewer/qt-vision-viewer.pro \
  -spec devices/linux-buildroot-g++

make -j2
```

This verification covers host-side cross-compilation and link success only. It
does not claim target-board runtime validation, camera validation, display
bring-up, or accelerator integration.
