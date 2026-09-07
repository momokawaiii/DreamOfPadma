# PadmaGame Module Program Document

- Document ID: ARCH-MODULE-GAME-001
- Version: 0.2
- Status: Planned logical boundary; TASK-007 session handoff is implemented in the staging module; no standalone UE module yet
- Canonical language: English for Agent consumption
- Chinese companion: README.zh-CN.md
- Owner: Game Composition Module Agent
- Current implementation: Source/DreamOfPadma/ is the temporary shared runtime module; TASK-007 adds only the Demo session boundary and does not change default-map configuration
- Parent architecture: ../../ProgramArchitecture.md and ../../DataDrivenArchitecture.md

## 1. Purpose

PadmaGame is the runtime composition root. It connects Core contracts to Gameplay, World, UI, persistence, and control-mode presentation without owning their detailed rules.

For TASK-007, the composition boundary is the GameInstance-lifetime `UDemoTransitionSessionSubsystem`. After a map loads, it binds the World presentation's typed transition publisher/consumer callbacks, validates and stores `FDemoTransitionContext` across a future world change, and exposes publish/peek/consume operations to the next scene. The World presentation does not include this concrete subsystem.

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
- Owning the session lifetime of the fixed Demo transition context without serializing a map Actor, level coordinate, or widget state.

## 3. Non-responsibilities

PadmaGame must not own:

- Card formulas, combat damage formulas, or AI scoring details.
- Widget internals or direct input-device interpretation.
- Node layout or world mutation rules.
- A universal mutable Manager containing every system.
- The Demo tile's selection/highlight presentation or the World fixture's graph rules.

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
- `FDemoTransitionContext` handoff through `UDemoTransitionSessionSubsystem::PublishTransition`, `PeekTransition`, and `ConsumeTransition`.

The Game layer must pass typed commands and results rather than expose subsystem internals.

## 5. Data and runtime ownership

PadmaGame owns session-level state such as active run identity, lifecycle, save orchestration, and final outcome lock. It consumes Core, World, and Gameplay state views; it does not duplicate their mutable values.

The outcome table and scenario profile are data inputs. The resolver applies them against authoritative state and records the resulting OutcomeId.

The TASK-007 `FDemoTransitionContextStore` validates the node, scenario, and spawn-point IDs before replacing pending state. An invalid replacement reports a readable failure and preserves the previous context.

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
- TASK-007 context automation test and DemoSandbox map-load smoke.

Debug output should show service initialization order, active profile IDs, phase transitions, save boundaries, outcome checks, and command routing failures.

## 8. Implementation stages

1. Keep composition in the generated DreamOfPadma module and provide the TASK-007 session-lifetime Demo handoff.
2. Add explicit service ownership and state views.
3. Add sandbox-to-Encounter routing when an ABC-card character reaches an enemy-occupied point, action-timeline orchestration, and complete battle transaction boundaries.
4. Add save and outcome orchestration after Core and World contracts exist.
5. Add complete ACT RealTimeAction route orchestration after the Encounter path is stable.
6. Split the composition root into PadmaGame only when build dependencies justify it.

## 9. Learning targets and risks

Learning targets: Unreal Gameplay Framework, subsystem lifetimes, composition roots, typed session state, save orchestration, mode routing, and integration testing. TASK-007 introduces the session-lifetime handoff; user teach-back remains pending.

Main risk: using GameMode or GameInstance as a universal rules container instead of a coordinator.
