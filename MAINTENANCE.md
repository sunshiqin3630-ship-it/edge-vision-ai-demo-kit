# Maintenance Playbook

This playbook documents the public maintenance workflow for Edge Vision AI Demo
Kit. It keeps review, triage, release, and sanitization work visible without
publishing private board SDKs, generated model artifacts, credentials, lab
hosts, or deployment paths.

## Maintainer Responsibilities

- Keep the public architecture portable across embedded Linux boards.
- Keep private accelerator adapters outside this repository.
- Keep the model manifest policy accurate for source, revision, license, input
  metadata, and artifact distribution status.
- Keep Qt, FFmpeg, V4L2, display routing, and board configuration examples
  sanitized and reproducible.

## Issue Triage

- Label bug reports by affected boundary: capture, backend, manifest,
  sanitization, Qt viewer, documentation, or CI.
- Ask reporters to replace board-specific values with placeholders before
  posting logs.
- Convert private paths, lab addresses, credentials, or generated artifact names
  into sanitized reproduction steps.
- Close reports that require publishing vendor SDK code or binary runtime
  bundles in this public repository.

## Pull Request Review

- Confirm the change builds and tests without private hardware access.
- Run the unit tests, manifest validation, and Sanitization Gate before merge.
- Check that new examples keep SDK paths, stream URLs, model artifacts,
  credentials, and lab hosts out of Git.
- Require public model entries to include source repository, fixed revision,
  license, input metadata, and artifact distribution status.
- Keep private adapter hooks behind documented interfaces rather than importing
  board-specific SDK code.

## Sanitization Gate

Run these checks before publishing changes:

```bash
PYTHONPATH=src python3 -m unittest discover -s tests -v
PYTHONPATH=src python3 -m edge_vision_ai_demo_kit.cli validate-manifest models/manifest.example.json
PYTHONPATH=src python3 scripts/check_sanitization.py
```

The gate rejects generated model files, private runtime libraries, archives,
private lab addresses, password shortcuts, root login targets, and known
vendor-specific terms.

## Release Checklist

- Unit tests pass on the public repository.
- Manifest validation passes for public model metadata.
- Sanitization scan passes.
- README, roadmap, and porting notes reflect any public interface changes.
- Release notes mention only public behavior and sanitized verification status.
- Qt viewer changes that affect board builds update the local board SDK
  compile/link verification record when re-tested.

## Local Board SDK Verification

The Qt viewer has a documented local board SDK compile/link verification in
`examples/qt-vision-viewer/docs/local-build-verification.md`. The public record
is sanitized: it confirms the build shape, Qt/qmake version family, FFmpeg and
V4L2 enablement, and aarch64 cross-compilation through a user-provided
`CROSS_COMPILE` prefix. It does not claim target runtime validation unless that
is separately performed and sanitized.
