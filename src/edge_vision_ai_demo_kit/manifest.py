from __future__ import annotations

from dataclasses import dataclass
import json
from pathlib import Path
from typing import Any


@dataclass(frozen=True)
class ModelSource:
    repo: str
    revision: str
    license: str


@dataclass(frozen=True)
class ModelInputs:
    width: int
    height: int
    format: str


@dataclass(frozen=True)
class ModelArtifact:
    type: str
    path: str
    distributed: bool


@dataclass(frozen=True)
class ModelEntry:
    id: str
    task: str
    source: ModelSource
    inputs: ModelInputs
    artifacts: list[ModelArtifact]


@dataclass(frozen=True)
class Manifest:
    models: list[ModelEntry]


def _require_mapping(value: Any, field: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise ValueError(f"{field} must be an object")
    return value


def _require_string(value: Any, field: str) -> str:
    if not isinstance(value, str) or not value.strip():
        raise ValueError(f"{field} must be a non-empty string")
    return value


def _require_int(value: Any, field: str) -> int:
    if not isinstance(value, int) or value <= 0:
        raise ValueError(f"{field} must be a positive integer")
    return value


def _parse_model(raw: Any, index: int) -> ModelEntry:
    model = _require_mapping(raw, f"models[{index}]")
    source = _require_mapping(model.get("source"), f"models[{index}].source")
    inputs = _require_mapping(model.get("inputs"), f"models[{index}].inputs")
    artifacts_raw = model.get("artifacts", [])
    if not isinstance(artifacts_raw, list):
        raise ValueError(f"models[{index}].artifacts must be a list")

    artifacts = []
    for artifact_index, artifact_raw in enumerate(artifacts_raw):
        artifact = _require_mapping(
            artifact_raw, f"models[{index}].artifacts[{artifact_index}]"
        )
        distributed = artifact.get("distributed", False)
        if not isinstance(distributed, bool):
            raise ValueError(
                f"models[{index}].artifacts[{artifact_index}].distributed must be a boolean"
            )
        artifacts.append(
            ModelArtifact(
                type=_require_string(
                    artifact.get("type"),
                    f"models[{index}].artifacts[{artifact_index}].type",
                ),
                path=_require_string(
                    artifact.get("path"),
                    f"models[{index}].artifacts[{artifact_index}].path",
                ),
                distributed=distributed,
            )
        )

    return ModelEntry(
        id=_require_string(model.get("id"), f"models[{index}].id"),
        task=_require_string(model.get("task"), f"models[{index}].task"),
        source=ModelSource(
            repo=_require_string(source.get("repo"), f"models[{index}].source.repo"),
            revision=_require_string(
                source.get("revision"), f"models[{index}].source.revision"
            ),
            license=_require_string(
                source.get("license"), f"models[{index}].source.license"
            ),
        ),
        inputs=ModelInputs(
            width=_require_int(inputs.get("width"), f"models[{index}].inputs.width"),
            height=_require_int(inputs.get("height"), f"models[{index}].inputs.height"),
            format=_require_string(inputs.get("format"), f"models[{index}].inputs.format"),
        ),
        artifacts=artifacts,
    )


def load_manifest(path: str | Path) -> Manifest:
    manifest_path = Path(path)
    raw = json.loads(manifest_path.read_text(encoding="utf-8"))
    data = _require_mapping(raw, "manifest")
    models_raw = data.get("models")
    if not isinstance(models_raw, list) or not models_raw:
        raise ValueError("models must be a non-empty list")
    return Manifest(models=[_parse_model(model, idx) for idx, model in enumerate(models_raw)])
