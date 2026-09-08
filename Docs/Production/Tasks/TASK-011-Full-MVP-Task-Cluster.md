# TASK-011 Full MVP Task Cluster and Separate GAS Modes

- Chinese companion: TASK-011-Full-MVP-Task-Cluster.zh-CN.md
- Document ID: TASK-011
- Version: 0.1
- Status: In Progress
- Primary Role / Agent: Integration Coordinator / current primary documentation agent
- Parent milestone: complete playable MVP
- Workspace: current Local checkout; documentation only; no independent worktree
- Authorization: user requested this task cluster on 2026-09-08 and confirmed that Encounter and ACT both use GAS with different mode systems and configuration tables.

## Goal

Deliver the bilingual TASK-012 through TASK-034 contracts, ADR-0004, a decision register, and synchronized production/architecture entry points for the complete seven-day MVP. This is the first of the 24 newly proposed tasks. Runtime implementation is delivered by the child tasks.

The accepted MVP contains one seven-day Era in the seven-Era Chapter structure, approximately eight shared sandbox nodes, two A/two B/two C cards plus basic battle cards and a non-reward failure definition, resource operations, synthesis, world changes and ruler responses, complete Encounter and ACT battle routes, safe-boundary persistence, and the two confirmed campaign victories.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, Docs/Agent/Workflow.md, Docs/Agent/CodexSetup.md, Docs/Production/TaskTemplate.md, all four Docs/Rules files, the five accepted English design baselines, ProgramArchitecture.md, DataDrivenArchitecture.md, ModuleMap.md, RuntimeFlow.md, SaveSchema.md, PadmaGameplay README/ACTDevelopmentContract, and TASK-003 through TASK-010 as relevant. English is canonical.

## Decision state

- Both modes use Unreal's native GAS. They retain separate catalogs, row schemas, execution systems, clocks, mode state, validators, and mode-specific abilities/effects.
- Shared identity or card-repository mapping does not imply one ability definition, AttributeSet, cooldown, sequence, or live ability instance.
- Exact same-card effect/numerical independence, initial art acceptance and temporary balance-profile authority are being clarified with the user; see the decision register.
- Encounter remains the first combat delivery. TASK-007 through TASK-010 keep their fixture-only scopes. TASK-005 is needed before the first random feature.
- The user remains the owner of undecided gameplay semantics. A task may be fully documented while its implementation remains gated.

## Scope and exclusive write set

Only the following Markdown pairs are writable in this task:

- Docs/Production/Tasks/TASK-011-Full-MVP-Task-Cluster.md and its .zh-CN.md companion.
- Every TASK-012 through TASK-034 pair listed in the delivery matrix below.
- Docs/Production/MVPDecisionRegister.md and .zh-CN.md.
- Docs/Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md and .zh-CN.md.
- Docs/Decisions/ADR-0002-Encounter-First-ACT-Preparation.md and .zh-CN.md, for the explicit partial-supersession note.
- Docs/Rules/Combat.md and .zh-CN.md.
- Docs/Design/EN/03_CombatAIPlanner.md and Docs/Design/ZH/03_CombatAIPlanner.md.
- Docs/Architecture/ProgramArchitecture.md, DataDrivenArchitecture.md, ModuleMap.md, RuntimeFlow.md and SaveSchema.md, each with its .zh-CN.md companion.
- Docs/Architecture/Modules/PadmaGameplay/README.md and ACTDevelopmentContract.md, each with its .zh-CN.md companion.
- Docs/Production/Roadmap.md, Docs/00_INDEX.md, Docs/ProjectState.md and Docs/Changelog.md, each with its .zh-CN.md companion.

Source, Content, Config, build files, TASK-005 through TASK-010, generated folders, .agents and .codex are outside the write set. Another Integration Coordinator was finishing TASK-007 and updating shared state at preflight; do not write ProjectState or Changelog until that writer finishes, then re-read and preserve its changes. No runtime work, merge, push, tag or release is part of this task.

## Delivery matrix and dependency order

The task matrix and executable order are populated as the contracts are written. A task number is an identity, not a promise that numerical order is dependency order.

## Child-task execution contract

Each child names one Primary Agent, narrow feature paths, prerequisites, decision gates, typed handoff, observable acceptance, checks, recovery and at most two learning targets. Child contracts start at Backlog until predecessor results and applicable decisions are recorded. Promotion must not invent a rule. The current request approves documentation creation; it does not assert runtime completion.

Use one Local feature branch per implementation task under the current operating policy. Keep one UE build/Editor/PIE lane. Shared headers, mode schemas, module READMEs, maps, build settings and final composition edits are serial. Read-only specialist review may run alongside the primary writer when it has a bounded package; no second writer is authorized by this cluster.

Every feature provides its own minimal view/log evidence and snapshot export/restore participation when it owns battle-mutated run state. Late integration wires approved public contracts and returns feature defects to their owners. TASK-034 does not own child source fixes.

## Delegation and review

Docs/Architecture/AGENTS.md requires architecture/affected-rule review. A read-only architect checks mode separation and partial supersession while the Primary Agent drafts. A separate read-only reviewer checks the completed contracts, dependency graph, exact paths, decision gates and English/Chinese consistency. They have no write authority.

## Acceptance criteria

- [ ] TASK-011 through TASK-034 have synchronized bilingual contracts with concrete acceptance and ownership.
- [ ] Both modes use GAS without a forced shared table, effect profile, state machine or runtime instance.
- [ ] Every accepted MVP requirement maps to an implementation task and final acceptance.
- [ ] Dependencies are acyclic and runtime gates identify their decision owner.
- [ ] The current task does not overwrite concurrent TASK-007/008/configuration work.
- [ ] Relevant rules, architecture, ADR, index, roadmap and state agree.
- [ ] Documentation-pair, link/task-graph, strict project and scoped whitespace checks pass.
- [ ] Independent review has no unresolved P0-P2 findings.

## Verification plan

Run Scripts/AuditDocs.ps1, Scripts/ValidateProject.ps1 -Strict, a task-link/dependency/write-set audit and a scoped git diff --check. AuditDocs checks companion existence; it does not validate links or translated meaning. Perform those checks separately. No UE compile or PIE is needed for documentation-only changes.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| Shared infrastructure versus independent game-mode contracts | Predict whether changing an ACT-only row can affect Encounter, inspect the contract, then explain a forbidden dependency. | Not started |
| Dependency-based delivery | Trace a playable milestone through prerequisites and identify a pending user decision. | Not started |

User practice: pick one task and trace its input, owned state and acceptance observation. Transfer: explain how a later FPS mode would reuse identity without inheriting ACT sequences. User evidence is recorded separately; writing these documents does not demonstrate learning.

## Risks and recovery

Main risks are disguised cross-mode coupling, a dependency cycle, speculative schemas and overlapping shared-state writers. Resolve them in the documentation before implementation. Recover by reverting only this task's approved documentation change after checking current ownership; preserve unrelated files.

## Completion report

- Final status: In Progress.
- Changed files and independent review: pending.
- Runtime/UE checks: not run; documentation-only scope.
- Agent-produced evidence: contract drafting in progress.
- User-produced learning evidence: not yet provided.
- Integration: working-tree documentation delivery; no merge or remote action.
