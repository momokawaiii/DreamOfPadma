# PadmaCore Module Program Document

- Document ID: ARCH-MODULE-CORE-001
- Version: 0.3
- Status: Implemented Core folder boundary; no standalone UE module
- Canonical language: English for Agent consumption
- Chinese companion: README.zh-CN.md
- Owner: Core Module Agent
- Current implementation: Source/DreamOfPadma/{Public,Private}/Core; exact write authority per TASK
- Parent architecture: ../../ProgramArchitecture.md and ../../DataDrivenArchitecture.md

## TASK-046 current native implementation

Core/Content now defines the checked table-value snapshot; Core/Run owns the accepted HTML calendar, resources, ABC lifecycle, seeded synthesis, graph commands, battle rollback/commit and save validation. Neither contains scene/UI/GAS objects. Eight rule tests and two catalog tests cover the native slice. [Native playable guide](../../../Content/NativePlayableDemo.md).

## 1. Purpose

PadmaCore contains the rules-facing values, stable identities, data contracts, deterministic calculations, commands, results, and events that must remain independent from presentation and concrete world Actors.

It is the intended home for the calendar, resources, card contracts, synthesis calculation, deterministic random streams, and save-schema value types.

## 2. Responsibilities

- Stable IDs for cards, nodes, encounters, story events, outcomes, and save records.
- Calendar state and phase transition contracts.
- Flow, computation, entropy, faith, threat, war-balance, core, and stability value types.
- Card definition, card runtime-state, and card-activation-context contracts.
- Synthesis input, candidate, preview, success/failure, and non-reward Sigma/consequence result types.
- Battle-mode, Encounter action-timeline, initiative/priority/window, ACT RealTimeAction-input, card-repository, page, and card-slot value types. The ACT MVP contract is `Tab` repository open, background blur, 1/10 world time, movement/attack input rejection, and five visible slots per page; total page count remains deferred.
- Player, GM, automation, and future network command contracts.
- Domain results and explicit state-change events.
- Deterministic random stream interfaces and calculation traces.
- Save-schema value types that do not contain Actor pointers.

## 3. Non-responsibilities

PadmaCore must not own:

- UMG widgets, Niagara systems, cameras, animation, or audio playback.
- Concrete map or combat Actors.
- Level layout, World Partition, PCG, weather, water, or foliage presentation.
- Input-device details.
- Detailed AI presentation or UI state.
- Asset path assumptions as gameplay identity.

The Core boundary may use the minimum Unreal support required by the staged project, but it must not acquire presentation or world dependencies.

## 4. Public contracts

The proposed contracts include:

- CalendarState and phase transition events.
- ResourceState and resource-change results.
- CardDefinition, CardRuntimeState, and card-pile operations.
- BattleMode, BattleActionTimelineState, CardActivationContext, CardRepositoryPage, and card-play request/result contracts.
- SynthesisInput, SynthesisPreview, SynthesisResult, and calculation trace.
- Command, validation error, result, and event envelopes.
- WorldState and save-schema value objects.
- OutcomeConditionDefinition and RunOutcomeState.

Names are provisional until the implementation task creates the real C++ types. The semantics must follow the English design/rule documents.

## 5. Data and runtime ownership

PadmaCore consumes typed card, calendar, resource, synthesis, and outcome definitions. It owns calculations and contracts, not mutable UObject assets. Mutable run state is owned by the runtime composition and passed through explicit services.

Definitions and state must be separable so a new card value or synthesis curve does not require a code change.

## 6. Dependencies and integration

Logical dependencies: none on PadmaGameplay, PadmaWorld, PadmaUI, or PadmaEditor.

Integration uses:

- Commands from UI, GM tools, tests, and future network callers.
- Results returned to the caller.
- Events consumed by Gameplay, World, UI, Save, and telemetry.
- Stable IDs and typed queries instead of concrete Actor references.

The current DreamOfPadma Build.cs is a staging implementation. A future standalone PadmaCore module requires an ADR and an independent build/test reason.

