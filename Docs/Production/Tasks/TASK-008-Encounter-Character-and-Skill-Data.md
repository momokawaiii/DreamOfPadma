# TASK-008 Encounter Demo Runtime, Character, and Skill Data

- Chinese companion: `TASK-008-Encounter-Character-and-Skill-Data.zh-CN.md`
- Document ID: `TASK-008`
- Version: `0.1`
- Status: `Ready`
- Parent milestone or integration Goal: `TASK-006 fixed playable prototype vertical slice`
- Primary Role: `Gameplay Module Agent`
- Primary Agent: `module_worker`, assigned when the new implementation session starts
- Branch/worktree: Local feature branch `feature/TASK-008-encounter-character-skill`; no independent Worktree by default
- Task mode: bounded local-scene runtime, character/skill fixture, and summon boundary; no combat implementation

## Goal

Implement the local Encounter half of the fixed prototype slice. The target scene can consume TASK-007's transition context, initialize one project-owned Demo Encounter, load one data-driven character definition with a small skill list, expose a read-only character/skill query, and provide an authoritative character-summon boundary for TASK-009's card command.

The observable result is an Encounter scene and runtime boundary that can show where a character would be summoned and can return its skill definitions. It does not activate a skill or resolve combat.

## Why now

TASK-007 supplies the stable scene context. The reference project combines card data and skills inside a large manager; DreamOfPadma needs the smallest reusable local runtime boundary so the later UI can request a summon and read the same character data without embedding rules in a Widget.

## Read first

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Production/Tasks/TASK-006-Playable-Prototype-Cluster.md`
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.md`
- `Docs/Decisions/ADR-0003-Playable-Prototype-Vertical-Slice.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaTests/README.md`
- `Content/AGENTS.md`
- `Source/AGENTS.md`

## Decision state

### Frozen inputs

- The local scene is one fixed playtest Encounter fixture selected by TASK-006/007.
- Character identity, skill identity, display text, and presentation references are data-driven and separated from live Actor pointers.
- The character skill query is read-only and returns the same definition used by the summon/runtime boundary.
- The summon boundary receives a stable character identity and an explicit local spawn context; it returns a typed result/event that the UI can observe.
- Demo definitions may use a small project-owned Primary Data Asset or equivalent typed fixture under `Content/Padma/Demo/`; this is not a final project-wide import decision.
- Placeholder meshes/materials are acceptable. No external prototype asset tree is required.

### Deferred or open inputs

- Final CardDefinition/CharacterDefinition schema, production ID namespace, import pipeline, Asset Manager policy, and content localization policy.
- Skill activation contexts, target rules, costs, cooldowns, animation timing, effects, damage, status, AI, Encounter action-bar precedence, and battle outcomes.
- Card draw/lifecycle, resource costs, random outcomes, save/rollback, networking, ACT/GAS, and final scene-streaming policy.

The task may expose a fixture-only query and summon operation, but it must not turn any of these unresolved items into a product rule.

## Scope

### Allowed paths and write set

The Primary Agent may edit only:

