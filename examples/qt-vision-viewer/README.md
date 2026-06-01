# Qt Vision Viewer Example

This is a sanitized Qt Widgets skeleton for displaying a synthetic video frame
with mock detections. It demonstrates the public UI and overlay boundary that a
private adapter can feed from another repository.

The example does not include camera drivers, vendor SDK code, accelerator
runtimes, generated model artifacts, board credentials, lab hosts, or deployment
paths.

## Build

Install Qt 6 development packages, then build the example:

```bash
cmake -S examples/qt-vision-viewer -B build/qt-vision-viewer
cmake --build build/qt-vision-viewer
```

## Run

```bash
./build/qt-vision-viewer/qt-vision-viewer \
  --detections examples/qt-vision-viewer/data/detections.example.json
```

If no detection file is provided, the app falls back to built-in mock
detections.

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
