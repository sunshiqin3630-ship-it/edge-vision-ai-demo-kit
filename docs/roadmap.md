# Roadmap

This roadmap tracks public maintenance work for Edge Vision AI Demo Kit. It is
intentionally hardware-neutral: private accelerator adapters, generated model
artifacts, deployment bundles, credentials, lab addresses, and board-specific
runtime details stay outside this repository.

## Current Focus

- Keep the mock pipeline deterministic and easy to run in CI.
- Keep the model manifest schema strict enough to record source, revision,
  license, input metadata, and artifact distribution status.
- Keep sanitization checks aligned with the repository policy.
- Improve contributor workflows with issue templates, review checklists, and
  release notes.

## Near-Term Maintenance

- Add more manifest examples for public detection and classification models.
- Document a minimal adapter contract test suite for private backends.
- Add a sample sanitized issue report and porting checklist.
- Expand CI with documentation link checks when the repository grows.

## Out Of Scope

- Publishing vendor SDK source, headers, libraries, tools, or examples.
- Publishing generated model weights, engine files, runtime archives, or target
  filesystem bundles.
- Publishing lab hosts, credentials, root login examples, internal project
  names, or personal image data.
- Adding backend code that requires private hardware access to run.

## Release Criteria

A public release should include:

- passing unit tests, manifest validation, and sanitization scan;
- updated documentation for any public interface changes;
- manifest entries with source repository, fixed revision, and license;
- no generated binaries, private runtime files, credentials, or board-specific
  deployment values.
