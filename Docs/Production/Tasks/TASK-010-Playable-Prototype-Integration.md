# TASK-010 End-to-End Playable Prototype Integration

- Chinese companion: `TASK-010-Playable-Prototype-Integration.zh-CN.md`
- Document ID: `TASK-010`
- Version: `0.1`
- Status: `Review`
- Parent milestone or integration Goal: `TASK-006 fixed playable prototype vertical slice`
- Primary Role: `Integration Coordinator`
- Primary Agent: `Integration Coordinator`; no separate implementation Agent
- Branch/worktree: integration-owned Local checkout; no independent Worktree
- Task mode: serial integration, configuration, evidence, and status closure; no feature implementation or bug fixing in child write sets

## Goal

Integrate the reviewed TASK-007, TASK-008, and TASK-009 results into one reproducible playable path. The approved default map opens the Demo Sandbox, the user selects the Demo tile, enters the mapped Encounter scene, plays the Demo card, sees the character spawn, and reads the character's skill list. The task leaves compile, manual PIE, documentation, and integration evidence that can be reviewed before GitHub backup.

## Why now

The three child implementation tasks intentionally own separate world, local runtime, and UI/data paths. A final serial integration task is needed to bind the default entry, verify the actual cross-task route, reject scope drift, and record the user-facing evidence without allowing an integration fixer to silently change a child task's rules.

## Read first

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Production/Tasks/TASK-006-Playable-Prototype-Cluster.md`
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.md`
- `Docs/Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.md`
- `Docs/Production/Tasks/TASK-009-Card-Play-and-Character-Summon.md`
- `Docs/Decisions/ADR-0003-Playable-Prototype-Vertical-Slice.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Production/BuildMatrix.md`
- `E:\2026ue\padma` as a read-only reference for the expected visible route

## Decision state

### Frozen inputs

- TASK-007, TASK-008, and TASK-009 must each have an independent Review result before integration.
- The final path is the fixed Demo fixture, not a general SLG route, full battle, or production card loop.
- The default map may be changed to the project-owned Demo Sandbox map only through the explicitly allowed central configuration edit.
- Manual PIE acceptance is the primary end-to-end gate. A debug panel and broad UI automation are not prerequisites.
- The user remains the final acceptance owner for the visible flow; the Coordinator records the user's observation separately from Agent evidence.

### Deferred or open inputs

- All Open/Proposed/Deferred/playtest-only gameplay, route, card, resource, random, save, ACT, GAS, import, ID, World Partition, and asset-migration decisions remain unchanged.
- Any defect in child source or assets must return to the owning TASK; this integration task must not fix it by widening its write set.

## Scope

### Allowed paths and write set

The Integration Coordinator may edit only:

- `Config/DefaultEngine.ini` — set or update only the Demo Sandbox default map entry required by this slice
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

Child TASK files may receive completion evidence and status updates only; their Goal, scope, and acceptance boundary may not be rewritten during integration.

### Out of scope

- Editing child source, Content assets, maps, module Build.cs, or UI widgets to repair a failing integration.
- Adding a general route resolver, combat, skill execution, random, resource, save, ACT, GAS, Debug panel, packaging, or asset migration.
- Marking a task `Done` without independent Review, required checks, and user PIE evidence.
- Merging, tagging, pushing, or creating a GitHub release in this task.

## Dependencies and integration order

- Required predecessors: TASK-006 cluster/ADR-0003 approved; TASK-007, TASK-008, and TASK-009 implemented and independently reviewed with no blocking findings.
- Integrate in order: world selection/context -> Encounter runtime/character data -> card/UI -> default-map configuration -> end-to-end PIE.
- TASK-005 remains `Review` and is not changed by this task.
- If the current project setting or build dependency conflicts with a child result, stop and request a new scoped architecture/configuration task.
- No second UE Editor/PIE lane may run during the final manual acceptance.

### Shared conflicts

This task owns the serial edit to `Config/DefaultEngine.ini` and shared documentation state. It does not own the child source or binary asset directories. Any failed child acceptance is returned to that child task rather than patched here.

## Delegation plan

No writable subagent is authorized. A separate read-only reviewer may inspect the integrated diff and evidence:

