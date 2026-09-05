# Future ACT Development Agent Contract

- Document ID: ARCH-GAMEPLAY-ACT-001
- Version: 0.1
- Status: Accepted architecture contract; implementation deferred
- Canonical language: English for Agent consumption
- Chinese companion: ACTDevelopmentContract.zh-CN.md
- Owner: Gameplay Module Agent
- Decision record: ../../../Decisions/ADR-0002-Encounter-First-ACT-Preparation.md
- Task contract: ../../../Production/Tasks/TASK-004-ACT-Architecture-Decision.md
- Parent documents: README.md, ../../DataDrivenArchitecture.md, ../../ProgramArchitecture.md

## 1. Purpose and authority

This document is the implementation boundary for future ACT RealTimeAction work. It records the architecture agreements reached before implementation so that a later Agent does not reconstruct them from chat history or copy the external Combat prototype without review.

This document does **not** authorize ACT implementation now. The current delivery order is Encounter first. ACT work may begin only when an approved task marks a bounded ACT slice `Ready` and its Encounter prerequisites are satisfied.

Design and rule documents remain authoritative for gameplay meaning. When a rule is marked `Open`, `Proposed`, `To confirm`, or deferred, an Agent must preserve that state and stop before an implementation choice would invent the answer.

## 2. Required reading and task gate

Before a future ACT task starts, the Primary Agent must read:

1. `AGENTS.md`, `Docs/00_INDEX.md`, and `Docs/ProjectState.md`.
2. The concrete approved `TASK-xxx` contract.
3. `Docs/Rules/Combat.md` and the relevant accepted design baseline.
4. `Docs/Architecture/ProgramArchitecture.md`, `DataDrivenArchitecture.md`, and `RuntimeFlow.md`.
5. This document, the PadmaGameplay module README, and ADR-0002.
6. Nested `AGENTS.md` files for every writable directory.

The task must declare Goal, exact allowed paths, out-of-scope work, dependencies, acceptance criteria, automated and manual tests, required evidence, and no more than two learning targets. One writable Primary Agent owns the task. Discovery and review remain read-only unless separately authorized.

## 3. Current repository reality

- `Source/DreamOfPadma/` is still the shared runtime module; `PadmaGameplay` is a logical boundary, not a standalone Unreal module.
- GAS is not currently a project dependency, and no ACT C++ framework or ACT content assets are implemented.
- Enhanced Input is available, but an ACT input schema and per-character bindings are not yet approved.
- `GASCompanion` and `AuroraDevs_UGC` belong to the external reference prototype. They are not Padma dependencies.
- A Launcher-installed Unreal Engine is sufficient for project gameplay code. An engine source build is required only if a separately approved task changes or deeply debugs engine source.

A future task must verify these facts again because repository and engine state can change.

## 4. Accepted architectural direction

Future ACT work is C++-first and data-driven:

- C++ owns authoritative rules, activation validation, state transitions, targeting, hit resolution, damage application, and cleanup.
- A C++ Gameplay Ability base owns the common ability lifecycle.
- C++ Ability Tasks own reusable asynchronous timing and sequence interpretation.
- An `AbilityDefinition` Primary Data Asset owns the coherent immutable definition of one ability.
- An `ACTSequence` Primary Data Asset owns sequence nodes, transitions, phase actions, and animation timing references.
- Shared typed C++ row structures back per-character DataTables.
- A data-only Ability Blueprint may select a native class and provide asset-facing defaults, but it must not reimplement the common lifecycle or authoritative rules.
- Gameplay Effects represent costs, cooldowns, attributes, tags, statuses, and damage changes where GAS is the approved execution path.
- Gameplay Cues carry cosmetic audiovisual presentation and cannot be the authoritative source of damage or state.

This is a target boundary, not permission to create all abstractions before one vertical slice proves them.

## 5. Encounter-first delivery boundary

The active implementation priority remains:

1. Pure damage/result services and fixtures.
2. Encounter turn state and one playable placeholder encounter.
3. Ruler behavior needed by that encounter.
4. The minimum status subset used by the encounter.
5. ACT only after the Encounter action-timeline path is stable and an ACT task is explicitly approved.

ACT preparation may update planning documents without enabling GAS, changing module dependencies, creating ACT assets, or pre-implementing an ability framework. Reusable Encounter contracts may be shaped so they do not block ACT later, but an Encounter task must not absorb speculative ACT infrastructure.

## 6. Static ACT data model

### 6.1 Character catalog and tables

Each playable ACT character may own a `CharacterACTCatalog` Primary Data Asset that references a character-specific set of tables:

- Ability Table.
- Projectile Table.
- Summon Table.
- Scope Table, after the meaning of `Scope` is confirmed.
- Ammo Table.

