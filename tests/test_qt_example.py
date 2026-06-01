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
            EXAMPLE / "cmake" / "board-linux-toolchain.example.cmake",
            EXAMPLE / "config" / "board.env.example",
            EXAMPLE / "config" / "board-system.example.json",
            EXAMPLE / "config" / "runtime.example.json",
            EXAMPLE / "data" / "detections.example.json",
            EXAMPLE / "docs" / "board-system-profile.md",
            EXAMPLE / "docs" / "local-build-verification.md",
            EXAMPLE / "qt-vision-viewer.pro",
            EXAMPLE / "scripts" / "build-board-example.sh",
            EXAMPLE / "src" / "display_router.cpp",
            EXAMPLE / "src" / "display_router.h",
            EXAMPLE / "src" / "ffmpeg_stream_source.cpp",
            EXAMPLE / "src" / "ffmpeg_stream_source.h",
            EXAMPLE / "src" / "frame_source.h",
            EXAMPLE / "src" / "main.cpp",
            EXAMPLE / "src" / "main_window.cpp",
            EXAMPLE / "src" / "main_window.h",
            EXAMPLE / "src" / "synthetic_frame_source.cpp",
            EXAMPLE / "src" / "synthetic_frame_source.h",
            EXAMPLE / "src" / "v4l2_camera_source.cpp",
            EXAMPLE / "src" / "v4l2_camera_source.h",
            EXAMPLE / "src" / "vision_canvas.cpp",
            EXAMPLE / "src" / "vision_canvas.h",
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

    def test_qt_viewer_exposes_capture_and_display_architecture(self) -> None:
        paths = {
            "cmake": EXAMPLE / "CMakeLists.txt",
            "readme": EXAMPLE / "README.md",
            "main_window": EXAMPLE / "src" / "main_window.cpp",
            "display_router": EXAMPLE / "src" / "display_router.cpp",
        }
        for path in paths.values():
            self.assertTrue(path.exists(), f"missing {path.relative_to(ROOT)}")

        cmake = paths["cmake"].read_text(encoding="utf-8")
        qmake = (EXAMPLE / "qt-vision-viewer.pro").read_text(encoding="utf-8")
        readme = paths["readme"].read_text(encoding="utf-8")
        main_window = paths["main_window"].read_text(encoding="utf-8")
        display_router = paths["display_router"].read_text(encoding="utf-8")

        self.assertIn("EDGE_VIEWER_ENABLE_FFMPEG", cmake)
        self.assertIn("EDGE_VIEWER_ENABLE_V4L2", cmake)
        self.assertIn("EDGE_VIEWER_ENABLE_FFMPEG", qmake)
        self.assertIn("EDGE_VIEWER_ENABLE_V4L2", qmake)
        self.assertIn("EDGE_VIEWER_HAS_FFMPEG", qmake)
        self.assertIn("EDGE_VIEWER_HAS_V4L2", qmake)
        self.assertIn("CROSS_COMPILE", qmake)
        self.assertIn("QMAKE_CC", qmake)
        self.assertIn("QMAKE_CXX", qmake)
        self.assertIn("QMAKE_LINK", qmake)
        self.assertIn("EDGE_VIEWER_QT_MAJOR", cmake)
        self.assertIn("find_package(QT NAMES Qt6 Qt5", cmake)
        self.assertIn("Qt${QT_VERSION_MAJOR}::Widgets", cmake)
        self.assertIn("FfmpegStreamSource", main_window)
        self.assertIn("V4l2CameraSource", main_window)
        self.assertIn("QGuiApplication::screens", display_router)
        self.assertIn("dual-screen", readme)
        self.assertIn("FFmpeg", readme)
        self.assertIn("V4L2", readme)

    def test_qt_viewer_includes_board_build_configuration_examples(self) -> None:
        env = (EXAMPLE / "config" / "board.env.example").read_text(encoding="utf-8")
        toolchain = (EXAMPLE / "cmake" / "board-linux-toolchain.example.cmake").read_text(
            encoding="utf-8"
        )
        build_script = (EXAMPLE / "scripts" / "build-board-example.sh").read_text(
            encoding="utf-8"
        )
        runtime = json.loads((EXAMPLE / "config" / "runtime.example.json").read_text())
        board_system = json.loads(
            (EXAMPLE / "config" / "board-system.example.json").read_text()
        )

        self.assertIn("BOARD_QT_MAJOR=auto", env)
        self.assertIn("BOARD_QT_CMAKE_PREFIX=", env)
        self.assertIn("BOARD_SYSROOT=", env)
        self.assertIn("BOARD_TOOLCHAIN_PREFIX=", env)
        self.assertIn("EDGE_VIEWER_PRIMARY_CONNECTOR=", env)
        self.assertIn("EDGE_VIEWER_SECONDARY_CONNECTOR=", env)
        self.assertIn("CMAKE_SYSROOT", toolchain)
        self.assertIn("CMAKE_PREFIX_PATH", toolchain)
        self.assertIn("EDGE_VIEWER_QT_MAJOR", build_script)
        self.assertEqual(runtime["sourceProfiles"][0]["type"], "synthetic")
        self.assertEqual(runtime["sourceProfiles"][1]["type"], "ffmpeg")
        self.assertEqual(runtime["sourceProfiles"][2]["type"], "v4l2")
        self.assertEqual(runtime["display"]["mode"], "dual-screen")
        self.assertEqual(board_system["qt"]["major"], "auto")
        self.assertIn("eglfs", board_system["qt"]["qpaPlugins"])
        self.assertEqual(board_system["displayResources"][0]["role"], "operator-ui")
        self.assertEqual(board_system["displayResources"][1]["role"], "vision-output")
        self.assertIn("drmConnector", board_system["displayResources"][0])
        self.assertIn("qtScreenName", board_system["displayResources"][1])
        self.assertEqual(board_system["captureResources"]["uvc"][0]["device"], "/dev/video0")

    def test_qt_viewer_documents_how_to_collect_board_system_profile(self) -> None:
        guide = (EXAMPLE / "docs" / "board-system-profile.md").read_text(encoding="utf-8")

        self.assertIn("qtpaths --version", guide)
        self.assertIn("qmake -v", guide)
        self.assertIn("/sys/class/drm", guide)
        self.assertIn("v4l2-ctl --list-devices", guide)
        self.assertIn("ffmpeg -protocols", guide)
        self.assertIn("Do not commit", guide)

    def test_qt_viewer_records_local_build_verification_without_private_details(self) -> None:
        record_path = EXAMPLE / "docs" / "local-build-verification.md"
        self.assertTrue(record_path.exists(), f"missing {record_path.relative_to(ROOT)}")
        record = record_path.read_text(encoding="utf-8")

        self.assertIn("completed on maintainer local machine", record)
        self.assertIn("Qt 5.15.x", record)
        self.assertIn("qmake", record)
        self.assertIn("linux-buildroot-g++", record)
        self.assertIn("aarch64", record)
        self.assertIn("FFmpeg", record)
        self.assertIn("V4L2", record)
        self.assertNotIn("offline_sdk", record)
        self.assertNotIn("/home/", record)
        self.assertNotIn("192.168.", record)
        self.assertNotIn("zq" + "500", record.lower())


if __name__ == "__main__":
    unittest.main()
