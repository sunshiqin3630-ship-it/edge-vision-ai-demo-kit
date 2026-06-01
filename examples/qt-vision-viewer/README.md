# Qt Vision Viewer Example

This is a sanitized Qt Widgets skeleton for displaying a synthetic video frame,
FFmpeg stream frames, or V4L2 UVC camera frames with mock detections. It
demonstrates the public UI, capture, overlay, and dual-screen display boundary
that a private adapter can feed from another repository.

The example does not include vendor SDK code, accelerator runtimes, generated
model artifacts, board credentials, lab hosts, private deployment paths, or
board-specific binary bundles.

## Build

Install the same Qt major version that your target system uses, then build the
example. `EDGE_VIEWER_QT_MAJOR=auto` first searches Qt 6, then Qt 5.

```bash
cmake -S examples/qt-vision-viewer -B build/qt-vision-viewer \
  -DEDGE_VIEWER_QT_MAJOR=auto
cmake --build build/qt-vision-viewer
```

Enable optional capture backends when the matching development packages are
available:

```bash
cmake -S examples/qt-vision-viewer -B build/qt-vision-viewer \
  -DEDGE_VIEWER_ENABLE_FFMPEG=ON \
  -DEDGE_VIEWER_ENABLE_V4L2=ON
```

For a board cross-build, use the sanitized environment and toolchain examples:

```bash
examples/qt-vision-viewer/scripts/build-board-example.sh \
  examples/qt-vision-viewer/config/board.env.example
```

## Run

```bash
./build/qt-vision-viewer/qt-vision-viewer \
  --detections examples/qt-vision-viewer/data/detections.example.json \
  --runtime-config examples/qt-vision-viewer/config/runtime.example.json
```

If no detection file is provided, the app falls back to built-in mock
detections.

## Capture Profiles

- `synthetic`: generated frames for CI, demos, and UI validation.
- `ffmpeg`: RTSP, file, or network streams through FFmpeg when
  `EDGE_VIEWER_ENABLE_FFMPEG=ON`.
- `v4l2`: UVC camera capture through `/dev/video*` when
  `EDGE_VIEWER_ENABLE_V4L2=ON`.

Use placeholders in public examples, then override them locally on the board.

## Board System Configuration

- `config/board.env.example`: sanitized build and runtime environment values,
  including Qt major selection, sysroot, toolchain prefix, QPA platform, screen
  connectors, and capture source placeholders.
- `config/board-system.example.json`: sanitized board resource map for Qt screen
  names, DRM connectors, screen geometry, UVC devices, and FFmpeg stream
  profiles.
- `config/runtime.example.json`: app-level source and dual-screen display
  profile.
- `docs/board-system-profile.md`: commands for collecting Qt, DRM/KMS,
  framebuffer, V4L2, and FFmpeg capabilities from a board, then sanitizing the
  result before publication.

The screen resource examples use generic connector names such as `HDMI-A-1` and
`DSI-1`. Replace them locally with the names reported by your board system, but
do not commit private board dumps or lab-only values.

## Dual-Screen

The main window is the operator UI. When dual-screen mode is enabled and Qt
reports at least two screens, the secondary vision output window is moved to the
second screen and shown full-screen. If only one screen is available, the
secondary window stays hidden.

## Adapter Boundary

Keep hardware-specific capture, model loading, inference, and runtime setup in
a private adapter outside this repository. The public viewer only needs generic
detection metadata:

```json
{
  "frame": {"width": 1280, "height": 720, "source": "sample://synthetic-frame"},
  "detections": [
    {"label": "person", "score": 0.92, "bbox": [0.14, 0.18, 0.22, 0.50]}
  ]
}
```

`bbox` uses normalized `[x, y, width, height]` coordinates relative to the frame.
