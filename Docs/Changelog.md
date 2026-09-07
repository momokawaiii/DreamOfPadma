# Changelog

- Chinese companion for user reading: `Docs/Changelog.zh-CN.md`
- Document ID: `CHANGELOG`
- Version: `Unreleased`
- Status: `Active`

This document is the concise, user-facing release summary for delivered implementation work. Detailed scope, acceptance evidence, design decisions, and unresolved questions remain in the relevant TASK, ADR, module documentation, and `Docs/ProjectState.md`.

## Unreleased

### Added

- 2026-09-07 — **Bilingual implementation change-log workflow**
  - Goal: require every implementation Goal that changes code, data, UE assets, configuration, build files, or tests to leave a concise release-style summary.
  - Workflow: the Primary Agent supplies the draft; the Integration Coordinator writes the synchronized English and Chinese entries before `Verified` or `Done`.
  - Verification: `Scripts/AuditDocs.ps1`, `Scripts/ValidateProject.ps1 -Strict`, and `git diff --check`.
  - Runtime impact: none; no gameplay rule was changed.
