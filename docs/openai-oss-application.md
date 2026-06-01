# OpenAI Open Source Maintainer Application Notes

## Project Summary

Edge Vision AI Demo Kit is a public reference toolkit for embedded Linux
edge-vision applications. It documents a reusable architecture for camera input,
AI model selection, accelerator backend adapters, and display overlays while
keeping private board SDKs and generated model artifacts out of the repository.

## Maintainer Role

I maintain the project scope, public interface, documentation, issue triage,
model manifest policy, sanitization checks, and examples. Private hardware
adapters are intentionally kept outside the public repository so the open-source
project remains portable and safe to review.

## Why The Project Fits OSS Support

The project helps embedded Linux developers build reproducible camera and edge
AI demos without leaking board-specific implementation details. It focuses on
common open-source maintenance work: reviewable interfaces, documentation,
testing, model provenance, license tracking, and release hygiene.

## How Codex / ChatGPT Pro Would Help

- Review pull requests for accidental private SDK paths, binary artifacts, and
  credential leaks.
- Improve backend interface design and test coverage.
- Draft model integration notes with license and revision tracking.
- Summarize issue reports from users porting the kit to different boards.
- Generate release notes and migration guides as the public interface evolves.

## API Credit Use Plan

API credits would be used for maintainer automation: sanitization review,
manifest validation, issue triage, documentation updates, and test generation
for backend adapters. The goal is to reduce repetitive review work while keeping
the public repository portable and free of private artifacts.
