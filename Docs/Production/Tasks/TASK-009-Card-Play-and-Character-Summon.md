# TASK-009 Demo Card Play, Character Summon, and Skill Presentation

- Chinese companion: `TASK-009-Card-Play-and-Character-Summon.zh-CN.md`
- Document ID: `TASK-009`
- Version: `0.1`
- Status: `Review`
- Parent milestone or integration Goal: `TASK-006 fixed playable prototype vertical slice`
- Primary Role: `UI and Input Module Agent`
- Primary Agent: `module_worker`, assigned only after TASK-008 is approved and integrated
- Branch/worktree: Local feature branch `feature/TASK-009-card-summon-ui`; no independent Worktree by default
- Task mode: bounded Demo UI/input and card-command integration; no ACT or full card system

## Goal

Complete the visible card and skill part of the prototype-equivalent path. In the Demo Encounter scene, a card panel shows one configured character card; selecting it sends the approved battle-card command, the successful result causes TASK-008's summon boundary to create one character, and selecting that character displays its configured skill list in a read-only panel.

The card is a fixed basic non-A playtest fixture used during an eligible local Encounter action state. Its current-session visual state may become unavailable after play, but this task does not define the production discard, return, cooldown, draw, or cost rules.

## Why now

TASK-008 provides the local runtime, character definition, skill query, and summon boundary. The next useful result is the user-visible interaction that makes the flow feel playable, while keeping the UI as a command sender and view consumer rather than a second gameplay implementation.

## Read first

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Production/Tasks/TASK-006-Playable-Prototype-Cluster.md`
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.md`
- `Docs/Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.md`
- `Docs/Decisions/ADR-0003-Playable-Prototype-Vertical-Slice.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Content/AGENTS.md`
- `Source/AGENTS.md`

## Decision state

### Frozen inputs

- The card is one fixed basic non-A card in a local Encounter fixture and is played only in the explicitly eligible Demo state.
- UI input maps to the typed command path; the authoritative service validates and changes state, then emits a result/event or read-only view.
- The card references the stable Demo character identity; the skill panel reads the character/skill definition exposed by TASK-008.
- The card and skill panels are project-owned Demo presentation assets under `Content/Padma/Demo/`.
- The minimum UMG dependency may be added to the current staging module by this task, and only the designated writer may make that central Build.cs edit.

### Deferred or open inputs

- Card lifecycle probabilities, deck/draw rules, resource/cost policy, synthesis, random selection, rewards/failures, and persistence.
- The exact Encounter action-bar precedence and unresolved reaction/interrupt/extra-action card allowances.
- ACT `Tab` repository, five-slot paging, bullet time, total page count, ABC-card local-battle legality, GAS, and skill execution semantics.
- Final card/character schema, production ID namespace, import pipeline, localization, asset migration, and debug-panel data contract.

If the UI cannot be implemented without deciding one of these items, keep the fixture behavior explicit and return the decision; do not hide a product default in a Widget or Blueprint.

## Scope

### Allowed paths and write set

The Primary Agent may edit only:

