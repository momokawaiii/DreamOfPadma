# Program Architecture

- Document ID: ARCH-PROGRAM-001
- Version: 0.4
- Status: Accepted boundaries; current implementation and offline Demo target distinguished
- Chinese companion: [ProgramArchitecture.zh-CN.md](ProgramArchitecture.zh-CN.md)
- Owner: Lead Programmer / Architect
- Decision source: [ADR-0010](../Decisions/ADR-0010-Offline-Demo-Content-and-Map.md)

## Authority and reading map

The next delivery is the offline Chapter Zero Demo defined in [ChapterZero](../Rules/ChapterZero.md), not completion of every full-MVP backlog item. Rules own game meaning; this document owns dependency and runtime responsibility. Older accepted rules remain unless a newer user decision explicitly changes them.

| Question | Single technical owner |
|---|---|
| Where code lives and what exists | [ModuleMap](ModuleMap.md), [ProjectState](../ProjectState.md) |
| Definition fields, tables, assets and validation | [DataDrivenArchitecture](DataDrivenArchitecture.md) |
| Startup, story, tutorial and battle transitions | [RuntimeFlow](RuntimeFlow.md) |
| Frozen maps, run state, restore and compatibility | [SaveSchema](SaveSchema.md) |
| Map authoring and future bake pipeline | [WorldMapAuthoring](../Content/WorldMapAuthoring.md) |
| Demo work order and release evidence | [Roadmap](../Production/Roadmap.md), [BuildMatrix](../Production/BuildMatrix.md) |
| Scope changes and past decisions | [ADR-0010](../Decisions/ADR-0010-Offline-Demo-Content-and-Map.md), individual TASKs |

These documents distinguish **Current** (code exists), **Target** (accepted, implementation pending) and **Deferred/Open**. Target field names are conceptual until a schema implementation and migration are reviewed.

## Dependency and ownership

The UE5.8 project has one runtime module, `DreamOfPadma`. Core, Game, Gameplay, World, UI and Presentation are implemented folder boundaries, not separate build modules. Do not create empty modules for documentation symmetry.

`Game` composes the application. Core owns value contracts and rule settlement; Gameplay, World and UI consume those contracts through adapters. The current single-module build does not mechanically enforce all logical dependencies.

| Owner | Responsibility | Boundary |
|---|---|---|
| Core | Stable IDs, immutable value definitions, mutable rule state, calendar/resources/cards/synthesis, graph legality and deterministic calculations | No UMG, Niagara, PCG execution, concrete Actor, map loading or UGameplayStatics |
| Game | Definition loading, run/session lifetime, mode travel, transaction/save orchestration; future chapter orchestration | Compose services; no universal rules Manager |
| Gameplay | Separate Encounter and ACT GAS configuration, validation, execution, clocks and local battle state | No forced shared character collection, ability row or live GAS state |
| World | Terrain, roads, node/model projections, visual themes and decorative PCG | Does not decide traversability, capture, story branches or rewards |
| UI / Presentation | Input-to-command adapters, read-only views, CommonUI activation, material/motion/camera/audio playback | Widgets and LevelSequence do not own settlement |
| Editor / Tests | Authoring, validation, preview; tests through production contracts | Editor graph/tool objects must not become Shipping dependencies |

Per-module responsibilities and paths are indexed in [ModuleMap](ModuleMap.md). A structural dependency, public schema, persistence or import change requires an ADR and the focused review selected by [Workflow](../Agent/Workflow.md).

## Command and state contract

`Input → typed command → authoritative validation/settlement → result/events → presentation`

- Definition ID, owned card instance ID and battle entity ID are different identities. Display names, coordinates and asset paths are not save identities.
- Commands name their inputs, failures and affected state. Rejected commands cannot partially pay costs or mutate state.
- Static definitions, frozen generated map results and mutable run state have separate lifetimes. Never edit a DataAsset to represent current HP, ownership or tutorial progress.
- Random outcomes declare their seed/stream/position. Previews never consume live randomness. Cosmetic generation cannot affect gameplay outcomes.
- Save/restore operates on values at safe boundaries. A battle snapshot covers every battle-mutated run field; success commits once, failure/exit restores completely.
- Future network compatibility can reuse command/value boundaries. Networking, server implementation and replay playback are not Demo deliverables.

## Mode and presentation constraints

[ADR-0004](../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md) owns Encounter/ACT separation. Turn-based map units supply site/context; ACT uses its independently selected character/weapon roster. Basic non-A skill identity/slots can be shared while effects, costs and clocks remain mode-owned. Restrict the named trait/weapon/ability rather than silently excluding a whole character.

[ADR-0008](../Decisions/ADR-0008-Activatable-Presentation-Layers.md) owns page/overlay activation and input. The current native UI uses CommonUI with substantial Slate composition; it is not a finished designer-authored UMG skin. Clicking pins map details; hover is visual feedback. Inspection is read-only. Presentation completion is not proof that a rule action succeeded.

The current painted tutorial and the separate continuous-terrain/PCG path are prototypes. The target uses fixed anchors/edges and editor-baked algorithmic terrain plus cosmetic PCG; renderer replacement and approved visual assets remain work. [ADR-0010](../Decisions/ADR-0010-Offline-Demo-Content-and-Map.md) records precedence without relabelling existing saves or claiming that baking already works.

## Completion boundary

PIE and targeted automation establish only their tested scope. A releasable offline Demo additionally needs the real golden path, approved production values, presentation acceptance, stable recovery and both packaged configurations in [BuildMatrix](../Production/BuildMatrix.md). Existing temporary values and art are development evidence, not final-content approval. The Slate story editor and its graph compiler are deferred; normal typed assets/tables may feed the required runtime meanwhile.
