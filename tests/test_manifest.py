import json
from pathlib import Path
import tempfile
import unittest

from edge_vision_ai_demo_kit.manifest import load_manifest


class ManifestTests(unittest.TestCase):
    def test_load_manifest_accepts_public_model_entries(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            manifest_path = Path(tmp) / "manifest.json"
            manifest_path.write_text(
                json.dumps(
                    {
                        "models": [
                            {
                                "id": "yolo11n-detect",
                                "task": "object_detection",
                                "source": {
                                    "repo": "https://github.com/ultralytics/ultralytics",
                                    "revision": "v8.4.56",
                                    "license": "AGPL-3.0-or-commercial",
                                },
                                "inputs": {"width": 640, "height": 640, "format": "rgb"},
                                "artifacts": [
                                    {
                                        "type": "onnx",
                                        "path": "models/yolo11n-detect/yolo11n.onnx",
                                        "distributed": False,
                                    }
                                ],
                            }
                        ]
                    }
                ),
                encoding="utf-8",
            )

            manifest = load_manifest(manifest_path)

            self.assertEqual(manifest.models[0].id, "yolo11n-detect")
            self.assertEqual(manifest.models[0].task, "object_detection")
            self.assertIs(manifest.models[0].artifacts[0].distributed, False)

    def test_load_manifest_rejects_missing_source_license(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            manifest_path = Path(tmp) / "manifest.json"
            manifest_path.write_text(
                json.dumps(
                    {
                        "models": [
                            {
                                "id": "face-detection",
                                "task": "face_detection",
                                "source": {
                                    "repo": "https://github.com/opencv/opencv_zoo",
                                    "revision": "a74cdfad334b102cbd8daed769fbe1d4cb1c327a",
                                },
                                "inputs": {"width": 640, "height": 640, "format": "rgb"},
                                "artifacts": [],
                            }
                        ]
                    }
                ),
                encoding="utf-8",
            )

            with self.assertRaisesRegex(ValueError, "source.license"):
                load_manifest(manifest_path)


if __name__ == "__main__":
    unittest.main()