All character-specific table instances use common typed C++ row structures. DataTables do not form an inheritance hierarchy; reuse comes from shared row types, stable IDs, shared definitions, and explicit overrides.

The Ability Table is a roster and assembly index, not a giant skill document. A conceptual row may contain:

- Stable `AbilityId`.
- Soft reference to `AbilityDefinition`.
- input or gameplay-event binding ID.
- grant policy and initial level.
- enabled state.
- required and blocked character tags.
- narrowly defined character-specific overrides.

Projectile, Summon, Scope, and Ammo tables are indexed by stable IDs. An ability refers to those logical IDs rather than depending on a physical table path. A resolver uses the active character catalog to resolve the row. A direct typed row handle is allowed only when the content is intentionally bound to one particular table.

`Scope` is not yet a valid implementation schema. Before creating that row type, the owner must decide whether it means an area/field entity, a targeting profile, a weapon optic, or separate concepts.

### 6.2 AbilityDefinition

An `AbilityDefinition` is the cohesive source for one ability's immutable authoring data. Its future typed schema may include:

- Stable `AbilityId` and definition version.
- Native Gameplay Ability class or optional data-only Ability Blueprint class.
- activation contexts, required tags, blocked tags, and granted tags.
- cost and cooldown specifications.
- damage/effect specifications and typed evaluator IDs.
- soft reference to an `ACTSequence`.
- targeting, hit, and Motion Warping profile references.
- projectile, summon, field, scope, or ammo logical IDs when applicable.
- Gameplay Cue tags and presentation profile references.
- loading and execution policy selected from known typed policies.

The definition may provide magnitudes or references consumed by Gameplay Effects, but every gameplay scalar must have one declared source of truth. The same cooldown duration, cost, or damage value must not be independently authored in INI, a table row, a Data Asset, and a Blueprint graph.

### 6.3 ACTSequence

An `ACTSequence` describes execution topology rather than mutable execution state. A future schema is expected to separate:

- Nodes: stable node ID/state tag, animation montage soft reference, montage section, phase list, Hit Profile, and Motion Warping Profile.
- Edges: source node, destination node, input tag, required/blocked tags, priority, and buffer policy.
- Phase actions: typed actions such as opening a window, requesting target data, applying/removing a Gameplay Effect, emitting a gameplay event, spawning an approved payload, or sending a Gameplay Cue.

Gameplay Tags identify states, events, permissions, and gates. Tags do not replace explicit graph edges or runtime node state. Montage sections visualize and synchronize a sequence; they are not the sole gameplay state machine.

### 6.4 Static versus runtime ownership

Static definitions may contain base costs, cooldown policy, animation references, graph topology, tags, payload IDs, and presentation references.

Runtime state includes current cooldown, current ammo, current combo node, buffered input, active windows, active targets, active effect handles, spawned payload instances, prediction keys, and interruption state. Runtime state belongs to the ability execution/context and replicated gameplay state, never to a Data Asset, DataTable, or INI file.

## 7. Custom INI and developer settings boundary

A future typed `UDeveloperSettings` class may expose project-level ACT defaults through `DefaultGame.ini` or a dedicated config category. This is appropriate for infrastructure policy and validation behavior, for example:

- default native ability base class or approved class families.
- default input-profile ID and Enhanced Input integration policy.
- default cooldown Gameplay Effect class or cooldown tag root.
- approved ability phase tag roots.
- whether an ability archetype requires a sequence, montage, cooldown, target policy, or input binding.
- validator severity, debug logging, async-loading policy, and development feature gates.

INI must not own:

- a particular ability's montage or montage sections.
- a particular ability's damage, cost, or cooldown duration.
- combo nodes, combo edges, input windows, or cancel priorities.
- Hit Profiles, Motion Warping Profiles, projectiles, summons, fields, VFX, or SFX.
- mutable runtime state.

Enhanced Input Actions and Mapping Contexts remain Unreal assets. Character catalog/table data binds logical ability inputs to a character. INI may select a default profile or validation rule; it must not become a second per-skill binding database.

The configuration precedence is policy defaults, then explicit typed content. A default may fill optional infrastructure only. It must not silently override authored gameplay balance, and validation must report missing required content instead of manufacturing it.

No ACT developer settings class or INI section is authorized by this document alone.

## 8. Ability lifecycle and phase model

The technical model must support a typed sequence of phases, but the final global phase taxonomy is still open. A reasonable candidate vocabulary is:

- `Startup`: anticipation and pre-commit setup.
- `Commit`: cost/cooldown commitment and irreversible activation boundary.
- `Active`: hit, payload, or primary effect window.
- `Recovery`: post-action lock, cancel, and chaining rules.
- `End`: deterministic cleanup and result emission.