- `Source/DreamOfPadma/Public/Demo/Encounter/`
- `Source/DreamOfPadma/Private/Demo/Encounter/`
- `Content/Padma/Demo/Encounter/`
- `Content/Padma/Demo/Definitions/Characters/`
- `Content/Padma/Demo/Definitions/Skills/`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.md`
- `Docs/Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.zh-CN.md`

The task does not own TASK-007 session/world paths, `DreamOfPadma.Build.cs`, `Config/DefaultEngine.ini`, card/UI paths, shared ProjectState/index, or the final integration task.

### Out of scope

- Full Encounter action timeline, initiative, priority, reactions, interrupts, extra actions, end-turn effects, combat formulas, statuses, AI, victory/defeat, save, or battle transaction.
- Skill casting or effect execution; the skill list is read-only presentation data.
- Card hand, card draw, card lifecycle, resource payment, random selection, synthesis, ACT, GAS, Debug panel, or a universal Manager.
- Hard-coded character/skill data in Widgets or map Actors.
- Bulk copying or editing of `E:\2026ue\padma` assets.

## Dependencies and integration order

- Required predecessors: TASK-006/ADR-0003 approved; TASK-007's transition context and Demo World contract approved and integrated.
- TASK-005 remains independent and is not required; the Demo character and skill fixture has no random outcome.
- TASK-009 consumes the read-only character query and summon boundary. It must not edit this task's source or definition paths.
- TASK-010 consumes the Encounter map and local runtime and performs the end-to-end PIE check.
- If a new public Core schema, module dependency, save shape, or plugin is required, stop and request an architecture task/ADR rather than expanding this write set.

### Shared conflicts

The Encounter map, character/skill Demo definitions, and `Demo/Encounter/` source paths are exclusive to this task until independent Review completes. The UE build/Editor/PIE lane is serial. No other writer may modify these assets or paths during implementation.

## Delegation plan

| Role | Work package | Authority | Expected output |
|---|---|---|---|
| `architect` | Check the read-only definition/query and summon boundary against Core/Gameplay ownership and future save/network seams. | Read-only | Boundary findings or confirmation. |
| `reviewer` | Review actual runtime/data diff, fixture-only limits, asset loading, and preserved Open/Deferred items. | Read-only | P0-P3 findings and acceptance status. |

No second writer is authorized for the Encounter map, character definitions, skill definitions, or summon source.

## Acceptance criteria

- [ ] The local Demo Encounter scene can initialize from the TASK-007 context or reports a readable missing-context failure without partial state.
- [ ] One project-owned Demo character definition loads through a typed data boundary and has a stable playtest identity, display data, presentation reference, and at least one skill definition.
- [ ] The read-only character query returns the character's configured skill list without duplicating the data in a Widget or Actor instance.
- [ ] The runtime exposes a bounded summon operation that accepts the Demo character identity and an explicit spawn-point context, creates/returns the placeholder runtime representation, and emits a typed success/failure result or event.
- [ ] The summon operation rejects an unknown character or invalid spawn context without spawning a partial object or mutating unrelated state.
- [ ] No skill is executed, no combat rule is selected, and no Open/Deferred gameplay decision is invented.
- [ ] The module README pair documents ownership, data path, summon/query boundary, tests, and remaining risks.
- [ ] An independent Reviewer returns `Pass` or only explicitly accepted P3 improvements before integration.

## Verification plan

### Automated checks

- Compile the UE Editor target.
- Run a narrow data/query and invalid-summon check if the boundary can be exercised without loading the full map; do not create a UI automation suite.
- `pwsh -NoProfile -File .\Scripts\AuditDocs.ps1`.
- `pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict` when project structure changes.
- `git diff --check` and exact changed-path audit.

### Manual checks

1. Enter the Demo Encounter scene with the TASK-007 context.
2. Verify the local scene finds the expected spawn point and scenario identity.
3. Inspect the Demo character definition and confirm at least one skill is present.
4. Invoke the bounded summon operation through its public command/service boundary.
5. Confirm one placeholder character appears at the expected point and an invalid identity produces a readable failure with no extra character.

TASK-009 is responsible for rendering the card and skill panels around this boundary.

### Required evidence

- Compile output and any narrow query/summon test output.
- Screenshot or short PIE capture of the local scene, summoned placeholder, and definition evidence.
- Changed-path list and independent Review report.
- A note that no skill execution or unresolved rule was implemented.

## Learning contract

| Target | Evidence required | Status |
|---|---|---|
| Definition versus runtime state | The user identifies which fields belong to the immutable character/skill definition and which belong to a spawned runtime instance. | `Not started` |
| Read-only query and authoritative summon | The user traces why the UI asks a service to summon and reads a view instead of mutating an Actor or Widget-owned copy. | `Not started` |

- Learner's hands-on exercise: change the Demo character's skill description in the authored definition and predict which runtime/view observation changes without changing the summon command.
- Transfer question: if a future skill gains a cooldown, where should the authored default and the per-battle current value live?

## Risks and recovery

- Temporary fixture types could be mistaken for the final card/character schema. Recovery: label them playtest-only and return any production-schema request to architecture Review.
- A summon service could become a universal manager. Recovery: keep it limited to the Demo Encounter boundary and typed requests/results.
- Asset loading could hide missing references until PIE. Recovery: validate definitions at initialization and return a readable failure.
- Safe rollback is a Git revert of this task's implementation commit plus removal of only its owned Demo Encounter assets.

## Completion report

- Final status: `Ready`; TASK-007 was locally integrated after independent Review Pass on 2026-09-08, so this approved contract is unlocked for a new implementation session; implementation not started.
- Primary Agent and Role: assigned when the new session starts / `module_worker` / Gameplay Module Agent.
- Changed files: none yet; implementation paths are listed above.
- Acceptance evidence: predecessor gate satisfied by the locally integrated TASK-007 transition context; TASK-008 runtime acceptance remains pending implementation.
- Checks run and results: contract and repository documentation gates were validated during TASK-007 integration; no TASK-008 runtime result is claimed.
- Checks not run and reason: TASK-008 runtime, compile, automation, and PIE work has not started; this Ready transition does not authorize implementation evidence by itself.
- Review findings resolved or accepted: the contract-level Review gate is complete for the current scope; implementation Review remains required after the Primary Agent handoff.
- Remaining risks and open questions: all deferred inputs remain unresolved by design.
- English/Chinese documentation updated: contract pair prepared.
- Agent-produced learning evidence: pending.
- User-produced learning evidence: pending.
- Integration commit or handoff reference: TASK-007 local closeout commit `9ea99c7` satisfies the predecessor gate; TASK-008 is ready for Local branch `feature/TASK-008-encounter-character-skill`; no implementation commit and no push.
