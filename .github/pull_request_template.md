## Summary

- 

## Checks

- [ ] Tests pass with `PYTHONDONTWRITEBYTECODE=1 PYTHONPATH=src python3 -m unittest discover -s tests -v`
- [ ] Sanitization passes with `PYTHONDONTWRITEBYTECODE=1 PYTHONPATH=src python3 scripts/check_sanitization.py`
- [ ] No generated model artifacts, runtime libraries, archives, credentials, or board-specific paths were added
- [ ] Model manifest entries include source repository, fixed revision, and license
