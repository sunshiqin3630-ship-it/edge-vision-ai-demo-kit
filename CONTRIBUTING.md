# Contributing

Thanks for contributing to Edge Vision AI Demo Kit.

## Ground Rules

- Keep the public repository hardware-neutral.
- Do not commit private SDK code, generated model artifacts, runtime libraries,
  board archives, credentials, or lab addresses.
- Record model source, revision, and license in `models/manifest.example.json`
  or a new manifest file.
- Add or update tests for behavior changes.

## Development Checks

```bash
PYTHONPATH=src python3 -m unittest discover -s tests -v
PYTHONPATH=src python3 scripts/check_sanitization.py
```

Both checks must pass before sending a pull request.

## Pull Request Checklist

- The change builds without private hardware or SDK access.
- The sanitization scan passes.
- Public documentation uses placeholders for paths and hosts.
- Any model entry includes source repository, fixed revision, and license.
- Generated artifacts are excluded from Git.
