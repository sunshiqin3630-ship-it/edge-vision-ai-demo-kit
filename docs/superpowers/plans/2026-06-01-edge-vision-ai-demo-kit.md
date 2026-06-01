# Edge Vision AI Demo Kit Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Create a publishable open-source skeleton for an embedded Linux edge-vision demo kit without exposing private accelerator details.

**Architecture:** The public repo contains a standard-library Python reference package, a mock backend, model manifest validation, sanitization checks, and documentation. Hardware-specific implementations remain private adapters behind the backend contract.

**Tech Stack:** Python standard library, unittest, JSON manifest files, Markdown documentation, shell-compatible environment templates.

---

## Success Criteria

- [x] Public package runs without private SDKs or board access.
- [x] Tests cover manifest parsing, mock pipeline execution, and sanitization scanning.
- [x] Sanitization scanner rejects binary/model artifacts and sensitive deployment strings.
- [x] Documentation explains architecture, backend porting, sanitization policy, and application positioning.
- [x] Example manifest records source, revision, license, input metadata, and artifact distribution status.

## Verification

```bash
PYTHONPATH=src python3 -m unittest discover -s tests -v
PYTHONPATH=src python3 scripts/check_sanitization.py
```