Names such as `PreMissile` and `Missile` are projectile-specific and should be optional typed actions or subphases, not mandatory phases for every melee, movement, defensive, or passive ability.

Each phase may apply or remove Gameplay Effects and tags through explicit phase actions. Effect handles and cleanup ownership must be tracked so cancellation, interruption, death, and ability end produce the same deterministic cleanup.

The final phase names, commit point, cancel matrix, interruption priority, and relation to Encounter/Card timing require an approved decision before their shared enum, tags, or serialized schema are frozen.

## 9. Combo and buffered-input boundary

The default target is one activated combo ability with runtime node state. A separate Gameplay Ability should not be activated for every normal combo segment unless a concrete requirement proves that separation is necessary.

The runtime execution context should track at least:

- current and previous node IDs.
- current phase.
- open input/cancel windows.
- buffered input and expiry policy.
- selected edge and transition reason.
- active target data, effect handles, and prediction context.

Combo edges are explicit data. Required/blocked Gameplay Tags gate an edge, while input tags select candidates. Edge priority and buffer policy resolve legal candidates. Tags alone must not encode the entire graph.

Pre-input buffering must be redesigned and tested as a state machine. The external Combat prototype contains at least one broken pre-input route, so its Blueprint graph is evidence for test cases, not an implementation to port.

Before combo implementation, the owner must confirm buffer lifetime, replacement policy, multi-input priority, early/late input behavior, cancel precedence, interruption behavior, and server/authority validation.

## 10. Runtime responsibility flow

The intended future flow is:

    Device input
      -> PadmaUI or player input adapter
      -> typed input command or Gameplay Event
      -> activation policy and Ability System Component
      -> C++ Gameplay Ability base
      -> C++ Ability Task sequence interpreter
      -> target/hit validation
      -> authoritative Gameplay Effects and runtime state
      -> typed gameplay events
      -> Gameplay Cues and presentation consumers

Input code requests an action; it does not apply damage. Animation code announces authored timing markers; it does not decide authoritative hits. Presentation consumes results and cues; it does not define legality.

If networked ACT is later approved, clients may predict reversible input response, montage playback, and cosmetics. Target acceptance, damage, costs, cooldowns, inventory/ammo, and spawned authoritative actors must be validated by the authority. The project's actual networking baseline is still open and must be decided before prediction behavior becomes an acceptance requirement.

## 11. Animation, hit, Motion Warping, and presentation

- Montage references and section mappings belong to `AbilityDefinition` or `ACTSequence`, not INI and not duplicated child Blueprint graphs.
- Animation Notify/Notify State objects may emit narrow timing markers. They must not combine animation timing, hit authority, damage, AI notification, and VFX/SFX orchestration in one Blueprint.
- Hit detection and target-data construction belong to native components or Ability Tasks with explicit authority and prediction rules.
- Damage and status changes use rule services or Gameplay Effects and are authority-owned.
- Motion Warping uses a typed profile plus runtime target data; it must have deterministic fallback when no valid warp target exists.
- Gameplay Cues or presentation events trigger VFX, SFX, camera, and other cosmetic behavior. Cosmetic failure must not change gameplay outcome.
- AI observes typed gameplay events or state changes. It must not depend on a specific animation notify Blueprint.

## 12. GAS, plugins, Blueprints, and tooling

The architecture is compatible with Unreal's built-in Gameplay Ability System, but adding GAS dependencies is a future dependency change that needs an approved task and the repository's required architecture review. Do not enable GAS during Encounter work merely to prepare for ACT.

External GAS Companion or Aurora plugins are neither required nor approved. Adding either one requires a separate ADR covering ownership, source availability, version compatibility, packaging, licensing, upgrade risk, and removal strategy.

Blueprints remain useful for content assembly, data-only child classes, animation graphs, and presentation. Common lifecycle and authoritative gameplay logic belong in C++.

Read-only Blueprint auditing does not require an MCP server. Unreal Python, Asset Registry, and `BlueprintEditorLibrary` can inventory assets and inspect supported editor metadata. MCP may later improve editor automation, but it is a development tool decision, not a runtime dependency or permission to mutate assets.

## 13. Lessons from the external Combat prototype

The prototype is reference evidence only; its assets, plugins, values, and rules are not Padma defaults.

- A thin combo base that only removes a tag or switches a weapon mesh leaves Commit, Montage, branching, event waits, and end logic duplicated in every child. The Padma base/task split must centralize the invariant lifecycle and expose typed extension points.
- One AnimNotifyState that owns timing, hit detection, damage, AI notification, and presentation has too many authorities. Padma separates those responsibilities as described above.
- A configured normal combo path does not prove buffered input is sound. Every transition and cancellation route needs deterministic tests.
- Prototype execution thresholds, ranges, keys, lock-on assumptions, and special-counter triggers are examples only. They must not be copied into Padma data without a Padma rule decision.

