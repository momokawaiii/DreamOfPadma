# TASK-038 Global Preparation and Bonus UI

- Chinese companion: TASK-038-Global-Preparation-and-Bonus-UI.zh-CN.md
- Document ID: TASK-038
- Version: 0.1
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: UI / Input
- Primary Agent: one designated writer at Ready; none started
- Branch/worktree: Local branch assigned at preflight; worktree only for actual isolation

## Goal

Provide one player-facing preparation entry that composes mode loadouts, shared basic skills, relationship/growth records, bonus sources and card discovery from authoritative providers.

## Inputs and readiness

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, the predecessor contracts, [screen design](../../Design/EN/06_MapAndPreparation.md), [MVP decisions](../MVPDecisionRegister.md), ADR-0005, relevant module READMEs and nested instructions. English is canonical; the Chinese companion mirrors it.

- Required predecessors: [TASK-012](TASK-012-Run-State-and-Contracts.md), [TASK-019](TASK-019-Resource-and-Global-Ledgers.md), [TASK-020](TASK-020-Card-Catalog-and-Lifecycle.md), [TASK-035](TASK-035-ACT-Character-Cards-and-Roster.md), [TASK-039](TASK-039-Story-Relationships-and-Bonus-State.md).
- Decision gates: D20, D23, D26, D27, D28. Pending answers are not defaults.
- The user requests this capability; the current contract only plans it. Complete dependencies, record exact approved behavior/content/values, review public schemas and promote to Ready before coding. All unaccepted numbers require individual confirmation.
- Core remains presentation-independent; stable identities and typed command/result/view boundaries apply. No runtime success is claimed.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/UI/Preparation/`
- `Source/DreamOfPadma/Private/MVP/UI/Preparation/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-038/`
- `Content/Padma/MVP/UI/Preparation/`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.md`
- `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.zh-CN.md`

The Coordinator separately owns shared index/state/task-matrix updates. Add any required normative/ADR/public-contract paths explicitly before Ready; do not infer a broad lease. Other features' code, configuration/build files, .agents/.codex, generated folders and third-party content are excluded.

## Out of scope

Direct world/Actor mutation, another equipment/bonus/codex authority, numerical tuning, NPC progression rules, FPS execution/authoring, ACT bullet-time changes and a developer data-table editor.

## Acceptance criteria

- [ ] A consistent Global Preparation button can be hosted on the map and approved local screens; closing restores the previous screen/selection through a typed navigation boundary.
- [ ] ACT selects characters/weapons through TASK-035 and reads TASK-036 definitions through that provider. It does not create a second roster or directly edit deployed turn-based cards.
- [ ] FPS has its own capability/definition boundary and the D26-approved availability state. Never cast ACT tables to FPS or label an unimplemented mode playable; playable FPS requires its own contract.
- [ ] Shared basic-skill identity/slots expose Encounter and ACT effect views without merging their costs, clocks or execution. This page is distinct from in-battle ACT Tab/bullet time.
- [ ] Relationship/growth and bonus views show source, target/card category, mode/scope, condition, duration/clock and current eligibility supplied by authoritative services. Empty/unavailable providers stay explicit.
- [ ] Apply D27's open/edit/close/time policy; browsing never equips, grants a buff or submits a calendar advance. Validated edits return results/errors; global presets and current-run settings retain separate ownership.
- [ ] Codex respects the card provider's approved discovery rules, including the documented first-success C-card information boundary; no hidden definition leaks through another tab.
- [ ] TASK-034 binds the actual navigation hosts/providers; this task tests them with explicit valid/unavailable fixtures without editing other owners' source.
- [ ] Bilingual module/task reports, scoped validation and independent read-only Review pass.

## Verification

Read-only open/close and prior-screen restore; rejected/stale roster edits; trait restriction reason; missing mode/provider; per-mode skill views; discovery filtering; no duplicate grants on refresh. Observe both map and local navigation in final integration.

Compile the Editor target for native changes; run the narrow TASK038 automation through Scripts/RunTests.ps1 and inspect actual case results. Run Scripts/AuditDocs.ps1, applicable ValidateProject.ps1 -Strict and git diff --check. Capture reproducible manual observations; no broad UI automation suite is required. TASK-034 performs real feature composition. Failed integration findings return to the feature owner.

## Ownership, learning and recovery

One Primary writer and one serial UE build/Editor lane; maps, assets, schemas and module README pairs are exclusive leases. A bounded architect/reviewer may inspect read-only; no second writer. Commands/events and state ownership must survive scene reconstruction and save boundaries without Actor pointers.

Learning target: Trace a settings button → command → authoritative result → refreshed view, then identify which data is a preset and which belongs to the run. User evidence: Not started. Provide one prediction and hands-on observation at implementation handoff; Agent output does not prove user learning.

Recover by reverting only this task's reviewed change after checking ownership. Do not delete generated/unrelated data or merge/push without explicit authorization.

## Completion report

- Status: Backlog; this is a planning amendment, implementation not started.
- Code/assets, runtime tests, user observations and independent implementation Review: pending.
- English/Chinese pair created together. Implementation handoff supplies the Changelog draft, exact changed paths, evidence and remaining risks.

## 2026-09-09 confirmed model/movement handoff

D26 is answered: ACT is playable in the first MVP; FPS is a future capability page without deployment/skill execution. Model/table references are authored in UE Data Assets, not this player-facing UI. Global configuration edits/time/persistence and concrete relationship bonuses remain D27/D28.
