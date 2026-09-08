# Changelog

- Chinese companion for user reading: `Docs/Changelog.zh-CN.md`
- Document ID: `CHANGELOG`
- Version: `Unreleased`
- Status: `Active`

This document is the concise, user-facing release summary for delivered implementation work. Detailed scope, acceptance evidence, design decisions, and unresolved questions remain in the relevant TASK, ADR, module documentation, and `Docs/ProjectState.md`.

## Unreleased

### Added

- 2026-09-08 — **TASK-007 SLG Demo World Selection and Transition**
  - User-visible result: added a project-owned Demo Sandbox with one selectable tile and a typed node/scenario/spawn transition context retained within the same GameInstance/session across scene changes.
  - Verification: follow-up UE5.8 Editor compile, TASK-007 transition-context automation, unattended DemoSandbox map-load smoke, user-completed post-fix PIE selection/highlight/deselect/confirm/invalid-fixture checks, and independent Review `Pass`.
  - Integration: locally closed after completion-report commit `9ea99c7`; no remote push or release tag.
  - Deferred: default-map integration, Encounter scene, a dedicated lifecycle regression test, and all deferred production world rules remain outside this delivery.

- 2026-09-07 — **Bilingual implementation change-log workflow**
  - Goal: require every implementation Goal that changes code, data, UE assets, configuration, build files, or tests to leave a concise release-style summary.
  - Workflow: the Primary Agent supplies the draft; the Integration Coordinator writes the synchronized English and Chinese entries before `Verified` or `Done`.
  - Verification: `Scripts/AuditDocs.ps1`, `Scripts/ValidateProject.ps1 -Strict`, and `git diff --check`.
  - Runtime impact: none; no gameplay rule was changed.
