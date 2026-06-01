from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[1]


class RepositoryHealthTests(unittest.TestCase):
    def test_readme_links_maintenance_and_application_evidence(self) -> None:
        readme = (ROOT / "README.md").read_text(encoding="utf-8")

        self.assertIn("MAINTENANCE.md", readme)
        self.assertIn("docs/openai-oss-application.md", readme)

    def test_maintenance_playbook_covers_oss_review_signals(self) -> None:
        playbook_path = ROOT / "MAINTENANCE.md"
        self.assertTrue(playbook_path.exists(), "missing MAINTENANCE.md")
        playbook = playbook_path.read_text(encoding="utf-8")

        for expected in [
            "Issue Triage",
            "Pull Request Review",
            "Release Checklist",
            "Sanitization Gate",
            "local board SDK compile/link verification",
        ]:
            with self.subTest(expected=expected):
                self.assertIn(expected, playbook)

    def test_openai_application_notes_record_active_maintenance_evidence(self) -> None:
        notes = (ROOT / "docs" / "openai-oss-application.md").read_text(
            encoding="utf-8"
        )

        for expected in [
            "Evidence Of Active Maintenance",
            "pull request review",
            "issue triage",
            "release management",
            "local board SDK compile/link verification",
        ]:
            with self.subTest(expected=expected):
                self.assertIn(expected, notes)

        self.assertNotIn("offline_sdk", notes)
        self.assertNotIn("/home/", notes)
        self.assertNotIn("192.168.", notes)
        self.assertNotIn("zq" + "500", notes.lower())


if __name__ == "__main__":
    unittest.main()
