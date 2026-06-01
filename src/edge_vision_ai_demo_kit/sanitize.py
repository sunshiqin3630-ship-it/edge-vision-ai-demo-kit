from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import re


def _join(*parts: str) -> str:
    return "".join(parts)


DISALLOWED_EXTENSIONS = {
    ".a",
    ".bin",
    ".engine",
    ".img",
    ".npy",
    ".onnx",
    ".pt",
    ".pth",
    ".slz",
    ".so",
    ".whl",
    ".zip",
}

IGNORED_DIRS = {
    ".git",
    ".mypy_cache",
    ".pytest_cache",
    "__pycache__",
    "build",
    "dist",
    "install",
    _join("private", "_bundle"),
    "qt" + "-sysroot",
    "tmp",
}

TEXT_PATTERNS = [
    (re.compile(re.escape(_join("zq", "500")), re.IGNORECASE), "vendor board name"),
    (re.compile(re.escape("/opt/" + _join("zq", "500")), re.IGNORECASE), "vendor SDK path"),
    (re.compile(re.escape(_join("Deep", "Toolkit")), re.IGNORECASE), "vendor SDK tree"),
    (re.compile(re.escape(_join("ssh", "pass") + " -p root"), re.IGNORECASE), "embedded password shortcut"),
    (re.compile(r"root@\d{1,3}(?:\.\d{1,3}){3}", re.IGNORECASE), "root SSH target"),
    (re.compile(r"192\.168\.\d{1,3}\.\d{1,3}"), "private lab address"),
]


@dataclass(frozen=True)
class Finding:
    path: Path
    pattern: str
    reason: str


@dataclass(frozen=True)
class ScanReport:
    root: Path
    findings: list[Finding]

    @property
    def has_findings(self) -> bool:
        return bool(self.findings)


def _should_skip(path: Path, root: Path) -> bool:
    rel = path.relative_to(root)
    return any(part in IGNORED_DIRS for part in rel.parts)


def _is_probably_text(path: Path) -> bool:
    return path.suffix.lower() in {
        "",
        ".cfg",
        ".cmake",
        ".conf",
        ".cpp",
        ".env",
        ".h",
        ".ini",
        ".json",
        ".md",
        ".pro",
        ".py",
        ".sh",
        ".txt",
        ".yaml",
        ".yml",
    }


def scan_path(root: str | Path) -> ScanReport:
    root_path = Path(root).resolve()
    findings: list[Finding] = []

    for path in root_path.rglob("*"):
        if _should_skip(path, root_path) or path.is_dir():
            continue

        suffix = path.suffix.lower()
        if suffix in DISALLOWED_EXTENSIONS:
            findings.append(
                Finding(
                    path=path,
                    pattern=suffix,
                    reason="generated model, library, archive, or binary artifact",
                )
            )
            continue

        if not _is_probably_text(path):
            continue

        try:
            text = path.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue

        for pattern, reason in TEXT_PATTERNS:
            if pattern.search(text):
                findings.append(Finding(path=path, pattern=pattern.pattern, reason=reason))

    return ScanReport(root=root_path, findings=findings)
