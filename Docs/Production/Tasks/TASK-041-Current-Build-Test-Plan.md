# TASK-041 Current Build Test Plan

- Chinese companion: TASK-041-Current-Build-Test-Plan.zh-CN.md
- Document ID: TASK-041
- Version: 0.1
- Status: Verified
- Primary Agent / Role: root, sole documentation writer
- Branch: Local codex/mvp-demo-foundations; no new worktree
- Authorization: the user requested a test plan for current UE and HTML functionality on 2026-09-09.

## Goal and scope

Deliver a reproducible bilingual test plan for the current UE candidate and the existing HTML sketch. Identify launch points, preparation/reset, case IDs, steps, expected results, evidence and severity. Distinguish existing behavior, known rule drift, unauthored fixtures and future features. This authorizes documentation only, not gameplay changes or a new execution verdict.

Read AGENTS.md, Docs/00_INDEX.md, ProjectState, TASK-007/008/036/040, ModelAuthoring, ACTAuthoring, the relevant implementation/tests, and the two local HTML files. The latest user decisions remain authoritative.

## Allowed paths

- `Docs/Production/CurrentBuildTestPlan.md`
- `Docs/Production/CurrentBuildTestPlan.zh-CN.md`
- `Docs/Production/Tasks/TASK-041-Current-Build-Test-Plan.md`
- `Docs/Production/Tasks/TASK-041-Current-Build-Test-Plan.zh-CN.md`
- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`

All current code, maps, assets, scripts, HTML, configuration and unrelated working-tree changes are read-only. There is no overlapping writer and no delegation. No UE/browser runtime execution, build, test fixture creation, commit, merge or push is part of this plan delivery.

## Acceptance and verification

- [x] Current UE/HTML entrances and actual controls are grounded in code/assets.
- [x] Manual smoke, negative/recovery, model/ACT authoring and existing automation are covered.
- [x] Known HTML drift and future gameplay are explicit; prior evidence is not presented as a fresh test run.
- [x] Each case has an ID, starting condition, action, expected result and evidence convention.
- [x] Both language files, index links, task status and scoped Markdown checks agree.

Use AuditDocs.ps1, ValidateProject.ps1 -Strict, scoped diff/whitespace/link checks, case-ID parity and read-only source cross-checks. The test plan includes optional hands-on observation; no user learning or gameplay acceptance is claimed.

## Completion report

- Status: Verified for test-plan delivery only; execution under the plan remains pending.
- Changed paths: the six allowed Markdown paths above, including the synchronized plan and task pairs plus both indexes. The plan is linked at [CurrentBuildTestPlan](../CurrentBuildTestPlan.md).
- Coverage: 16 UE manual cases, 13 HTML cases, six existing project automation names, four known gaps, launch/reset instructions, optional isolated fixtures, and result/severity records.
- Source cross-check: exact UE controls, fixture restore selection behavior, current model/skill ownership, six test registrations, UE5.8.2 Build.version and both HTML SHA-256 fingerprints. The standalone wrapper contains the exact source fragment.
- Validation: AuditDocs passed (202 Markdown files / 101 pairs), ValidateProject -Strict passed, scoped git diff --check passed; custom case-ID/order, automation-name, whitespace and local-link checks passed; both PowerShell examples parse successfully.
- Planning adjustment: creating Docs/Testing was denied by filesystem access, so the plan uses the existing Docs/Production directory. No new permission or configuration was required.
- Review boundary: root performed documentation/source checks; no independent gameplay acceptance or new runtime test verdict is claimed.
- Runtime test execution and user evidence: not performed in this documentation task. The earlier six-test Pass remains historical evidence.
- No gameplay, asset, HTML, script or configuration edits; no commit, merge or push.
