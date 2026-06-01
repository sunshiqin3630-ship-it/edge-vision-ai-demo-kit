# Porting A Private Accelerator Backend

Keep accelerator-specific code in a private adapter package. This public
repository should remain useful without access to the hardware.

## Adapter Contract

An adapter should match the public backend behavior:

```python
class PrivateBackend:
    name = "private"

    def infer(self, model_id, frame):
        return [
            Detection(
                label="person",
                confidence=0.87,
                x=100,
                y=80,
                width=320,
                height=480,
            )
        ]
```

## Required Local Configuration

Use environment variables for every private input:

- `EDGE_VISION_ACCELERATOR_SDK_ROOT`
- `EDGE_VISION_BACKEND`
- `EDGE_VISION_BOARD_HOST`
- `EDGE_VISION_BOARD_USER`
- model artifact paths

Do not hard-code lab hosts, passwords, SDK roots, or deployment directories.

## Recommended Bring-up Steps

1. Run the public mock pipeline.
2. Validate the model manifest entry and license.
3. Convert or build model artifacts outside Git.
4. Run a one-frame hardware smoke test in the private adapter repository.
5. Connect the adapter to the UI or display layer.
6. Record latency, model id, frame size, and detection counts without copying
   private logs or binaries into this repository.

## Publication Checklist

- The public repo builds without the private SDK.
- Generated engines, weights, libraries, and archives are ignored by Git.
- Documentation uses placeholders for board hosts and paths.
- Sanitization scanner passes.
- Hardware claims clearly separate static validation, build success, and
  board-side runtime evidence.
