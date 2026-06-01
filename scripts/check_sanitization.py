#!/usr/bin/env python3
from __future__ import annotations

from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "src"
if str(SRC) not in sys.path:
    sys.path.insert(0, str(SRC))

from edge_vision_ai_demo_kit.sanitize import scan_path  # noqa: E402


def main() -> int:
    report = scan_path(ROOT)
    if not report.has_findings:
        print("sanitization ok")
        return 0

    for finding in report.findings:
        rel = finding.path.relative_to(report.root)
        print(f"{rel}: {finding.reason} ({finding.pattern})")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
