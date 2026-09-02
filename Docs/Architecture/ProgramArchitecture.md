# Program Architecture Baseline

- Document ID: ARCH-PROGRAM-001
- Version: 0.2
- Status: MVP architecture baseline frozen; implementation boundaries are staged
- Baseline accepted: 2026-09-02
- Canonical language: English for Agent consumption
- Chinese companion: ProgramArchitecture.zh-CN.md
- Owner: Lead Programmer / Architect Agent
- Related design: ../Design/EN/01_MainPlanner.md through ../Design/EN/05_NumericalPlanner.md

## 1. Purpose and authority

This document defines how Dream of Padma turns accepted design intent into a maintainable Unreal implementation. It owns program structure, runtime ownership, module boundaries, data flow, integration seams, and engineering change control.

It does not replace the design documents. The design documents define what the game means; this document defines how the program represents and executes that meaning. A design or rule change must be made in the appropriate English design or rule document first, then mapped here when it affects implementation.

The high-level MVP design and architecture baseline was accepted on 2026-09-02. This architecture may establish extension seams for future growth, but it must not invent rules or values that remain explicitly open, proposed, deferred, or subject to playtest.

## 2. Current implementation and target architecture

### Current project state

- Unreal Engine association is UE5.8.
- The project currently has one generated runtime module named DreamOfPadma.
- The module directories below are logical boundaries and documentation homes, not a request to create empty UE modules immediately.
- The first implementation should establish boundaries inside the current module and split only after a boundary has stable ownership or an independent build/test need.

### Target logical modules

    PadmaCore
        ^
        |
    PadmaGameplay   PadmaWorld   PadmaUI
          \            |           /
                 PadmaGame

PadmaGame is the composition root. The arrows describe permitted dependency direction: Gameplay, World, and UI depend on Core contracts; Game composes the runtime. The exact Unreal Build.cs split is a later structural decision.

## 3. Architectural principles

1. Commands enter the application/rules layer. UI, GM tools, tests, and future network messages use the same command contracts.
2. A rule service validates a command, changes authoritative state, and emits a result plus explicit events.
3. Presentation consumes state and events. Widgets, animation, Niagara, audio, and camera must not redefine rules.
4. Static definitions are separate from mutable run state. Assets describe possible content; runtime state describes the current run.
5. Stable IDs identify cards, nodes, encounters, story events, outcomes, and save records. Actor and UObject pointers are not save identities.
6. Deterministic random streams are used for synthesis, combat, AI choices, initiative, and any outcome that must be reproduced.
7. Cross-module contracts are small and typed. Do not create a universal Manager or an unbounded global event bus.
8. The composition root coordinates lifetimes; it does not become a rules god-object.
9. Data validation happens before content is allowed into a playable build.
10. An architecture choice is documented before it becomes a dependency that is difficult to reverse.

## 4. Runtime command flow

    Player input / GM command / automation / future network request
                              |
                              v
                         Command contract
                              |
                              v
                   Application service or rule service
                              |
               +--------------+--------------+
               |                             |
               v                             v
        Validation and mutation         Calculation result
               |                             |
               v                             v
          Authoritative state  ----->  Domain event
                                             |
               +-----------------------------+------------------+
               |                             |                  |
               v                             v                  v
             UI / HUD                 AI reaction        Save / replay / telemetry

Every state-changing command must have:

- A stable command name.
- An explicit input structure.
- Validation failures that are visible in debug builds.
- A deterministic result where randomness is involved.
- A list of state changes or events.
- A test path and a manual reproduction path.

Examples include movement, node capture, B-card attachment, synthesis preview, synthesis execution, entering an Encounter, resolving an action-timeline window, ending an Encounter turn, playing a basic non-A battle card, activating an ABC-card ability outside battle, opening the shared card repository, changing its page, entering ACT bullet time, playing a battle card slot, changing control mode, committing/rolling back a battle transaction, and saving.

## 5. Runtime ownership

| Concern | Logical owner | Notes |
|---|---|---|
| Card, calendar, resource, synthesis rules | PadmaCore | Pure contracts and deterministic calculations where possible. |
| Current run state and cross-level session | PadmaGame with Core state types | GameInstance lifetime; no UI-owned truth. |
| Shared sandbox and node state | PadmaWorld | Stable node IDs and mutation events. |
| Current local battle, action timeline, and battle-mode state | PadmaGameplay with PadmaGame orchestration | Encounter action-timeline state and ACT RealTimeAction state are separate local modes; combat rules remain independent from presentation. |
| Ruler policy | PadmaGameplay | Deterministic utility/priority policy for MVP. |
| Input and view models | PadmaUI | Converts input into commands and consumes results/events. |
| Save and load orchestration | PadmaGame; schema types from PadmaCore | Save stable IDs and versioned state, never Actor pointers. |
| Asset authoring and validation | PadmaEditor | Editor-only; no runtime dependency. |
| Automated tests and fixtures | PadmaTests | Tests call the same command and rule paths as gameplay. |