## 7. Tests and debug evidence

Required tests:

- Calendar boundary transitions.
- Resource accounting and explicitly authored paid/risky conversion loops.
- Stable ID and serialization fixtures.
- Candidate filtering, Softmax, success/failure, and no-reward Sigma/consequence results.
- Basic non-A card rejection outside battle, outside the player's eligible Encounter action turn, and after the one-card-per-turn limit.
- ABC-card active-ability activation from the sandbox without entering a battle.
- ACT RealTimeAction `Tab` repository transition, background blur, 1/10 world-time slowdown, movement/attack input rejection, card-only bullet-time input, inertia, five-visible-slot paging, and slot request validation. Do not assume total page count.
- Same seed produces the same result.
- Preview does not mutate state or advance the live random stream.
- Invalid command and missing-data failures are readable.

Debug output should include input IDs, data versions, seed/stream position, calculation fields, state changes, and emitted events.

## 8. Implementation stages

1. Extract value types and contracts inside Source/DreamOfPadma/.
2. Add pure calculation tests before adding Actors.
3. Add data-definition loading through the agreed data boundary.
4. Move shared contracts into a standalone UE module only after the boundary is proven.

## 9. Learning targets and risks

Learning targets: C++ value design, interfaces, deterministic simulation, serialization, data contracts, unit testing, and dependency control.

Main risk: allowing convenience access to Engine, Actors, or global state to turn Core into an untestable gameplay manager.

## 10. Full-MVP contract allocation

TASK-012 owns mode-neutral IDs, state participants and typed command/result envelopes; TASK-015 owns only calculations whose rule meaning is explicitly shared. TASK-020 owns the turn-based/ABC card system and shared basic-skill identity, slots and lifecycle providers. TASK-035 owns the separate ACT character-card collection and roster values. Definition ID, owned-card instance ID and battle entity ID are separate domains.

Core contains neither an ASC nor an execution schema shared by force between Encounter and ACT. Logical turn/window clocks and ACT real-time clocks are explicit inputs to their own mode services. A shared basic-skill slot chooses the active mode's effect definition; it does not merge character collections, cooldowns or live abilities. See [ADR-0004](../../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md).

State owners implement export/restore participation when they own battle-mutated run state. D20 classifies ACT presets versus run-owned roster/loadout before save fields are selected. Source-PDF fields with missing values or ambiguous scope are recorded as unresolved, not zero-valued runtime defaults.

## Neutral relationship contribution boundary

TASK-012 freezes a read-only effect-source/modifier value contract before resource/card/synthesis/combat consumers implement it. TASK-039 provides relationship source state; 019/020/022/015 consume only their D28-selected target domains, with one scalar/effect application owner. TASK-034 injects real providers. The contract and Core stay independent of GAS, Actors, maps and UI; unknown/duplicate/version-invalid contributions fail explicitly. Relationship state, numeric execution and the bonus overview are distinct responsibilities.

## TASK-040 handoff

TASK-040 implements FPadmaCardMobilityDefinition in Core/Cards without Actor/UI/model dependencies. Its configured flag prevents default movement approval; condition IDs remain data. TASK-012/020/024 own later rule commands and evaluator registration.

## Chapter Zero target boundary

[DataDrivenArchitecture](../../DataDrivenArchitecture.md), [RuntimeFlow](../../RuntimeFlow.md) and [SaveSchema](../../SaveSchema.md) own the new chapter/story/tutorial contracts under ADR-0010. Core owns value-only conditions/effects, stage assignments, checkpoint history and tutorial reward identity; Game owns loading/orchestration. These additions are not current APIs.

Keep frozen generation results separate from mutable run state. No PCG, Sequence, UEdGraph or Widget objects enter Core. Tests must cover stage/reload determinism, same-run reward idempotency versus new replay, invalid restore and complete battle rollback. Exact registry encoding and durable transaction implementation remain open. Existing full-MVP allocations below/above are later work, not the offline Demo checklist.
