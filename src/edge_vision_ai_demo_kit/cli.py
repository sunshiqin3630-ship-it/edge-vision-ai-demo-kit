from __future__ import annotations

import argparse
import json
from pathlib import Path

from .manifest import load_manifest
from .pipeline import PipelineConfig, run_once
from .sanitize import scan_path


def _cmd_validate_manifest(args: argparse.Namespace) -> int:
    manifest = load_manifest(args.path)
    print(f"manifest ok: {len(manifest.models)} model(s)")
    return 0


def _cmd_scan(args: argparse.Namespace) -> int:
    report = scan_path(args.path)
    for finding in report.findings:
        rel = finding.path.relative_to(report.root)
        print(f"{rel}: {finding.reason} ({finding.pattern})")
    return 1 if report.has_findings else 0


def _cmd_run_once(args: argparse.Namespace) -> int:
    result = run_once(
        PipelineConfig(
            backend=args.backend,
            source=args.source,
            model_id=args.model,
            frame_width=args.width,
            frame_height=args.height,
        )
    )
    print(
        json.dumps(
            {
                "backend": result.backend,
                "source": result.source,
                "model": result.model_id,
                "detections": [d.__dict__ for d in result.detections],
            },
            indent=2,
        )
    )
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(prog="edge-vision-kit")
    subcommands = parser.add_subparsers(required=True)

    validate = subcommands.add_parser("validate-manifest")
    validate.add_argument("path", type=Path)
    validate.set_defaults(func=_cmd_validate_manifest)

    scan = subcommands.add_parser("scan")
    scan.add_argument("path", type=Path)
    scan.set_defaults(func=_cmd_scan)

    run = subcommands.add_parser("run-once")
    run.add_argument("--backend", default="mock")
    run.add_argument("--source", default="sample://frame")
    run.add_argument("--model", default="yolo11n-detect")
    run.add_argument("--width", type=int, default=1280)
    run.add_argument("--height", type=int, default=720)
    run.set_defaults(func=_cmd_run_once)
    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()
    return args.func(args)


if __name__ == "__main__":
    raise SystemExit(main())
