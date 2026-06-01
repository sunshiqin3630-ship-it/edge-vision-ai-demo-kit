# Security Policy

## Reporting A Vulnerability

Open a private security advisory or contact the maintainers directly if you
find a vulnerability that could expose credentials, private model artifacts,
deployment paths, or proprietary SDK details.

## Sensitive Data Rules

This repository must not contain:

- credentials or tokens;
- private board addresses;
- root login shortcuts;
- generated model files;
- runtime libraries or board deployment bundles;
- proprietary SDK names, paths, or code.

Run the sanitization scanner before publishing changes:

```bash
PYTHONPATH=src python3 scripts/check_sanitization.py
```
