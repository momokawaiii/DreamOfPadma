# TASK-004 ACT Architecture Decision Integration

- Chinese companion for user reading: `TASK-004-ACT-Architecture-Decision.zh-CN.md`
- Document ID: `TASK-004`
- Version: `0.1`
- Status: `Done`
- Parent milestone or integration Goal: `M1 Gameplay architecture / future ACT boundary`
- Primary Role: `Integration Coordinator`
- Primary Agent: current Integration Coordinator
- Branch/worktree: `main`, integration-owned serial documentation change; no independent worktree
- Task mode: architecture-decision integration and review only; no code, asset, plugin, or dependency implementation

## Goal

Turn the existing ACT architecture and ADR drafts into one traceable, bilingual, reviewed architecture decision: bind the Future ACT Development Agent Contract and ADR-0002 to this task, align the PadmaGameplay module document, documentation index, and project state, preserve every Open/Deferred gameplay decision, obtain an independent read-only review, and commit only this task's documentation scope.

The observable result is an integrated documentation baseline that makes Encounter-first delivery, future ACT ownership, C++/data-driven authority, and GAS/plugin gates explicit without authorizing ACT implementation.

## Why now

The ACT contract and ADR already exist in the working tree and are linked from the module documentation, but they are not yet owned by a TASK contract, recorded as completed integration work in `ProjectState.md`, independently reviewed, or committed. Leaving accepted architecture only in an untracked working tree would make future Agent startup and decision provenance ambiguous.

## Read first

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Rules/Combat.md`
- `Docs/Design/EN/03_CombatAIPlanner.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md`
- `Docs/Decisions/ADR-0002-Encounter-First-ACT-Preparation.md`

## Decision state

### Frozen inputs

- Encounter is the current gameplay implementation priority; ACT remains a later RealTimeAction route.
- The accepted MVP behavior for the ACT card repository remains `Tab`, background blur, 1/10 world time, rejected movement/attack input, preserved scene inertia, and five visible card slots per page.
- Future ACT architecture is C++-first and data-driven. Authoritative gameplay does not move into UI, animation notifies, Gameplay Cues, INI, or data-only Blueprints.
- Unreal's built-in GAS is compatible with the future direction but is not enabled by this task. An external gameplay plugin requires a separate ADR.
- The external Combat prototype is reference evidence only and is not a source of Padma rules, dependencies, assets, or default values.
- This architecture decision is documentation authority only. A future ACT implementation requires its own approved `Ready` TASK and satisfied Encounter prerequisites.

### Deferred or open inputs

- The first ACT vertical slice, exact ability, Ability System Component ownership, grant/remove policy, and networking baseline.
- Final Gameplay Tag taxonomy, Attribute Sets, damage/effect magnitude ownership, stacking, phase names, cancel matrix, interruption priority, and input-buffer semantics.
- Exact `CharacterACTCatalog`, DataTable row, `AbilityDefinition`, and `ACTSequence` schemas; import/version policy; `Scope` meaning; and payload lifecycles.
- ACT duration/toggle behavior, resource cost/cooldown, page boundaries, total page count, card-to-ability legality, Motion Warping details, and all design-baseline items already marked Open or Deferred.
- These decisions remain with the named design, system, numerical, architecture, or user owner. This task must not invent them.

## Scope

### Allowed paths and write set

- `Docs/Production/Tasks/TASK-004-ACT-Architecture-Decision.md`
- `Docs/Production/Tasks/TASK-004-ACT-Architecture-Decision.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md`
- `Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.zh-CN.md`
- `Docs/Decisions/ADR-0002-Encounter-First-ACT-Preparation.md`
- `Docs/Decisions/ADR-0002-Encounter-First-ACT-Preparation.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.zh-CN.md`
- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`
- `Docs/ProjectState.md`
- `Docs/ProjectState.zh-CN.md`

### Out of scope

- Any C++, Blueprint, map, `.uasset`, Config, Build, plugin, engine, generated, test implementation, GAS dependency, or ACT runtime implementation.
- Changing accepted gameplay meaning or resolving any Open, Proposed, To confirm, playtest-only, or Deferred item.
- Implementing Encounter, ACT, card lifecycle, combat formulas, abilities, animation, input, UI, networking, save, import, or content assets.
- Changing the status or semantics of TASK-001, TASK-002, or TASK-003.
- Committing unrelated working-tree changes, merging, tagging, pushing, or creating a release.

## Dependencies and integration order

- Required predecessor: the accepted MVP design and architecture baseline in commit `cbb4da4`; TASK-002 is `Done`.
- Direct authorization: the user's 2026-09-05 request to place the ACT/ADR changes under a formal bilingual architecture-decision TASK, update project state, complete Review, and commit.
- Consumers: future Encounter planning, future ACT TASKs, PadmaGameplay, PadmaGame, PadmaUI, PadmaEditor, PadmaTests, and architecture reviewers.
- Integration order: this documentation decision may be integrated before TASK-003 closes, but it does not bypass TASK-003 or authorize an implementation Agent. TASK-003 remains the serial Core-contract gate for downstream Core implementation TASKs.
- Shared conflicts: the index and ProjectState contain pre-existing unrelated working-tree changes. Preserve them and stage only TASK-004-owned hunks. All shared-state writing remains serial under the Integration Coordinator.

