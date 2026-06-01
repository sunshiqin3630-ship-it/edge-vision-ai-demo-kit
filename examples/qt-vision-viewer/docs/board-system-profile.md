# Board System Profile

Use this guide on a target embedded Linux board to collect the public resource
shape needed by the Qt viewer. Do not commit raw command output if it contains
private SDK paths, credentials, lab-only hosts, internal project names, personal
data, or proprietary device identifiers. Convert findings into sanitized values
in `config/board-system.example.json` or a private local override.

## Qt Runtime

Check which Qt major version is installed on the board. Build this example with
the same major version.

```bash
qtpaths --version
qmake -v
find "$QT_PLUGIN_PATH" -maxdepth 2 -type f 2>/dev/null | sort
```

Record only the major version, QPA plugin family, and generic CMake package
prefix in public examples:

```text
BOARD_QT_MAJOR=auto
QT_QPA_PLATFORM=eglfs
BOARD_QT_CMAKE_PREFIX=/path/to/board/sysroot/usr/lib/cmake
```

## Screen Resources

Collect DRM/KMS connector names, framebuffer sizes, and Qt screen names. Replace
actual board-specific labels with generic connector names before committing.

```bash
ls -1 /sys/class/drm
for status in /sys/class/drm/*/status; do
  printf "%s " "$status"
  cat "$status"
done

for mode in /sys/class/drm/*/modes; do
  printf "%s\n" "$mode"
  head -n 5 "$mode"
done

for size in /sys/class/graphics/fb*/virtual_size; do
  printf "%s " "$size"
  cat "$size"
done
```

If available, these tools provide a cleaner view:

```bash
modetest -c
kmsprint
```

Map the result into sanitized display resources:

```json
{
  "displayResources": [
    {
      "role": "operator-ui",
      "qtScreenName": "HDMI-A-1",
      "drmConnector": "HDMI-A-1",
      "geometry": {"width": 1920, "height": 1080, "x": 0, "y": 0}
    },
    {
      "role": "vision-output",
      "qtScreenName": "DSI-1",
      "drmConnector": "DSI-1",
      "geometry": {"width": 1280, "height": 720, "x": 1920, "y": 0}
    }
  ]
}
```

## V4L2 UVC Capture

List UVC devices and supported formats:

```bash
v4l2-ctl --list-devices
v4l2-ctl --device=/dev/video0 --list-formats-ext
```

Public examples should use generic device paths and formats:

```json
{
  "type": "v4l2",
  "device": "/dev/video0",
  "format": "YUYV",
  "width": 1280,
  "height": 720,
  "fps": 30
}
```

## FFmpeg Stream Capture

Check FFmpeg protocol and codec support without publishing private stream URLs:

```bash
ffmpeg -version
ffmpeg -protocols
ffmpeg -decoders
```

Use a placeholder stream in public config:

```json
{
  "type": "ffmpeg",
  "url": "rtsp://example-camera.local/live",
  "transport": "tcp"
}
```

## Publication Rule

Do not commit raw board dumps, copied target filesystem content, generated
model artifacts, private runtime bundles, credentials, lab hosts, or internal
project names. Keep the public repository limited to sanitized resource shape
and replace exact local values in a private deployment repository or local env
file.