If one system needs data owned by another system, use a typed query, command, result, or event. Do not expose mutable internals solely to avoid writing a contract.

## 6. Logical module responsibilities

| Module | Owns | Does not own | Documentation |
|---|---|---|---|
| PadmaCore | IDs, value types, calendar, resources, card contracts, activation contexts, synthesis calculation, commands, results, events, deterministic streams | UMG, Niagara, concrete Actors, maps, camera, input devices | Modules/PadmaCore/README.md |
| PadmaGame | Composition, session lifetime, GameMode/GameState/PlayerController integration, sandbox/Encounter/RealTimeAction routing, battle commit/rollback, save orchestration | Detailed card rules, combat formulas, widget internals | Modules/PadmaGame/README.md |
| PadmaGameplay | Combat, Encounter action-timeline scheduling, RealTimeAction battle state, abilities, status effects, targeting, encounter orchestration, ruler policy | Map layout, UI mutation, card catalog ownership | Modules/PadmaGameplay/README.md |
| PadmaWorld | Node graph, ownership, movement legality, world mutations, local-battle route boundaries and rollback token, environment seams | Damage formulas, widget state, global session policy | Modules/PadmaWorld/README.md |
| PadmaUI | Input adapters, view models, HUD, card/map interfaces, result presentation | Authoritative state mutation, rule calculation, save truth | Modules/PadmaUI/README.md |
| PadmaEditor | Asset tools, import, validation, previews, content audit, editor-only GM helpers | Runtime game logic and packaged runtime dependencies | Modules/PadmaEditor/README.md |
| PadmaTests | Unit, automation, integration, determinism, migration, smoke fixtures | Production runtime behavior | Modules/PadmaTests/README.md |

The module list is a responsibility map. It does not require the project to have seven C++ modules on day one.

## 7. Data-driven architecture boundary

The design layer describes the meaning of data. DataDrivenArchitecture.md describes asset selection, loading, validation, versioning, and runtime conversion. A module document describes which module consumes each contract.

The minimum path is:

    Design rule and data meaning
             |
             v
    Typed asset / DataTable / CurveTable / GameplayTag
             |
             v
    Asset validation and registry
             |
             v
    Immutable runtime definition
             |
             v
    Rule service and mutable run state
             |
             v
    Result, event, save record, and presentation

No module may hide a balance coefficient, card identity, outcome threshold, or era modifier in gameplay code when it is intended to be content-tuned.

## 8. Phase and gameplay integration

The MVP calendar is explicit:

    One day = Dawn -> Morning -> Dusk
    Seven days = one Era
    Seven Eras = one Chapter

The player-facing loop is:

1. Dawn applies scheduled start-of-day effects, arrivals, card refresh, configured resource gain, and explicitly daily recovery.
2. Morning accepts sandbox movement, placement, B-card attachment or direct play, synthesis, deployment, construction, story choices, and eligible ABC-card active abilities.
3. The ruler policy patrols, detects, reinforces, and counterattacks according to state and configuration.
4. A legal world condition may enter an Encounter at any calendar phase. Encounter is turn-based with a condition-driven action timeline; terrain and card abilities may alter initiative, priority, reactions, interrupts, extra actions, and non-turn windows. Basic non-A cards are playable at most once during each eligible player action turn.
5. A legal world condition may instead enter the ACT RealTimeAction route. In the ACT MVP, clicking `Tab` opens the card repository, blurs the background, reveals five cards, slows world time to 1/10, and rejects movement and attack input. Only basic non-A card input is accepted; the scene preserves inertia, and a numeric slot dispatches a basic non-A card effect. The first MVP must complete one ACT battle loop.
6. Both local battle families run as separate transactions: sandbox time pauses at entry, a complete versioned snapshot of all battle-mutated run state is recorded, success commits the result, and defeat or exit restores that snapshot exactly before returning a typed result and events to the sandbox. Shared effects, damage, statuses, and outcomes use the same authoritative services where their rule meaning is shared.
7. Dusk resolves delayed effects, pile transitions, threat and entropy changes, and end-of-day checks.
8. If no outcome is resolved, the run saves at a safe boundary and enters the next day.

