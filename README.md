# Edge Vision AI Demo Kit

![CI](https://github.com/sunshiqin3630-ship-it/edge-vision-ai-demo-kit/actions/workflows/ci.yml/badge.svg)

A sanitized reference kit for embedded Linux edge-vision demos.

The project captures a reusable architecture for camera input, display output,
model selection, and accelerator backends without publishing vendor SDKs,
board credentials, generated model artifacts, or runtime bundles.

## What This Repository Contains

- A small Python reference pipeline with a deterministic `mock` backend.
- A model manifest schema that records source repository, revision, license,
  input shape, and artifact distribution status.
- A sanitization scanner that rejects generated model files, binary runtime
  files, lab addresses, password shortcuts, and vendor-specific SDK names.
- Documentation for adapting the architecture to Qt, V4L2, FFmpeg, DRM/KMS,
  and private accelerator backends.
- Open-source project materials: contribution guide, security policy, license,
  environment template, and application summary.

## What This Repository Does Not Contain

- Vendor accelerator SDK source, headers, libraries, tools, or examples.
- Generated model files such as engine blobs, weights, runtime archives, or
  board-specific binary packages.
- Lab IP addresses, root passwords, board-specific deployment paths, or
  personal gallery images.
- Private runtime bundles copied from target devices.

## Quick Start

Run the reference tests:

```bash
PYTHONPATH=src python3 -m unittest discover -s tests -v
```

Validate the example manifest:

```bash
PYTHONPATH=src python3 -m edge_vision_ai_demo_kit.cli validate-manifest models/manifest.example.json
```

Run the mock pipeline once:

```bash
PYTHONPATH=src python3 -m edge_vision_ai_demo_kit.cli run-once --backend mock --source sample://frame --model yolo11n-detect
```

Run the repository sanitization gate:

```bash
PYTHONPATH=src python3 scripts/check_sanitization.py
```

## Architecture

The public architecture is split into four boundaries:

1. **Capture**: RTSP, file, or UVC camera input. The reference package models
   this as a source string; production ports can map it to FFmpeg or V4L2.
2. **Inference backend**: A small interface that accepts a model id and frame
   metadata. Public builds include only `mock`; hardware backends should live
   in private adapter repositories.
3. **Overlay**: Detections are represented in display coordinates so a Qt,
   DRM/KMS, OpenGL, or CPU overlay layer can consume them.
4. **Manifest**: Every model is described by upstream source, fixed revision,
   license, input metadata, and artifact distribution status.

See [docs/architecture.md](docs/architecture.md) for the detailed design.

## Project Maintenance

- [Roadmap](docs/roadmap.md)
- [Contributing guide](CONTRIBUTING.md)
- [Security policy](SECURITY.md)
- [Sanitization policy](docs/sanitization-policy.md)

## Repository Layout

```text
src/edge_vision_ai_demo_kit/  Reference package
tests/                       Standard-library tests
scripts/                     Repository checks
models/                      Manifest examples only
docs/                        Architecture, backend, and release guidance
examples/                    Environment and service templates
```

## Porting A Private Backend

Keep private accelerator code outside this repository. A backend adapter should:

- implement the same input/output contract as `MockBackend`;
- read SDK paths from environment variables;
- keep generated engines and runtime libraries out of Git;
- provide its own hardware smoke tests in a private repository or CI job;
- publish only generic status, latency, and detection metadata here.

See [docs/porting-accelerator-backend.md](docs/porting-accelerator-backend.md).

## License

Apache License 2.0. See [LICENSE](LICENSE).
