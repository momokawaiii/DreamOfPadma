# TASK-006 Playable Prototype Vertical Slice Cluster

- Chinese companion: `TASK-006-Playable-Prototype-Cluster.zh-CN.md`
- Document ID: `TASK-006`
- Version: `0.1`
- Status: `Done`
- Parent milestone or integration Goal: `Prototype-equivalent playable flow / fixed-fixture vertical slice`
- Primary Role: `Integration Coordinator`
- Primary Agent: `Integration Coordinator` for this serial contract-design Goal; child implementation Agents are assigned only after approval
- Branch/worktree: integration-owned checkout; no independent Worktree
- Task mode: bilingual architecture decision and downstream TASK-cluster design only; no source, asset, config, or test implementation

## Goal

Create and review a bilingual task cluster that can deliver the smallest playable flow comparable to the functional behavior demonstrated by `E:\2026ue\padma`: enter the game, select one SLG tile, enter its mapped local scene, show one character card, play the card to summon the character, and inspect the character's skill list.

The observable result of this task is an approved-ready cluster contract and ADR, not runtime code. The cluster deliberately uses one fixed playtest fixture and preserves all unresolved gameplay rules.

## Why now

The default map PIE launch is already accepted, while the current project still has only a generated runtime module. The user has prioritized seeing a playable presentation route before the complete M1 random/test foundation. The reference project provides interaction and presentation guidance, but its implementation is not the architecture source for DreamOfPadma.

## Read first

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/ModuleMap.md`
- `Docs/Architecture/SaveSchema.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaTests/README.md`
- `Docs/Production/Tasks/TASK-003-Core-Contracts.md`
- `Docs/Production/Tasks/TASK-004-ACT-Architecture-Decision.md`
- `Docs/Production/Tasks/TASK-005-Deterministic-Random-Foundation.md`
- `E:\2026ue\padma` as read-only visual and behavior reference only

## Decision state

### Frozen inputs

- The current default map PIE launch is accepted.
- The old prototype is reference-only; the clean DreamOfPadma project is the implementation source.
- Stable identity is separate from display names, asset paths, Actor pointers, and map coordinates.
- State-changing work follows command -> validation -> authoritative state change -> typed result/event.
- UI presents state and sends commands; it is not the owner of world or gameplay truth.
- The first slice may use one explicit fixture mapping and placeholder presentation assets.
- The current generated `DreamOfPadma` module remains the staging home; no permanent module split, GAS, or external gameplay plugin is authorized.

### Deferred or open inputs

- General world-graph routing, movement costs, resource costs, ownership, mutations, and joint battle-route resolution.
- Whether the overworld and local scene share World Partition, and the final level-streaming strategy.
- Card lifecycle, deck/draw rules, reward/failure semantics, synthesis, random outcomes, and save/rollback implementation.
- Exact Encounter action-bar precedence, response/interrupt windows, card allowance in unresolved windows, and skill execution semantics.
- ABC-card legality inside local battle, ACT implementation, GAS ownership, final import pipeline, production ID namespace, asset licensing, and network authority.
- The in-PIE debug panel and broad automated UI testing.

If an implementation task needs one of these decisions to pass acceptance, it must stop and return the decision to the named owner; it must not add a default.

## Scope

### Allowed paths and write set

This contract-design task may edit only:

- `Docs/Decisions/ADR-0003-Playable-Prototype-Vertical-Slice.md`
- `Docs/Decisions/ADR-0003-Playable-Prototype-Vertical-Slice.zh-CN.md`
- `Docs/Production/Tasks/TASK-006-Playable-Prototype-Cluster.md`
- `Docs/Production/Tasks/TASK-006-Playable-Prototype-Cluster.zh-CN.md`
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.md`
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.zh-CN.md`
- `Docs/Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.md`
- `Docs/Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.zh-CN.md`
- `Docs/Production/Tasks/TASK-009-Card-Play-and-Character-Summon.md`
- `Docs/Production/Tasks/TASK-009-Card-Play-and-Character-Summon.zh-CN.md`
- `Docs/Production/Tasks/TASK-010-Playable-Prototype-Integration.md`
- `Docs/Production/Tasks/TASK-010-Playable-Prototype-Integration.zh-CN.md`
- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`
- `Docs/ProjectState.md`
- `Docs/ProjectState.zh-CN.md`

No `Source/`, `Content/`, `Config/`, build, generated, or test implementation file is writable under TASK-006.

### Out of scope

- Any runtime C++, Blueprint, map, `.uasset`, input, UMG, automation, or packaging implementation.
- Changing accepted rules or deciding any Open/Proposed/Deferred/playtest-only item.
- Approving TASK-005, starting an implementation Agent, allocating a Worktree, merging, tagging, or pushing.

## Dependencies and integration order

### Child TASK cluster

| Task | Result | Depends on | Primary ownership |
|---|---|---|---|
| `TASK-007` | One demo SLG tile can be selected and produces a cross-scene transition context. | `TASK-006` approved | World/Game composition |
| `TASK-008` | One local Encounter scene loads the context, owns the demo character/skill definitions, and exposes a summon/read-only query boundary. | `TASK-007` approved | Gameplay |
| `TASK-009` | Card and skill panels send commands; playing one demo card summons the character and displays its skills. | `TASK-008` approved | UI and input |
| `TASK-010` | Default entry and the complete manual PIE route are integrated and evidenced. | `TASK-007` through `TASK-009` independently reviewed | Integration Coordinator |

The tasks are designed now as a cluster but must be implemented serially at first. They share one generated module, maps/assets, central build configuration, and one UE build/Editor/PIE lane. Parallel implementation is not authorized by this contract.

