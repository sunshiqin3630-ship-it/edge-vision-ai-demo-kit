# Architecture

Edge Vision AI Demo Kit is a public reference architecture for embedded Linux
camera demos with optional AI acceleration. The repository deliberately keeps
hardware-specific details behind adapter boundaries.

## Data Flow

```text
camera source -> frame metadata -> inference backend -> detections -> overlay/display
```

The public package implements only the metadata and backend contract. A board
port can replace each boundary with production implementations:

- capture: FFmpeg, V4L2, GStreamer, file replay, or synthetic frames;
- backend: CPU, mock, or a private accelerator adapter;
- display: Qt Widgets, DRM/KMS planes, OpenGL, Wayland, or framebuffer output;
- overlay: host drawing, display-plane composition, or device-side drawing.

## Backend Boundary

The backend accepts:

- model id;
- source name;
- frame width and height.

It returns detections in display coordinates:

- label;
- confidence;
- x/y/width/height.

The public `mock` backend is deterministic and does not load any SDK. Private
backends can map this same contract to hardware decoders, model runtimes, or
device-side preprocessing.

## Model Manifest

Every model must be tracked in a manifest entry with:

- public source repository;
- fixed revision;
- license;
- input dimensions and format;
- artifact path and distribution flag.

Generated artifacts are not committed. The manifest records how they should be
produced and where local builds may place them.

## Sanitization Gate

The repository scanner rejects:

- generated model artifacts and runtime binaries;
- local board archives and sysroots;
- lab network addresses;
- root SSH shortcuts and embedded passwords;
- vendor SDK names or fixed install paths.

Run `PYTHONPATH=src python3 scripts/check_sanitization.py` before publishing.