## Delegation plan

| Role | Work package | Authority | Expected output |
|---|---|---|---|
| `reviewer` | Review the actual TASK-004 diff against this contract, canonical architecture/rules, Open/Deferred preservation, bilingual consistency, and staged-path isolation. | Read-only | P0-P3 findings, checks performed, residual risks, and `Block`, `Conditional pass`, or `Pass`. |

## Acceptance criteria

- [x] This English/Chinese TASK pair has matching identity, version, status, scope, acceptance, and completion meaning.
- [x] ADR-0002 names its architecture owner and TASK-004, and the ACT development contract names TASK-004.
- [x] The ACT contract, ADR, and PadmaGameplay README consistently require Encounter-first delivery and a separately approved ACT implementation TASK.
- [x] C++/data-driven authority, built-in GAS gating, external-plugin ADR requirements, and presentation/non-authority boundaries remain consistent with the architecture baseline.
- [x] Every unresolved gameplay, schema, dependency, network, save, input, and content decision remains explicitly Open or Deferred.
- [x] The bilingual index links TASK-004, and bilingual ProjectState records the accepted documentation-only decision and the correct next gate through TASK-003.
- [x] No TASK-001 or TASK-003 file, source, asset, config, build, plugin, generated, or implementation file is included in the TASK-004 commit.
- [x] An independent read-only Review returns `Pass`, or all blocking findings are resolved and re-reviewed.
- [x] Documentation audit, strict project validation, whitespace checks, and staged-scope checks pass.

## Verification plan

### Automated checks

- `pwsh -NoProfile -File .\Scripts\AuditDocs.ps1`
- `pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict`
- `git diff --check` and `git diff --cached --check`
- Compare `git diff --cached --name-only` with the twelve allowed paths.
- Confirm no source, asset, config, build, plugin, generated, TASK-001, or TASK-003 file is staged.

### Manual checks

- Trace the ACT/Encounter claims to `Combat.md`, the accepted combat design baseline, and the architecture documents.
- Compare every changed English Markdown file with its Chinese companion for matching meaning.
- Confirm that `Accepted` describes architecture documentation only and cannot be read as ACT/GAS implementation permission.
- Inspect the staged diff independently from the worker narrative.

### Required evidence

- Audit and strict-validation output.
- Exact staged-path list and staged diff summary.
- Read-only reviewer findings and final acceptance status.
- Commit hash for the scoped documentation integration.

## Learning contract

| Target | Evidence required | Status |
|---|---|---|
| ADR authority versus TASK execution authority | The user explains why an accepted ADR does not authorize implementation without a separate `Ready` TASK. | `Not started` |
| Serial shared-contract integration | The user identifies why shared index/state/architecture changes stay with the Integration Coordinator before downstream Primary Agents begin. | `Not started` |

- Learner's hands-on exercise: identify one future ACT decision that belongs in an ADR and one bounded implementation result that belongs in a TASK.
- Transfer question: after TASK-003 closes, what additional contract must exist before `module_worker` may edit Core implementation files?

## Risks and recovery

- Mixed working-tree changes could be committed accidentally. Recovery: use path- and hunk-scoped staging, inspect the staged diff, and leave unrelated changes unstaged.
- `Accepted` could be mistaken for implementation permission. Recovery: retain the documentation-only status, task gate, and ACT Agent stop conditions in both languages.
- Future work could silently copy the prototype or enable GAS/plugins. Recovery: stop the implementation and require the relevant `Ready` TASK, dependency review, and ADR.
- Safe rollback: revert the TASK-004 integration commit; do not erase unrelated working-tree changes.

## Completion report

- Final status: `Done`.
- Primary Agent and Role: current Integration Coordinator / Integration Coordinator.
- Changed files: the twelve allowed Markdown paths listed above.
- Acceptance evidence: the staged set matched all twelve allowed paths; all newly linked TASK, ACT-contract, and ADR files existed; the independent re-review returned `Pass` with no findings.
- Checks run and results: `AuditDocs.ps1` passed for 108 Markdown files and 54 language pairs; `ValidateProject.ps1 -Strict`, `git diff --check`, `git diff --cached --check`, link-target existence, and staged-scope assertions passed.
- Checks not run and reason: gameplay, compile, PIE, cook, and packaging checks are outside this documentation-only task.
- Review findings resolved or accepted: the first completed review blocked a bilingual ProjectState branch-policy rewrite as unrelated; that hunk was removed, and the same independent reviewer returned `Pass` on the updated staged diff.
- Remaining risks and open questions: all items under **Deferred or open inputs** remain unresolved by design.
- English/Chinese documentation updated: yes.
- Agent-produced learning evidence: the task distinguishes ADR authority from implementation authorization and records the serial integration boundary.
- User-produced learning evidence: not yet recorded.
- Integration commit or handoff reference: this scoped TASK-004 documentation commit; the resulting hash is reported in the Integration Coordinator handoff.
