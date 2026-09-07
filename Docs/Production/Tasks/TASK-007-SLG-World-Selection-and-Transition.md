# TASK-007 SLG Demo World Selection and Transition

- Chinese companion: `TASK-007-SLG-World-Selection-and-Transition.zh-CN.md`
- Document ID: `TASK-007`
- Version: `0.1`
- Status: `Review`
- Parent milestone or integration Goal: `TASK-006 fixed playable prototype vertical slice`
- Primary Role: `World Module Agent` with Game composition support
- Primary Agent: `module_worker` / World Module Agent for this implementation session
- Branch/worktree: Local feature branch `feature/TASK-007-world-selection-transition`; no independent Worktree by default
- Task mode: bounded runtime and demo-map implementation; no general world-rule implementation

## Goal

Implement the first visible SLG step of the prototype-equivalent slice: PIE can display one project-owned demo world map, the user can select and highlight one demo tile, and confirming the selection produces a typed transition context containing the demo node, scenario, and spawn-point identities for the local scene.

The task may provide a replaceable direct level-load adapter for the later Encounter scene, but it must not decide the final world graph, movement rules, or World Partition strategy.

## Why now

TASK-006 freezes the fixed-fixture vertical slice and its non-canonical boundaries. The default-map PIE launch is already accepted, so the next observable result is a user-visible strategic selection that can hand off stable context instead of passing an Actor pointer or raw coordinate to the next scene.

## Read first

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Production/Tasks/TASK-006-Playable-Prototype-Cluster.md`
- `Docs/Decisions/ADR-0003-Playable-Prototype-Vertical-Slice.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/ModuleMap.md`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Design/EN/04_LevelContentPlanner.md`
- `Content/AGENTS.md`
- `Source/AGENTS.md`

## Decision state

### Frozen inputs

- The task uses one explicitly marked playtest fixture from TASK-006: one selectable node and one target local scenario.
- Selection is presentation/input state; entering the selected demo scenario goes through a typed application request and a transition context.
- Stable IDs identify the demo node, scenario, and spawn point. Coordinates and level asset paths are presentation/configuration data, not save identity.
- The context may live for the scene transition in the GameInstance/session boundary; it must not become a universal gameplay manager.
- The project-owned map and tile assets live under `Content/Padma/Demo/World/`.
- The task does not add a new UE module or external dependency.

### Deferred or open inputs

- General graph topology, adjacency, route constraints, movement costs, resource costs, ownership, discovery, capture, facilities, world mutations, and enemy occupancy.
- Joint battle-route resolution and whether the map and local scene share World Partition.
- Final scene streaming policy, save schema, production ID namespace, import pipeline, and asset migration from the reference project.
- Encounter action-timeline rules, card legality beyond the fixed later fixture, random outcomes, and all other Open/Deferred design items.

If the task cannot pass without one of these decisions, stop and return the decision; use neither a guessed default nor a hidden hard-coded rule.

## Scope

### Allowed paths and write set

The Primary Agent may edit only:

- `Source/DreamOfPadma/Public/Demo/Session/`
- `Source/DreamOfPadma/Private/Demo/Session/`
- `Source/DreamOfPadma/Public/Demo/World/`
- `Source/DreamOfPadma/Private/Demo/World/`
- `Content/Padma/Demo/World/`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaWorld/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.md`
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.zh-CN.md`

`Config/DefaultEngine.ini`, `DreamOfPadma.Build.cs`, `Docs/00_INDEX*`, `Docs/ProjectState*`, the Encounter map, and the UI card/skill paths remain outside this task.

### Out of scope

- A general hex graph, pathfinding, movement animation, resource deduction, node mutation, enemy garrison, battle-route resolver, save/rollback, or World Partition decision.
- Encounter gameplay, character definitions, card hand, skill panel, card play, character spawning, combat, or Debug panel.
- Direct UI mutation of world state, Actor-pointer transition identity, or a hard-coded level-name switch as a gameplay rule.
- Bulk copying or editing of `E:\2026ue\padma` content.

## Dependencies and integration order