- `Source/DreamOfPadma/DreamOfPadma.Build.cs` — add only the minimum UMG dependency required by this task; no unrelated dependency change
- `Source/DreamOfPadma/Public/Demo/UI/`
- `Source/DreamOfPadma/Private/Demo/UI/`
- `Content/Padma/Demo/Definitions/Cards/`
- `Content/Padma/Demo/UI/Card/`
- `Content/Padma/Demo/UI/Skill/`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-009-Card-Play-and-Character-Summon.md`
- `Docs/Production/Tasks/TASK-009-Card-Play-and-Character-Summon.zh-CN.md`

The task may consume TASK-007/008 public interfaces but may not edit their source, maps, definitions, session paths, `Config/DefaultEngine.ini`, shared ProjectState/index, or the final integration task.

### Out of scope

- Full card repository, hand/deck/draw/discard/return/cooldown lifecycle, resource payment, synthesis, random selection, save, or network behavior.
- Skill activation, targeting, animation, damage, status, combat, Encounter timeline, ACT, GAS, Debug panel, or external plugin use.
- Direct Widget mutation of gameplay state or direct `SpawnActor` from a card button.
- Broad UMG/Slate framework refactors, localization system, or bulk migration from `E:\2026ue\padma`.

## Dependencies and integration order

- Required predecessors: TASK-006/ADR-0003 approved; TASK-007 and TASK-008 implemented, reviewed, and integrated.
- The task consumes TASK-008's summon/query boundary and must not add a parallel spawn implementation.
- `TASK-010` integrates the default map, assembles the full route, and records manual PIE evidence.
- The Build.cs edit is central and serial. If a broader module/dependency change is needed, stop and request an architecture task/ADR.
- TASK-005 remains independent; this Demo card has no random draw or outcome.

### Shared conflicts

The UMG dependency edit and all `Content/Padma/Demo/UI/Card/`, `Content/Padma/Demo/UI/Skill/`, and Demo card-definition assets are exclusive to this task until Review completes. The UE build/Editor/PIE lane remains serial.

## Delegation plan

| Role | Work package | Authority | Expected output |
|---|---|---|---|
| `architect` | Check the command/view boundary and the minimum UMG dependency against the current staging-module architecture. | Read-only | Boundary and dependency findings. |
| `reviewer` | Review UI diff, command dispatch, invalid-result handling, asset references, and absence of hidden card/skill rules. | Read-only | P0-P3 findings and acceptance status. |

No second writer is authorized for the Build.cs edit or Demo UI assets.

## Acceptance criteria

- [ ] The local Demo Encounter displays one configured card in a visible card panel.
- [ ] Clicking the card sends the approved typed battle-card request; the Widget does not directly mutate state or spawn an Actor.
- [ ] A successful command result/event causes exactly one Demo character to appear through TASK-008's summon boundary, and the card's current-session presentation state updates visibly.
- [ ] Selecting the summoned character opens a read-only skill panel showing the configured skill identity/text/icon data from TASK-008's definition/query.
- [ ] An invalid, repeated, or unavailable card request presents the authoritative failure and does not create a duplicate character.
- [ ] The UI works with the approved local mouse/keyboard input path in PIE and does not implement ACT `Tab`/paging or a Debug panel.
- [ ] Only the minimum UMG Build.cs addition is made, and the UI module README pair records ownership and command/view boundaries.
- [ ] No card lifecycle, resource, random, combat, or Open/Deferred rule is invented.
- [ ] An independent Reviewer returns `Pass` or only explicitly accepted P3 improvements before integration.

## Verification plan

### Automated checks

- Compile the UE Editor target with the minimum UMG dependency.
- If command dispatch can be tested without UMG, run one narrow valid/invalid card-request check; do not introduce UI automation for this first slice.
- `pwsh -NoProfile -File .\Scripts\AuditDocs.ps1`.
- `pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict`.
- `git diff --check` and exact changed-path/dependency audit.

### Manual checks

1. Enter the Demo Encounter scene through the TASK-007 flow.
2. Observe one card in the card panel.
3. Click the card and observe the request/result path and one spawned character.
4. Attempt to play the same card again and observe a readable failure with no duplicate.
5. Select the spawned character and observe its configured skill list in the skill panel.
6. Confirm that no skill is activated and no hidden resource/card-lifecycle behavior is inferred.

### Required evidence

- Compile output and any narrow command check output.
- PIE screenshots or a short capture of the card panel, summon result, repeated-play failure, and skill panel.
- Changed-path and Build.cs diff audit.
- Independent Review report.

## Learning contract

| Target | Evidence required | Status |
|---|---|---|
| UI as command sender and view | The user identifies the Widget event, authoritative request, result/event, and view update in the card-play path. | `Not started` |
| Data-driven card-to-character-to-skill mapping | The user traces the three stable identities and predicts which authored data changes the card/skill display. | `Not started` |

- Learner's hands-on exercise: change the Demo skill text in its authored definition, run PIE, and explain why the Widget changes without editing Widget logic.
- Transfer question: if the card later gains a cost or cooldown, which service owns validation and which result/event should the UI display?

## Risks and recovery

- A convenient Widget callback could become the real summon implementation. Recovery: reject the diff and route through the TASK-008 boundary.
- Adding UMG could accidentally introduce Slate or unrelated plugins. Recovery: revert to the minimum module dependency and request architecture Review for any broader change.
- Card UI could imply an unapproved lifecycle. Recovery: label the visual state as current-session Demo behavior and leave production lifecycle out.
- Safe rollback is a Git revert of this task's implementation commit plus removal of only its owned Demo UI/card assets.

## Completion report

- Final status: `Review`; contract prepared, implementation not started.
- Primary Agent and Role: pending approval / `module_worker` / UI and Input Module Agent.
- Changed files: none yet; implementation paths are listed above.
- Acceptance evidence: pending implementation.
- Checks run and results: pending implementation.
- Checks not run and reason: runtime, compile, automation, and PIE work are not authorized while this contract is under Review.
- Review findings resolved or accepted: pending implementation Review.
- Remaining risks and open questions: all deferred inputs remain unresolved by design.
- English/Chinese documentation updated: contract pair prepared.
- Agent-produced learning evidence: pending.
- User-produced learning evidence: pending.
- Integration commit or handoff reference: waits for approval; no push.
