# Sanitization Policy

This repository is designed to be safe to publish as open source.

## Forbidden Content

Do not commit:

- private accelerator SDKs, sample code, generated headers, or tools;
- generated model artifacts, weights, engine blobs, archives, or runtime
  libraries;
- board root filesystems, deployment bundles, sysroots, or copied target
  runtime directories;
- lab IP addresses, credentials, passwords, SSH shortcuts, or personal images;
- proprietary benchmark logs, customer data, or internal project names.

## Allowed Content

You may commit:

- source code for public interfaces and mock backends;
- documentation that describes the generic architecture;
- manifest entries with public model source URLs, fixed revisions, and licenses;
- scripts that validate repository hygiene;
- templates that use placeholders instead of real deployment values.

## Required Checks

Before publishing or opening a pull request:

```bash
PYTHONPATH=src python3 -m unittest discover -s tests -v
PYTHONPATH=src python3 scripts/check_sanitization.py
```

Both commands must pass.