- Required predecessor: TASK-006 is `Done` and ADR-0003 is `Accepted`; this task has been user-approved as `Ready`.
- `TASK-005` is not required; this fixture has no random outcome.
- The default map PIE evidence from TASK-001 is a prerequisite baseline.
- `TASK-008` consumes the transition context and target identifiers; it must not modify TASK-007's source or world asset paths.
- `TASK-009` consumes only the read-only local state exposed by the later Encounter task.
- `TASK-010` owns default-map configuration and end-to-end integration evidence.
- The task owns no shared index, ProjectState, central build file, or default-map configuration.

### Shared conflicts

The demo world map and any binary assets under `Content/Padma/Demo/World/` are exclusive to this task until its independent Review is complete. The UE build/Editor/PIE lane remains serial. No other writer may edit these paths during this task.

## Delegation plan

The Primary Agent remains accountable. A read-only specialist may be requested for one bounded question only:

| Role | Work package | Authority | Expected output |
|---|---|---|---|
| `level-content-planner` | Check that one demo tile and one explicit local-scenario mapping demonstrate the requested selection without defining the final world graph. | Read-only | Fixture review and omitted-rule list. |
| `architect` | Check session context ownership, stable IDs, transition boundary, and dependency direction. | Read-only | Boundary findings or confirmation. |

No second writer is authorized for the map, session context, or world source paths.

## Acceptance criteria

- [ ] The project-owned Demo Sandbox map loads in PIE without modifying generated folders.
- [ ] One demo tile has a stable playtest identity and can be selected, highlighted, and deselected.
- [x] The selected tile exposes its demo scenario and spawn-point context through a typed query/request, not an Actor pointer as identity.
- [x] Confirming the selection produces a transition context that can be consumed after a scene change; missing or invalid fixture data produces a readable failure and no partial transition state.
- [x] The implementation does not charge a resource, resolve a general route, mutate persistent world state, or invent a final node/ID/import policy.
- [x] The world map and source implementation remain in the exact allowed paths, and World/Game module documentation pairs describe the new boundary.
- [ ] An independent Reviewer returns `Pass` or records only explicitly accepted P3 improvements before integration.

## Verification plan

### Automated checks

- Compile the UE Editor target after implementation.
- If the context is testable without a map, run one narrow fixture/context test proving stable IDs and failure-without-mutation; do not create UI automation.
- `pwsh -NoProfile -File .\Scripts\AuditDocs.ps1`.
- `pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict` when shared project structure changes.
- `git diff --check` and exact changed-path audit.

### Manual checks

1. Launch the approved default map in PIE.
2. Observe the Demo Sandbox map and one selectable tile.
3. Click the tile; observe highlight and selection state.
4. Confirm entry; observe a transition request/context containing node, scenario, and spawn-point identities.
5. Verify an invalid/missing fixture reports failure and leaves the prior state unchanged.

The fixed map also exposes a narrow fixture diagnostic path for review: `F10` clears the selected fixture's scenario ID, `Enter` verifies the readable failure and unchanged pending context, `F11` restores the authored fixture, and `F12` consumes the pending context. This is not a general Debug panel or UI automation suite.

The later TASK-010 performs the complete route into the actual Encounter map.

### Required evidence

- Compile output.
- A short PIE capture or screenshots showing the map, tile selection, and transition result.
- Context/failure test output if the narrow test is implemented.
- Changed-path list and independent Review report.

## Learning contract

| Target | Evidence required | Status |
|---|---|---|
| Stable identity versus presentation coordinates | The user predicts which values survive a map rebuild and identifies why coordinates/Actor pointers are not save identity. | `Introduced` |
| Cross-level command/context ownership | The user traces the transition request from tile input to session context and names the owner that may mutate each state. | `Introduced` |

- Learner's hands-on exercise: change the demo tile's display location without changing its stable identity, then predict which transition fields remain equal.
- Transfer question: if two different visual maps represent the same node, which data must remain shared and which data may differ?

## Risks and recovery

- A direct level-load implementation could leak asset names into gameplay rules. Recovery: route through the typed context and keep the level asset as fixture presentation data.
- A map Actor could become the save/source of truth. Recovery: keep stable IDs/context in the session boundary and return the change to Review.
- The task could grow into a world-graph implementation. Recovery: stop at one fixture and create a separate World task.
- Safe rollback is reverting this task's implementation commit and removing only its project-owned Demo World assets; do not delete unrelated Content.

## Completion report

