# PadmaGame Module Program Document

- Document ID: ARCH-MODULE-GAME-001
- Version: 0.5
- Status: Current logical composition boundary in the shared runtime module; legacy Demo chain retired; no standalone UE module
- Canonical language: English for Agent consumption
- Chinese companion: README.zh-CN.md
- Owner: Game Composition Module Agent
- Current implementation: Source/DreamOfPadma/{Public,Private}/Game; Core owns rules, Game composes the current map and separate battle modes
- Parent architecture: ../../ProgramArchitecture.md and ../../DataDrivenArchitecture.md

## TASK-046/048 current native implementation

Game/Content resolves soft catalog shards; Game/Run retains value state in GameInstance and writes safe SaveGame envelopes declared in Game/Save/PadmaRunSaveGame.h. Game/Framework separates PadmaGameMode.h/.cpp from PadmaPlayerController.h and its composition/UI/combat implementation files; the controller composes map/camera/CommonUI/battle, translates UI intent to commands, travels to the battle map and projects one result. It handles missing-map/start/travel failure and tears down callbacks. TASK-048 retires the TASK-007/008 Demo maps, definitions and travel chain. Game/WorldMap owns map assets, checkpoint-table import and editor handles; Core/WorldMap owns value generation/validation. Game/Run freezes the chosen map for save and rollback. See [map authoring](../../../Content/WorldMapAuthoring.md). [Native playable guide](../../../Content/NativePlayableDemo.md).

## 1. Purpose

PadmaGame is the runtime composition root. It connects Core contracts to Gameplay, World, UI, persistence, and control-mode presentation without owning their detailed rules.

Historical TASK-007 used UDemoTransitionSessionSubsystem for typed context handoff. That implementation is retired; current Game/Run and Game/Framework coordinate the native run/battle lifecycle. The TASK report retains its original evidence.

## 2. Responsibilities

- GameMode, GameState, PlayerController, GameInstance, and session lifecycle integration.
- Composition and initialization of logical services.
- Routing commands to the correct application or rule service.
- Coordinating calendar progression and safe phase boundaries.
- Coordinating save/load, pre-battle autosave, and battle commit/rollback transactions.
- Loading the active scenario or MVP rule profile.
- Owning the generic outcome resolver orchestration.
- Routing strategic sandbox, condition-driven turn-based Encounter, and RealTimeAction mode requests, including the complete MVP ACT route and future FPS/other variants.
- Coordinating engagement-route constraints supplied by both sides, terrain, and card/story effects without owning their detailed rules; the resolved route can select/trigger Encounter, ACT, or a future RealTimeAction presentation and story consequence.
- Exposing runtime status to UI through read-only views or events.
- Retaining run identity and the frozen map across save/load and battle transactions without serializing scene Actors or widgets.

## 3. Non-responsibilities

PadmaGame must not own:

- Card formulas, combat damage formulas, or AI scoring details.
- Widget internals or direct input-device interpretation.
- Node layout or world mutation rules.
- A universal mutable Manager containing every system.
- World/Map selection/highlight geometry or Core world-rule mutations.

It may coordinate a service, but the service owns its own state and contract.

## 4. Public contracts

- Session start, continue, and finish commands.
- Active profile and definition loading results.
- Phase transition and safe-boundary notifications.
- Save request, save result, load result, and migration failure.
- OutcomeResolved and run-lock notifications.
- Control-mode and engagement-route transition request and result.
- Complete versioned pre-battle snapshot, battle commit, and exact battle rollback notifications.
- Read-only state snapshots for presentation.
- FPadmaMapLayout, run command results and complete battle snapshots through the current run subsystem.

The Game layer must pass typed commands and results rather than expose subsystem internals.

## 5. Data and runtime ownership

PadmaGame owns session-level state such as active run identity, lifecycle, save orchestration, and final outcome lock. It consumes Core, World, and Gameplay state views; it does not duplicate their mutable values.

The outcome table and scenario profile are data inputs. The resolver applies them against authoritative state and records the resulting OutcomeId.

Historical TASK-007 context-store validation is preserved in its task evidence; the retired store is not a current API. Current Core/Run validates map/run snapshots before replacement.

## 6. Dependencies and integration

As the composition root, PadmaGame may depend on the public contracts of PadmaCore, PadmaGameplay, PadmaWorld, and PadmaUI. Those modules must not depend on PadmaGame internals.

Future online authority should use this layer to distinguish local session coordination from server-owned state.

## 7. Tests and debug evidence

