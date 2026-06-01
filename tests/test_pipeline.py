import unittest

from edge_vision_ai_demo_kit.pipeline import PipelineConfig, run_once


class PipelineTests(unittest.TestCase):
    def test_run_once_uses_mock_backend_without_private_sdk(self) -> None:
        result = run_once(
            PipelineConfig(
                backend="mock",
                source="sample://frame",
                model_id="yolo11n-detect",
                frame_width=1280,
                frame_height=720,
            )
        )

        self.assertEqual(result.backend, "mock")
        self.assertEqual(result.model_id, "yolo11n-detect")
        self.assertEqual(len(result.detections), 1)
        self.assertEqual(result.detections[0].label, "person")
        self.assertGreaterEqual(result.detections[0].confidence, 0.0)
        self.assertLessEqual(result.detections[0].confidence, 1.0)


if __name__ == "__main__":
    unittest.main()
