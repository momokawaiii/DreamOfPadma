# TASK-008 Encounter Demo Runtime, Character, and Skill Data

- Chinese companion: `TASK-008-Encounter-Character-and-Skill-Data.zh-CN.md`
- Document ID: `TASK-008`
- Version: `0.2`
- Status: `Verified`
- Parent milestone or integration Goal: `TASK-006 fixed playable prototype vertical slice`
- Primary Role: `Gameplay Module Agent`
- Primary Agent: current root / Gameplay Module Agent
- Branch/worktree: Local feature branch `codex/mvp-demo-foundations`; no independent Worktree by default
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

### Implementation amendment (2026-09-08)

The user's implementation request authorizes the missing fixed-scene Game adapter. Session publishes a receipt after committing valid context; delayed travel and consumption check that receipt, GameInstance and World. Initialization waits for the target scene's BeginPlay readiness; failure retains context without an automatic retry loop. Gameplay consumes typed values and does not depend on the concrete session subsystem. Add stale-receipt/F12/duplicate/missing-map checks. Existing world inputs and configuration remain unchanged.

Use mycard page 29's 维特鲁威人 and its three skill descriptions as read-only source text. D22d fixes C→R, but this presentation fixture has no attributes, costs or active effects. Placeholder geometry sizes are presentation parameters. Source text is not numerical approval.

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

- `Source/DreamOfPadma/Public/Demo/Integration/`
- `Source/DreamOfPadma/Private/Demo/Integration/`
- `Source/DreamOfPadma/Public/Demo/Session/DemoTransitionSessionSubsystem.h`
- `Source/DreamOfPadma/Private/Demo/Session/DemoTransitionSessionSubsystem.cpp`
- `Source/DreamOfPadma/Private/Demo/Session/DemoTransitionContextTest.cpp`
- `Scripts/Editor/AuthorDemoEncounter.py`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`

The task does not own other TASK-007 world/session paths, `DreamOfPadma.Build.cs`, `Config/DefaultEngine.ini`, card/UI paths, shared ProjectState/index, or the final integration task.

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

- [x] The local Demo Encounter scene can initialize from the TASK-007 context or reports a readable missing-context failure without partial state.
- [x] One project-owned Demo character definition loads through a typed data boundary and has a stable playtest identity, display data, presentation reference, and at least one skill definition.
- [x] The read-only character query returns the character's configured skill list without duplicating the data in a Widget or Actor instance.
- [x] The runtime exposes a bounded summon operation that accepts the Demo character identity and an explicit spawn-point context, creates/returns the placeholder runtime representation, and emits a typed success/failure result or event.
- [x] The summon operation rejects an unknown character or invalid spawn context without spawning a partial object or mutating unrelated state.
- [x] No skill is executed, no combat rule is selected, and no Open/Deferred gameplay decision is invented.
- [x] The module README pair documents ownership, data path, summon/query boundary, tests, and remaining risks.
- [x] An independent Reviewer returns `Pass` or only explicitly accepted P3 improvements before integration.

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

- Final status: Verified; root / Gameplay Module Agent on Local codex/mvp-demo-foundations.
- Implemented: typed character/skill definitions, read-only query, validated world-owned summon service, Encounter scene, fixed Game travel adapter with publication receipts, and a scene GameMode without a default Pawn. The opt-in smoke capture waits eight actual frames; normal play has no automatic summon.
- Content: L_DemoEncounter.umap, DA_Demo_VitruvianMan and three source skill assets. AuthorDemoEncounter.py creates missing assets and preserves existing content.
- Verification (2026-09-08): UE5.8.2 Editor compile passed; four project automation tests passed with zero test warnings/failures in automation-reviewed/index.json. The tests cover stale/F12/duplicate receipt handling, failed initialization, zero scale/invalid rotation, definition refresh, invalid/repeated summon, preserved spawn transform and ACT authoring.
- Real runtime evidence: travel-render-reviewed.log records DemoSandbox → OpenLevel → receipt consumption → one Vitruvian Man representation → three configured skills, with Pending=0. encounter-reviewed.png was captured at frame 11 and visually checked: oblique camera, readable title/status, central spawn marker and one placeholder representation.
- Asset checks: author-encounter.log and author-finalize.log report zero commandlet errors/warnings; both scripts reran against existing content. Build and validation artifacts are under C:/Users/lenovo/.codex/visualizations/2026/09/08/01a07f68-77cf-7fe0-8479-93a66fb8a4f6/runtime-review.
- Review fixes: per-axis transform validation; safe failed-test cleanup; correct Sphere asset; stale receipt guard; late-frame capture and no extraneous default Pawn. Independent read-only Reviewer /root/full_mvp_contract_review returned Pass on 2026-09-08 with no remaining P0–P2 findings after inspecting the final build, four-test report, authoring rerun and rendered image.
- Environment note: Chinese-culture startup smoke has 13 Condition failed assertions already present in pre-change Saved/Logs/DreamOfPadma.log and the 2026.09.07-18.08.16 backup. The final test process used -culture=en and had none; project/user culture was not changed. The older world-context test warning is absent from the latest report. Auto-review initially rejected generated-output commands, then allowed normal UE generation after repository script-rule evidence; no approval block remains.
- User reproduction: open DemoSandbox, PIE, click its tile, Enter to travel, then F8 to request the fixture summon. Character/skill definitions are under Content/Padma/Demo/Definitions. Card and skill panels remain TASK-009.
- Documentation: task and module pairs synchronized. Final handoff passed AuditDocs (186 Markdown files / 93 pairs), ValidateProject -Strict, git diff --check, 946 local-link checks, 25 task metadata/dependency checks and the exact 136-path audit (108 Markdown / 28 implementation paths). Protected configuration/build/project hashes match the preflight baseline.
- Agent learning evidence: definition/query/instance separation, validated commands and receipt lifetime demonstrated by code/tests. User PIE acceptance and teach-back are not claimed.
- Changelog: the Coordinator finalized the bilingual Added entry — source-based Encounter scene/query/summon and fixed scene travel. Active effects, GAS combat, card lifecycle, full graph/map storage, calendar, economy and saving are not implemented by this task.
- Integration: working-tree candidate only; no commit, merge or push.
