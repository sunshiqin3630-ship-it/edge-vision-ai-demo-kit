from __future__ import annotations

import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[1]
EXAMPLE = ROOT / "examples" / "qt-vision-viewer"


class QtExampleTests(unittest.TestCase):
    def test_qt_viewer_skeleton_has_public_entry_points(self) -> None:
        expected = [
            EXAMPLE / "CMakeLists.txt",
            EXAMPLE / "README.md",
            EXAMPLE / "data" / "detections.example.json",
            EXAMPLE / "src" / "main.cpp",
        ]

        for path in expected:
            with self.subTest(path=path):
                self.assertTrue(path.exists(), f"missing {path.relative_to(ROOT)}")

    def test_qt_viewer_uses_sanitized_mock_data(self) -> None:
        data_path = EXAMPLE / "data" / "detections.example.json"
        self.assertTrue(data_path.exists(), f"missing {data_path.relative_to(ROOT)}")
        data = json.loads(data_path.read_text())

        self.assertIn("frame", data)
        self.assertIn("detections", data)
        self.assertGreater(len(data["detections"]), 0)
        for detection in data["detections"]:
            self.assertIn("label", detection)
            self.assertIn("score", detection)
            self.assertEqual(len(detection["bbox"]), 4)
            self.assertTrue(all(isinstance(value, (int, float)) for value in detection["bbox"]))

    def test_qt_viewer_documents_private_adapter_boundary(self) -> None:
        readme_path = EXAMPLE / "README.md"
        self.assertTrue(readme_path.exists(), f"missing {readme_path.relative_to(ROOT)}")
        readme = readme_path.read_text(encoding="utf-8")
        self.assertIn("mock detections", readme)
        self.assertIn("private adapter", readme)
        self.assertIn("does not include", readme)


if __name__ == "__main__":
    unittest.main()
