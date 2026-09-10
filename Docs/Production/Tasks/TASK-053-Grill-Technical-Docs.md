# TASK-053 Grill-Based Technical Documentation

- Document ID: TASK-053
- Version: 1.0
- Chinese companion: [TASK-053-Grill-Technical-Docs.zh-CN.md](TASK-053-Grill-Technical-Docs.zh-CN.md)
- Status: Verified
- Primary / Local checkout: Codex Primary; E:/2026ue/DreamOfPadma, codex/mvp-demo-foundations
- User authorization: 2026-09-10 request to refine and simplify existing technical docs using the linked grill conclusions

## Outcome and exact scope

Consolidate technical ownership and distinguish current code from the accepted offline Demo target and deferred/open designs. Apply padma-task-runner. Structural target contracts are recorded in ADR-0010; no source, config, UE asset, save format or generated folder changes. No Git integration or remote actions.

English paths below and each same-directory .zh-CN.md companion are the complete document write set; evidence only in Artifacts/TASK-053. Preexisting working changes are preserved.

```text
Docs/Architecture/ProgramArchitecture.md
Docs/Architecture/DataDrivenArchitecture.md
Docs/Architecture/ModuleMap.md
Docs/Architecture/RuntimeFlow.md
Docs/Architecture/SaveSchema.md
Docs/Architecture/Modules/PadmaCore/README.md
Docs/Architecture/Modules/PadmaGame/README.md
Docs/Architecture/Modules/PadmaWorld/README.md
Docs/Architecture/Modules/PadmaEditor/README.md
Docs/Content/WorldMapAuthoring.md
Docs/Production/Roadmap.md
Docs/Production/BuildMatrix.md
Docs/Production/MVPDecisionRegister.md
Docs/ProjectState.md
Docs/00_INDEX.md
Docs/Changelog.md
Docs/Rules/ChapterZero.md
Docs/Decisions/ADR-0010-Offline-Demo-Content-and-Map.md
Docs/Production/Tasks/TASK-053-Grill-Technical-Docs.md
```

## Source and decision reconciliation

Source: [Skills Testing](codex://threads/01a06a4e-a758-72a2-abf6-985bd08d6ae1), user answers and question definitions on 2026-09-10. The app reader returned empty message items; the matching local session transcript supplied the exact questions/answers and final summary. Earlier assistant recommendations are not independent authorization.

| Evidence | Adopted meaning / document owner |
|---|---|
| Q1, Q4–Q8 | Installable offline release-quality Demo, both build configurations, no Steam integration; ChapterZero / Roadmap / BuildMatrix |
| Q11, Q14, Q15 | Real golden path, distinct Encounter then one ACT, court ending, final production values; ChapterZero |
| Q12, Q16 → Q21 | Fixed anchors/edges/Seed; later editor-bake decision overrides earlier runtime mixture; WorldMapAuthoring |
| Q17–Q24, Q28–Q29 | Chapter/data separation, orthogonal classification, MapKey/immutable manifest, independent story version; DataDrivenArchitecture / SaveSchema |
| Q25, Q30 → Q33 | Stage-first-map-load story assignment becomes saved state; reload does not redraw; ChapterZero / RuntimeFlow |
| Q26, Q31 | Distinct status and TutorialRunId; skip continues, deliberate replay rewards again, retry does not; RuntimeFlow / SaveSchema |
| Q34, Q36–Q38 | Once-locked checkpoints, fixed registry, dialogue table/text table, presentation-only cues; data/runtime documents |
| Q19, Q23, Q27–Q28, Q32, Q35; 08:52:50 UTC deferral | Restricted Slate/UEdGraph source-to-runtime design retained but editor/compile/authoring UI postponed; PadmaEditor |
| Q39–Q45 | No answers. Preserve as proposals, including direct special-node dispatch choice, parameter encoding, tutorial events and exhaustive compiler gates |
| Current code/ADRs | Single runtime module, synchronous catalog, current frozen layout/save envelope, separate modes and activatable UI remain current; no migration claims |

Verified current anchors in Core/Content, Core/Run, Core/WorldMap, Game/Content, Game/Save, Build.cs and ADR-0004/0007/0008/0009. No external engine specification or Civ6 algorithm correctness is asserted.

## Acceptance

- [x] Every accepted grill decision has an owner; later answers override earlier options.
- [x] Remove duplicated central rule/module/backlog prose; retain links to normative and historical sources.
- [x] Document precise map/data/state lifetimes, story assignment and tutorial reward/restore requirements without claiming implementation.
- [x] Keep Q39–Q45 and unprovided final values open; story editor is deferred.
- [x] English/Chinese parity, relative links and scoped whitespace checks.
- [x] AuditDocs and applicable repository validation.
- [x] Focused independent read-only review of public/save/determinism target contracts, as required by Workflow.

## Evidence and remaining work

Task-start copies are Artifacts/TASK-053/before/*.txt; exact paths and initial Git state are allowed-paths.json and status-before.txt. This baseline includes earlier uncommitted edits, unlike a HEAD-only diff.

Validation completed:

- AuditDocs passed: 290 Markdown files / 145 language pairs; audit-docs.log.
- ValidateProject -Strict passed; validate-project.log.
- All 38 changed documents passed local link, paired ID/version, fence and task-start-diff whitespace checks; check-docs.cjs and doc-checks.json.
- Independent read-only reviewer docs_053_review: Pass, no P0–P3 findings; review.json records its scope and limits.
- Five central architecture pairs reduced from 1,644 to 636 lines (61%); task-start/current totals for the complete write set are in doc-checks.json.
- Original question/answer provenance is retained in grill-evidence.json. Git status changes outside this document write set were not observed; preexisting edits were retained.

Runtime build/PIE/packaging are not rerun because this delivery changes only documentation. The actual Demo still needs implementation/content/user acceptance in Roadmap; documentation completion does not establish those results.

Commit/merge/push: not performed or authorized.