| Role | Work package | Authority | Expected output |
|---|---|---|---|
| `reviewer` | Check integration diff, default-map change, end-to-end evidence, task status, and preserved Open/Deferred items. | Read-only | P0-P3 findings and `Pass`/`Block`. |
| `learning-tutor` | Check that the user's manual route and teach-back are recorded separately from Agent evidence. | Read-only | Learning evidence review. |

## Acceptance criteria

- [ ] The project starts in the project-owned Demo Sandbox map when launched through the approved default entry.
- [ ] The complete route works in one PIE session: select the Demo tile -> enter its mapped Encounter scene -> see one card -> play it -> see one summoned character -> select it -> read its skill list.
- [ ] The scene transition carries the expected stable Demo context and does not rely on stale Actor pointers.
- [ ] The card and skill UI reflect authoritative result/event or read-only view data and do not contain a parallel summon or skill implementation.
- [ ] Replaying the card or using an invalid fixture produces a readable failure without an unintended duplicate or partial state.
- [ ] Compile, required documentation checks, independent child Reviews, and manual PIE evidence are recorded.
- [ ] `TASK-005` remains unchanged and all Open/Proposed/Deferred/playtest-only decisions remain explicitly unresolved.
- [ ] The user has personally confirmed the visible route before the child tasks and this integration task are closed.

## Verification plan

### Automated checks

- Compile the UE Editor target in the approved single build lane.
- `pwsh -NoProfile -File .\Scripts\AuditDocs.ps1`.
- `pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict`.
- `git diff --check` and exact changed-path/config audit.
- Do not require broad UI automation. Add a narrow service test only if it already exists from a child task and does not expand scope.

### Manual checks

1. Launch the default map in PIE.
2. Select and confirm the Demo SLG tile.
3. Verify the mapped Encounter scene and preserved node/scenario/spawn context.
4. Observe the one Demo card.
5. Play it and verify exactly one character appears at the configured spawn point.
6. Select the character and verify the configured skill list.
7. Attempt a repeated/invalid play and verify a readable failure without an extra character.
8. Record the user's acceptance observation and a screenshot or short capture.

### Required evidence

- UE Editor compile output.
- Manual PIE screenshot/video covering every step.
- Independent Review reports for TASK-007 through TASK-009 and this integration diff.
- Exact changed-path list, default-map diff, documentation audit, and final ProjectState/task status.
- Separate Agent-produced and user-produced learning evidence.

## Learning contract

| Target | Evidence required | Status |
|---|---|---|
| End-to-end ownership tracing | The user can name the owner of tile selection, transition context, Encounter state, summon result, and skill view. | `Not started` |
| Evidence-based integration | The user personally follows the route, records one expected observation and one failure observation, and explains why the integration task cannot silently fix child scope. | `Not started` |

- Learner's hands-on exercise: perform the complete PIE route and write down the first stable identity observed at each scene boundary.
- Transfer question: if the second tile fails to enter its scene, should the integration task patch the map, the world task, or the transition contract, and what evidence determines that owner?

## Risks and recovery

- A default-map change can break the already accepted foundation map. Recovery: keep the prior map value in Git history and revert only the explicit config line if the Demo map cannot launch.
- Integration may hide a child defect through a one-off Blueprint fix. Recovery: stop, return the defect to the owning TASK, and preserve the exact evidence.
- Asset dependencies from the reference project may fail in another checkout. Recovery: use project-owned placeholders and record asset migration as a future task.
- Safe rollback is a Git revert of the integration/config/documentation commit; do not delete child assets or generated folders.

## Completion report

- Final status: `Review`; contract prepared, integration not started.
- Primary Agent and Role: Integration Coordinator / Integration Coordinator.
- Changed files: none yet; integration paths are listed above.
- Acceptance evidence: pending child Reviews, compile, and user PIE acceptance.
- Checks run and results: pending integration.
- Checks not run and reason: no integration or PIE work is authorized while this contract is under Review.
- Review findings resolved or accepted: pending.
- Remaining risks and open questions: all deferred inputs remain unresolved by design.
- English/Chinese documentation updated: contract pair prepared.
- Agent-produced learning evidence: pending.
- User-produced learning evidence: pending.
- Integration commit or handoff reference: waits for user approval and later GitHub backup; no push.