Exact action-bar precedence and numerical timing values, the joint engagement-route resolution data, and whether ABC active abilities are also legal during a local battle remain design review items. It is already decided that a resolved engagement route can trigger/select Encounter, ACT, or a future RealTimeAction presentation and can affect story progression. ABC active abilities are Sandbox-available by default, subject to per-ability restrictions.

## 9. Outcome resolution

The MVP has two confirmed primary victories:

- Ruler core life reaches zero.
- War balance reaches the player victory boundary.

The architecture uses one generic outcome resolver rather than separate hard-coded end-game branches. It reads enabled outcome definitions, evaluates a typed condition against authoritative state, applies priority for simultaneous conditions, emits an OutcomeResolved event, locks the run outcome, and persists the outcome ID.

The same contract can represent defeat conditions without adding a third victory. Faith collapse and themed victory grades, along with other documented victory/defeat methods, remain future-compatible data until explicitly enabled.

Outcome rules are described by the data-driven architecture and consumed by PadmaGame/PadmaCore. The resolver must not be owned by a widget, map Actor, or individual card.

## 10. Persistence, replay, and future network authority

- Save schema types are defined independently from Actors and levels.
- A save stores schema version, build information, seed/stream position, calendar, global resources, stable world state, card state, encounter state, story flags, event history or an equivalent history reference, and the resolved outcome when the run is finished.
- Safe-boundary saves and a pre-battle rollback snapshot are the first target. Mid-animation and mid-battle saves require a separate stable snapshot contract.
- A future server owns authoritative state changes. Clients submit commands and receive results/events.
- The same deterministic rule service should be callable from local play, GM commands, automation, replay verification, and future server code.
- Network replication is not an MVP feature, but public contracts must not assume that every caller is local.

## 11. Debugging and validation

Development and test builds must expose:

- Command ID, source, and failure reason.
- Data definition IDs and versions used by a calculation.
- Random seed, stream name, and roll identifiers.
- Before/after state for resource, entropy, faith, threat, war balance, and core values.
- Synthesis candidate scores, Softmax probabilities, success probability, and Sigma/consequence result; synthesis failure must show no reward.
- AI decision candidates, scores, selected action, and reason.
- Outcome condition checks and the winning condition ID.

Validation layers:

1. Static documentation and schema review.
2. Editor-time asset validation.
3. Unit tests for pure calculations and boundary values.
4. Automation tests through the command path.
5. PIE smoke test.
6. Development package smoke test when the production pipeline is affected.

## 12. Design pattern usage

Use patterns only where they clarify ownership:

- Command for player, GM, replay, and future network requests.
- Strategy for damage, synthesis scoring, AI policy, and target selection.
- State for calendar phase, Encounter action timeline, and battle/control mode.
- Observer/event subscribers for UI, story, audio, and quest reactions.
- Factory/registry for data-defined presentation creation.
- Adapter for sandbox, Encounter, RealTimeAction, mouse, keyboard, controller, ACT, and FPS input.
- Facade for composition at the Game boundary.
- Composite for data-defined effect sequences.

Patterns do not authorize singletons, service locators, or unrestricted global access.

## 13. Architecture change control

An ADR is required for:

- A new logical or UE module.
- A dependency-direction change.
- A public data schema or stable ID change.
- A save schema change.
- A network-authority change.
- A change from hand-authored data to an import pipeline.
- A change that makes a future feature a runtime dependency.

The Architect Agent owns the baseline. Module Agents propose local changes through their module documents. The Review Agent checks dependency direction and contract compatibility. The design owner approves changes that alter rule meaning.

## 14. Implementation sequence

1. Keep the current single UE module and create folder-level boundaries.
2. Implement Core value types, IDs, commands, results, events, and deterministic test fixtures.
3. Add a minimal data catalog and validation path.
4. Implement one sandbox graph and world mutation path.
5. Implement one synthesis preview/execution path.
6. Implement one encounter, one ruler response, and one outcome resolver.
7. Add save/load and pre-battle autosave.
8. Add UI/view-model presentation and debug traces.
9. Split UE modules only when the boundary is proven by ownership, build, or testing needs.

## 15. Architecture acceptance

This baseline is accepted for implementation when:

- Every MVP responsibility has one owning module.
- No dependency points from Core to presentation or concrete world Actors.
- Static definitions, run state, events, and save data are distinguishable.
- Synthesis and outcome resolution can run without a widget.
- The same command path can be called by input, GM commands, and tests.
- Data IDs and versioning rules are defined before assets grow.
- Cross-module changes have an ADR and a review owner.
- Module README files state their current status and boundaries.