Required tests:

- Project startup and subsystem initialization.
- New-run and continue-run lifecycle.
- Phase/day/era/chapter boundary routing.
- Save/load round trip, complete pre-battle autosave, exact restore on defeat/exit, and commit on success.
- Outcome priority and run locking.
- Sandbox-to-battle route, including an ABC-card character moving onto an enemy-occupied point, joint engagement-route constraints, mode/story trigger resolution, and control-mode request routing without changing rule state.
- Encounter action-timeline lifecycle and complete ACT RealTimeAction battle-loop routing.
- PIE smoke test and clean shutdown.
- Current WorldMap generation, checkpoint, save-order and rollback tests; playable-map startup and return smoke.

Debug output should show service initialization order, active profile IDs, phase transitions, save boundaries, outcome checks, and command routing failures.

## 8. Implementation stages

1. Keep the current Core/Game/Gameplay/World composition in the shared DreamOfPadma module; the TASK-007 handoff is retired.
2. Add explicit service ownership and state views.
3. Add sandbox-to-Encounter routing when an ABC-card character reaches an enemy-occupied point, action-timeline orchestration, and complete battle transaction boundaries.
4. Add save and outcome orchestration after Core and World contracts exist.
5. Add complete ACT RealTimeAction route orchestration after the Encounter path is stable.
6. Split the composition root into PadmaGame only when build dependencies justify it.

## 9. Learning targets and risks

Learning targets: Unreal Gameplay Framework, subsystem lifetimes, composition roots, typed session state, save orchestration, mode routing, and integration testing. TASK-007 is historical lifecycle evidence, not a current entry point.

Main risk: using GameMode or GameInstance as a universal rules container instead of a coordinator.

## 10. Separate mode composition and roster handoff

TASK-027 resolves a battle route from world/context constraints and calls the appropriate mode adapter. Encounter receives its turn-based deployment; ACT receives TASK-035's independently configured battle-settings roster and the entry terrain/context. Additional linkage to the triggering map unit is D20, not an implicit character-card conversion.

TASK-028/029 coordinate complete battle transactions and safe-boundary persistence through TASK-012 participants. TASK-035 registers all approved run-owned roster/weapon/inventory fields that battle can mutate; global presets have a separately decided lifetime. Game rebuilds mode-owned runtime objects from stable definitions and state instead of saving GAS handles.

TASK-034 binds TASK-020's production card/payment/lifecycle providers into Encounter and ACT, replaces approved test fixtures, and validates full runs. An individual death/interrupt is handled by the mode; only an approved local result closes the battle transaction. No new universal rules manager or standalone UE module is implied.


## Historical TASK-008 fixed Demo composition — retired

The former Demo/Integration travel subsystem queued a fixed Encounter level with receipt and actor-ready checks. TASK-048 removed this route and its production dependencies. Current travel, commit and rollback are composed by Game/Framework and Game/Run.

Historical opt-in Demo smoke/context/summon evidence remains in TASK-008; its tests and debug inputs are retired. Current tests are routed from the [test plan](../../../Production/CurrentBuildTestPlan.md).

## TASK-040 handoff

TASK-040 owns Game/Presentation adapters from ACT/general model definitions into neutral loaded presentation views. A qualified definition key selects exactly one source. APadmaNodePreview composes static scene slots and preview bindings with no session, roster, movement/payment or save writes. TASK-048 migrates the garrison preview to a generic model; current runtime entry is documented in NativePlayableDemo.

## Chapter Zero target boundary

Game will compose ChapterDefinition, matching baked map metadata, story/tutorial execution, menu/opening/ending and safe restore under [ADR-0010](../../../Decisions/ADR-0010-Offline-Demo-Content-and-Map.md). See [RuntimeFlow](../../RuntimeFlow.md) and [SaveSchema](../../SaveSchema.md) for lifecycle and persistence; do not duplicate those schemas here.

Validate a save against independently installed map/content definitions before replacing the active run. Rebuild presentation and mode GAS from stable state. Tutorial UI/Sequence sends intents; Core/Game services settle progress and rewards. Replay creates a new tutorial identity; continue restores the existing one. These chapter features, MapKey bake lookup and reward persistence are pending, not delivered by current Game/Run.

Acceptance needs new/continue/skip/replay, map mismatch, mode travel failure and clean shutdown in both packages. The story editor is deferred. Preserve current Core/Game ownership and unrelated full-MVP task allocations; no universal chapter Manager or runtime editor dependency.
