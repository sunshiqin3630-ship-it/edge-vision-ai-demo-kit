from pathlib import Path
import tempfile
import unittest

from edge_vision_ai_demo_kit.sanitize import scan_path


class SanitizeTests(unittest.TestCase):
    def test_scan_path_reports_banned_vendor_terms(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            source = Path(tmp) / "README.md"
            vendor_path = "/opt/" + "zq" + "500" + "/sdk"
            source.write_text(f"runtime lives under {vendor_path}\n", encoding="utf-8")

            report = scan_path(Path(tmp))

            self.assertTrue(report.has_findings)
            self.assertTrue(
                any("zq" + "500" in finding.pattern.lower() for finding in report.findings)
            )

    def test_scan_path_reports_disallowed_binary_extensions(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            model = Path(tmp) / "model.engine"
            model.write_bytes(b"private engine")

            report = scan_path(Path(tmp))

            self.assertTrue(report.has_findings)
            self.assertTrue(any(finding.path.name == "model.engine" for finding in report.findings))


if __name__ == "__main__":
    unittest.main()
