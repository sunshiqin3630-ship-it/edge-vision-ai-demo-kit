from __future__ import annotations

from dataclasses import dataclass
from typing import Protocol


@dataclass(frozen=True)
class Detection:
    label: str
    confidence: float
    x: int
    y: int
    width: int
    height: int


@dataclass(frozen=True)
class FrameInfo:
    source: str
    width: int
    height: int


class InferenceBackend(Protocol):
    name: str

    def infer(self, model_id: str, frame: FrameInfo) -> list[Detection]:
        """Run one inference step and return display-space detections."""


class MockBackend:
    name = "mock"

    def infer(self, model_id: str, frame: FrameInfo) -> list[Detection]:
        box_width = max(1, frame.width // 5)
        box_height = max(1, frame.height // 3)
        return [
            Detection(
                label="person",
                confidence=0.87,
                x=max(0, (frame.width - box_width) // 2),
                y=max(0, (frame.height - box_height) // 3),
                width=box_width,
                height=box_height,
            )
        ]


def get_backend(name: str) -> InferenceBackend:
    normalized = name.strip().lower()
    if normalized == "mock":
        return MockBackend()
    raise ValueError(
        f"Unsupported backend '{name}'. Public builds include only the mock backend; "
        "hardware backends should live in private adapter packages."
    )