- Final status: `Review`; implementation is ready for independent review. The contract was approved by the user on 2026-09-07.
- Primary Agent and Role: `module_worker` / World Module Agent.
- Changed files: `Source/DreamOfPadma/Public/Demo/Session/`, `Source/DreamOfPadma/Private/Demo/Session/`, `Source/DreamOfPadma/Public/Demo/World/`, `Source/DreamOfPadma/Private/Demo/World/`, `Content/Padma/Demo/World/DemoSandbox.umap`, both PadmaWorld README files, both PadmaGame README files, and this TASK-007 pair.
- Acceptance evidence: the project-owned `DemoSandbox.umap` loads in an unattended runtime smoke and `ADemoSandboxWorld::BeginPlay` emits the TASK-007 initialization prompt; the typed request/context path and GameInstance session boundary are implemented; invalid session replacement and missing fixture fields are covered by the narrow automation test; the user has manually observed the final lit map's selection, cancellation, confirmation, stable-ID status, and invalid-fixture diagnostic flow. A screenshot/video artifact still needs to be attached to the final review package.
- Checks run and results: UE5.8 `DreamOfPadmaEditor Win64 Development` compile passed after the World/Session dependency refactor; the narrow `DreamOfPadma.Demo.TASK007.TransitionContext` automation run passed with `succeeded=1 / failed=0` using temporary `-DDC-ForceMemoryCache -ddc=NoZenLocalFallback` startup flags; `UnrealEditor-Cmd.exe ... /Game/Padma/Demo/World/DemoSandbox -game -nullrhi ... -ExecCmds=quit` loaded the final map, entered Play, and emitted `[TASK-007]` BeginPlay output; `pwsh -NoProfile -File .\Scripts\AuditDocs.ps1` passed with 124 Markdown files and 62 language pairs; `pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict` passed; `git diff --check` passed; exact changed-path audit passed.
- Checks not run and reason: the final visual screenshot/video artifact has not been captured in this environment because native application control is unavailable; the map-load smoke is runtime evidence but does not prove click/highlight/deselect/confirm readability. Independent Reviewer evidence is not yet available. The first wrapper invocation of `RunTests.ps1` hit the machine's non-writable Installed DDC graph before tests started; the direct rerun with temporary in-memory DDC flags passed without changing project configuration.
- Review findings resolved or accepted: the World -> concrete Game/Session dependency was removed; World now exposes typed publisher/consumer callbacks and the GameInstance session subsystem binds them after map load. The initial invalid direct construction of `UGameInstanceSubsystem` remains resolved through map-free `FDemoTransitionContextStore` delegation. Independent implementation Review remains required before integration.
- Remaining risks and open questions: final default-map configuration and Encounter scene remain owned by TASK-010/TASK-008; all Open/Proposed/Deferred world, route, ID namespace, import, save, and World Partition decisions remain unchanged. The final visual artifact and independent Review are pending.
- English/Chinese documentation updated: yes; both module README pairs and the TASK-007 pair were synchronized.
- Changelog draft: date `2026-09-08`; TASK/Goal `TASK-007 SLG Demo World Selection and Transition`; category `Added`; user-visible summary `Added a project-owned Demo Sandbox with one selectable tile and a typed node/scenario/spawn transition context retained within the same GameInstance/session across scene changes`; affected area `Demo World presentation, Demo Session transition handoff, PadmaWorld/PadmaGame module boundaries`; validation evidence `UE5.8 Editor compile after dependency refactor, TASK-007 context automation Success, unattended DemoSandbox map-load smoke, and user PIE observation`; unresolved/deferred notes `Final visual screenshot/video attachment, independent Review, default-map integration, Encounter scene, and all deferred production world rules remain open`.
- Agent-produced learning evidence: `Introduced` for stable identity versus presentation coordinates (typed ID structs and the automation assertions) and cross-level ownership (World selection model -> typed session store -> GameInstance subsystem). No user teach-back is claimed.
- User-produced learning evidence: pending; the user still needs to perform the display-location-only exercise, compare the three unchanged IDs, and answer the transfer question before either target can be `Demonstrated`.
- Integration commit or handoff reference: Local branch `feature/TASK-007-world-selection-transition`, no merge or push; ready for independent Review. The pre-existing `AGENTS.md` pair remains preserved separately in `stash@{0}` and was not included in this TASK.