## 14. Module and ownership boundaries

| Owner | ACT responsibility |
|---|---|
| PadmaCore | Stable card/ability identity and presentation-neutral card contracts shared across modes |
| PadmaGameplay | Ability rules, sequence execution, target/hit/damage resolution, runtime combat state, and future GAS integration |
| PadmaGame | Mode composition, possession, and transition between Sandbox, Encounter, and RealTimeAction |
| PadmaUI | Device bindings, input adaptation, HUD/repository presentation, prompts, and cosmetic feedback |
| PadmaEditor | Asset validators, migration/import helpers, and authoring diagnostics |
| PadmaTests | Pure rule, data validation, automation, replay/determinism, and future PIE coverage |

Until physical UE modules exist, these are dependency and ownership rules inside `Source/DreamOfPadma/`; they are not permission to create cross-module includes or premature module folders.

## 15. Decisions that remain open

A future Agent must not infer these from the proposed class names:

- The first ACT vertical slice and its exact player-facing ability.
- Ability System Component ownership, avatar/owner lifecycle, and grant/remove policy.
- Offline-only versus networked baseline and prediction acceptance criteria.
- Final Gameplay Tag taxonomy and registration ownership.
- Attribute sets, damage pipeline, Gameplay Effect magnitude ownership, and stacking rules.
- Final ability phase taxonomy, cancel matrix, interruption policy, and combo buffer semantics.
- Exact `CharacterACTCatalog` and row schemas, override policy, and definition versioning.
- Meaning of `Scope` and whether it is one or several data domains.
- Card-to-ability activation rules in Encounter and ACT where current design marks them open.
- Projectile, summon, field, ammo, lock-on, execution, and Motion Warping lifecycles.
- Asset loading, cooking, replication, replay, and save compatibility policy.
- ACT `Tab` duration/toggle behavior, cost/cooldown, page boundaries, and total page count already marked open in the design baseline.

When one of these choices changes public identity, dependencies, save/network shape, import behavior, or more than one module, record or update an ADR before implementation.

## 16. Suggested future task decomposition

These are work packages, not active TASK IDs and not implementation authorization:

1. Confirm the first ACT vertical slice and dependency/network decisions.
2. Add native GAS foundations and one minimal C++ ability lifecycle, if approved.
3. Add typed ACT data contracts, Asset Manager loading, and validation for one character catalog.
4. Implement one sequence interpreter path with cancellation and deterministic cleanup.
5. Add one hit/damage route and one cosmetic cue route.
6. Add one two-node combo with tested buffered input.
7. Add Motion Warping or a payload family only when the vertical slice requires it.
8. Integrate the accepted ACT `Tab` repository/bullet-time contract and complete one battle loop.

Each task should produce one observable result, use the smallest write set, and avoid shared binary assets unless ownership and Editor-session serialization are explicit.

## 17. Verification and evidence requirements

Future implementation is not complete from compilation alone. Relevant tasks must select evidence from:

- C++ unit or automation tests for phase transitions, edge selection, buffering, cancellation, and cleanup.
- data validation tests for duplicate IDs, unresolved soft references, invalid tags, missing required policies, and illegal cross-table references.
- authority tests showing damage/cost/cooldown cannot be created by cosmetic or input code.
- deterministic traces containing AbilityId, node, phase, input/event, selected edge, target result, applied effect, and cleanup reason.
- PIE evidence for montage synchronization, interruption, Motion Warping fallback, cues, and the accepted card repository flow.
- packaging/cook validation for soft-referenced definitions and presentation assets.

Manual PIE observations must be performed and recorded by the user or an authorized Editor operator. An Agent must not claim unobserved manual results. English/Chinese documentation pairs, `Scripts/AuditDocs.ps1`, applicable automated tests, and `Scripts/ValidateProject.ps1 -Strict` remain required by repository policy.

## 18. Stop conditions for an ACT Agent

Stop and request the owning decision when:

- no approved `Ready` task authorizes the ACT slice.
- Encounter prerequisites or the task's required contracts are not stable.
- implementation would choose an item listed as open or deferred.
- a dependency, plugin, module split, save/network contract, or shared schema change lacks review.
- the proposed work moves authority into UI, animation, Gameplay Cue, or a data-only Blueprint.
- two sources would own the same gameplay value.
- another Agent owns an overlapping writable file or binary asset.

An ACT Agent must never silently copy the external Combat prototype, edit engine/plugin source, mutate another module's state directly, or broaden an Encounter task into speculative ACT implementation.
