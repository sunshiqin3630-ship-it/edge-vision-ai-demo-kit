from __future__ import annotations

from dataclasses import dataclass

from .backends import Detection, FrameInfo, get_backend


@dataclass(frozen=True)
class PipelineConfig:
    backend: str
    source: str
    model_id: str
    frame_width: int
    frame_height: int


@dataclass(frozen=True)
class PipelineResult:
    backend: str
    source: str
    model_id: str
    detections: list[Detection]


def run_once(config: PipelineConfig) -> PipelineResult:
    if config.frame_width <= 0 or config.frame_height <= 0:
        raise ValueError("frame dimensions must be positive")
    backend = get_backend(config.backend)
    frame = FrameInfo(
        source=config.source,
        width=config.frame_width,
        height=config.frame_height,
    )
    detections = backend.infer(config.model_id, frame)
    return PipelineResult(
        backend=backend.name,
        source=config.source,
        model_id=config.model_id,
        detections=detections,
    )
