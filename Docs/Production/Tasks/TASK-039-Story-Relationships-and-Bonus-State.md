# TASK-039 Story Relationships and Bonus State

- Chinese companion: TASK-039-Story-Relationships-and-Bonus-State.zh-CN.md
- Document ID: TASK-039
- Version: 0.1
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: Core / Game relationship state
- Primary Agent: one designated writer at Ready; none started
- Branch/worktree: Local branch assigned at preflight; worktree only for actual isolation

## Goal

Implement the minimum user-authored relationship progression and bonus-source ledger, exposing validated effects to existing resource/card/combat contracts and read-only views to global preparation.

## Inputs and readiness

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, the predecessor contracts, [screen design](../../Design/EN/06_MapAndPreparation.md), [MVP decisions](../MVPDecisionRegister.md), ADR-0005, relevant module READMEs and nested instructions. English is canonical; the Chinese companion mirrors it.

- Required predecessors: [TASK-012](TASK-012-Run-State-and-Contracts.md), [TASK-015](TASK-015-Combat-Calculation-Contracts.md), [TASK-018](TASK-018-Calendar-and-Phase-Scheduler.md), [TASK-019](TASK-019-Resource-and-Global-Ledgers.md), [TASK-020](TASK-020-Card-Catalog-and-Lifecycle.md), [TASK-025](TASK-025-Node-Operations-and-Story.md).
- Decision gates: D03, D19, D27, D28. Pending answers are not defaults.
- The user requests this capability; the current contract only plans it. Complete dependencies, record exact approved behavior/content/values, review public schemas and promote to Ready before coding. All unaccepted numbers require individual confirmation.
- Core remains presentation-independent; stable identities and typed command/result/view boundaries apply. No runtime success is claimed.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/Core/Relationships/`
- `Source/DreamOfPadma/Private/MVP/Core/Relationships/`
- `Source/DreamOfPadma/Public/MVP/Game/Relationships/`
- `Source/DreamOfPadma/Private/MVP/Game/Relationships/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-039/`
- `Content/Padma/MVP/Definitions/Relationships/`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-039-Story-Relationships-and-Bonus-State.md`
- `Docs/Production/Tasks/TASK-039-Story-Relationships-and-Bonus-State.zh-CN.md`

The Coordinator separately owns shared index/state/task-matrix updates. Add any required normative/ADR/public-contract paths explicitly before Ready; do not infer a broad lease. Other features' code, configuration/build files, .agents/.codex, generated folders and third-party content are excluded.

## Out of scope

A complete relationship network, invented NPC/content/numerical profiles, automatic conversion to combat characters, UI implementation, direct GAS/Actor dependencies in Core and edits to other feature executors.

## Acceptance criteria

- [ ] D28 records the named story character, trigger event, bonus/target, threshold and every numerical field, grant/removal/stacking/timing and global-versus-run policy before implementation. No sample value closes this gate.
- [ ] Use independent StoryCharacterId, BonusDefinitionId/version, progress/unlock state and source-event identity; do not reuse synthesis compatibility, turn-based card identity or ACT character identity.
- [ ] Consume TASK-025 story outcome events through a validated application service. Replaying the same source event cannot grant progression or effects twice; failed changes leave prior state intact.
- [ ] A read-only eligibility/effect-source view identifies source, target/category, mode/scope, clock, grant condition and active/inactive reason. Approved effect consumers own execution; relationship state does not become another universal effect manager.
- [ ] Support the target approved in D28 through TASK-012's neutral contribution port; if B-card/category or synthesis is selected, use its owning card/synthesis modifier consumer. Do not require all relationship bonuses to use ACT GAS or inherit Arcana's non-stacking policy.
- [ ] Classify global and run-owned state explicitly. Run-owned participants support full battle snapshot/restore; separate global progression is not silently reverted by a local battle. Persist authoritative sources/versions and rebuild transient effects idempotently.
- [ ] Test an approved story → relationship → bonus → affected calculation path using provider fixtures; TASK-034 wires real consumers without giving this task ownership of their source.
- [ ] Bilingual module/task reports, scoped validation and independent read-only Review pass.

## Verification

Threshold/eligibility boundaries using approved values; repeated event idempotence; unsupported target rejection; source removal; global/run ownership; snapshot/load round trip; identical calculation after effect reconstruction.

Compile the Editor target for native changes; run the narrow TASK039 automation through Scripts/RunTests.ps1 and inspect actual case results. Run Scripts/AuditDocs.ps1, applicable ValidateProject.ps1 -Strict and git diff --check. Capture reproducible manual observations; no broad UI automation suite is required. TASK-034 performs real feature composition. Failed integration findings return to the feature owner.

## Ownership, learning and recovery

One Primary writer and one serial UE build/Editor lane; maps, assets, schemas and module README pairs are exclusive leases. A bounded architect/reviewer may inspect read-only; no second writer. Commands/events and state ownership must survive scene reconstruction and save boundaries without Actor pointers.

Learning target: Trace one story event to a persistent relationship source and a transient bonus; explain why reopening the UI cannot apply it again. User evidence: Not started. Provide one prediction and hands-on observation at implementation handoff; Agent output does not prove user learning.

Recover by reverting only this task's reviewed change after checking ownership. Do not delete generated/unrelated data or merge/push without explicit authorization.

## Completion report

- Status: Backlog; this is a planning amendment, implementation not started.
- Code/assets, runtime tests, user observations and independent implementation Review: pending.
- English/Chinese pair created together. Implementation handoff supplies the Changelog draft, exact changed paths, evidence and remaining risks.

## Explicit producer handoff

TASK-012 freezes neutral source/modifier values and the consumer registry before this implementation. This task produces the relationship source/eligibility provider; 019/020/022/015 own selected numerical application, and 034 injects the provider. This task never retrofits those executors or applies the same scalar a second time.