`TASK-005 Deterministic Random Foundation` remains `Review` and is not a prerequisite because this cluster uses no random outcome. It can be revisited after the visible slice or when an approved feature needs deterministic randomness.

## Delegation plan

No writable subagent is authorized. Before user approval, the Integration Coordinator may use read-only checks from:

| Role | Work package | Authority | Expected output |
|---|---|---|---|
| `architect` | Check the proposed fixture, transition boundary, staging-module choice, and dependency/ADR implications. | Read-only | Blockers or boundary confirmation. |
| `level-content-planner` | Check that one demo node and one mapped local scene are sufficient without deciding the final world graph. | Read-only | Fixture and presentation-scope review. |
| `reviewer` | Inspect this contract pair and child task pairs for scope, ownership, language synchronization, and preserved Open/Deferred items. | Read-only | P0-P3 findings and acceptance status. |

## Acceptance criteria

- [x] ADR-0003 and TASK-006 have synchronized English/Chinese pairs.
- [x] Child TASK-007 through TASK-010 each have one observable Goal, exact write set, non-goals, dependencies, acceptance criteria, tests, learning targets, and worktree decision.
- [x] The child write sets do not overlap on source directories, UI asset directories, maps, central configuration, module documentation, or integration state.
- [x] The fixed fixture is sufficient to demonstrate the requested flow without implementing a general route, combat, random, save, or card-lifecycle rule.
- [x] The cluster explicitly preserves all relevant Open/Proposed/Deferred/playtest-only items.
- [x] TASK-005 is not silently changed or made a prerequisite.
- [x] Index and ProjectState entries identify ADR-0003 as accepted and TASK-006 as complete; no implementation Agent is authorized by TASK-006.
- [x] No source, asset, config, build, generated, or test implementation file changes in this design task.

## Verification plan

### Automated checks

- `pwsh -NoProfile -File .\Scripts\AuditDocs.ps1` — all Markdown pairs and links pass.
- `pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict` — project structure remains valid.
- `git diff --check` — no whitespace errors.
- A changed-path audit confirms that only the listed Markdown paths changed.

### Manual checks

- Trace the requested user flow from the cluster Goal through TASK-007, TASK-008, TASK-009, and TASK-010.
- Confirm that UI never owns authoritative state and that the transition context uses stable IDs rather than Actor pointers or coordinates as identity.
- Confirm that the reference project is used only to identify visible behavior and not as an implementation source.
- Confirm that TASK-005 remains separate and that no child task requires an unresolved rule.

### Required evidence

- Audit, strict validation, and whitespace-check output.
- Exact changed-path list.
- Coordinator design review with findings and conditions.
- User approval before any child task is set to `Ready` or any implementation Agent is started.

## Learning contract

| Target | Evidence required | Status |
|---|---|---|
| Vertical-slice decomposition | The user can map each visible step to one child task and explain why the tasks are serial despite being designed together. | `Not started` |
| Fixture versus production rule | The user can identify one demo fixture and one Open/Deferred rule that must remain outside the implementation. | `Not started` |

- Learner's hands-on exercise: draw `tile selection -> transition context -> local state -> card command -> summon result -> skill view` and mark the owner of each state.
- Transfer question: when the demo needs a second tile or a second card, which part should become a data definition, and which new behavior would require a separate approved TASK?

## Risks and recovery

- The cluster could grow into a full game implementation. Recovery: keep one node, one scene, one card, one character, and read-only skills; create a new task for expansion.
- A child task could make a fixture a permanent gameplay rule. Recovery: return it to Review and label the behavior as playtest-only or request a design decision.
- Shared map/asset/build writes could be attempted in parallel. Recovery: stop the writer and preserve the serial order.
- The reference project's assets may carry unavailable dependencies or licensing uncertainty. Recovery: use placeholders and create a separate, explicitly scoped asset-migration task.

## Completion report

- Final status: `Done`; design review completed and user approval recorded on 2026-09-07.
- Primary Agent and Role: Integration Coordinator / Integration Coordinator.
- Changed files: ADR-0003 pair, TASK-006 through TASK-010 pairs, index pair, and ProjectState pair.
- Acceptance evidence: AuditDocs.ps1 passed for 122 Markdown files and 61 language pairs; ValidateProject.ps1 -Strict passed; git diff --check passed; the changed-path audit contains only the planned bilingual documentation and ADR paths; no Source/Content/Config/Build/generated/test implementation path changed.
- Checks run and results: pwsh -NoProfile -File E:\2026ue\DreamOfPadma\Scripts\AuditDocs.ps1 passed; pwsh -NoProfile -File E:\2026ue\DreamOfPadma\Scripts\ValidateProject.ps1 -Strict passed; git -c safe.directory=E:/2026ue/DreamOfPadma -C E:\2026ue\DreamOfPadma diff --check passed; bilingual pair, required-section, placeholder, and child-write-set read-only audits passed.
- Checks not run and reason: no runtime, compile, automation, PIE, or packaging checks belong to this design-only TASK.
- Review findings resolved or accepted: Coordinator design Review was Pass with conditions: the cluster has one fixed fixture, serial dependencies, non-overlapping child implementation paths, explicit central-file ownership, and preserved Open/Deferred inputs. ADR-0003 is accepted and TASK-006 is Done. Child TASK-007 through TASK-010 remain separately gated; TASK-006 did not start an implementation Agent.
- Remaining risks and open questions: all items under **Deferred or open inputs** remain unresolved by design.
- English/Chinese documentation updated: yes, after the design patch.
- Agent-produced learning evidence: contract decomposition only.
- User-produced learning evidence: pending.
- Integration commit or handoff reference: uncommitted working-tree documentation awaiting the user's commit and push; no implementation Agent started.
