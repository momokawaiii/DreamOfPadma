# PadmaWorld Module Program Document

- Document ID: ARCH-MODULE-WORLD-001
- Version: 0.2
- Status: Planned logical boundary; TASK-007 fixed Demo fixture is implemented in the staging module; no standalone UE module yet
- Canonical language: English for Agent consumption
- Chinese companion: README.zh-CN.md
- Owner: World Module Agent
- Current implementation: Source/DreamOfPadma/ is the temporary shared runtime module; TASK-007 owns the project-owned Demo Sandbox fixture under Content/Padma/Demo/World/
- Parent architecture: ../../ProgramArchitecture.md and ../../DataDrivenArchitecture.md

## 1. Purpose

PadmaWorld owns the shared sandbox graph and persistent world mutations. It makes player operations and ruler responses visible as changes to stable nodes, ownership, routes, facilities, encounters, and environment flags.

The TASK-007 slice adds one explicitly playtest-only Demo Sandbox fixture. Its presentation owns selection/highlight state, while the transition identity is expressed through typed stable IDs and handed to the Game session boundary. This fixture does not define the production graph or world rules.

## 2. Responsibilities

- Stable node and edge identity independent of coordinates.
- Hand-authored MVP graph and later World Partition/PCG seams.
- Node types: home/core, ordinary, anecdote, furnace, safe-house, outpost, and ruler core.
- Movement legality, adjacency, placement, capture, facility operations, and path conditions.
- Player and ruler ownership state.
- World mutations and mutation events.
- Local battle-route entry/exit boundaries (Encounter or RealTimeAction), including an ABC-card character moving onto a point occupied by an enemy, jointly constrained engagement routes that can select/trigger the presentation mode and story, and complete pre-battle snapshot-token requests.
- Terrain, weather, and environment modifiers as data-driven inputs.
- Story choices that produce persistent world-state changes.
- One fixed Demo Sandbox tile presentation, typed selection/confirm requests, and fixture validation for the TASK-007 slice.

## 3. Non-responsibilities

PadmaWorld must not own:

- Damage formulas, status resolution, or AI utility scoring.
- Card synthesis candidate selection or success probability.
- UI decisions or camera presentation.
- Save serialization of Actor pointers.
- A hard-coded level-name switch for a gameplay condition.

World state may expose legal queries and accept commands, but mutation must remain in the world rule/application path.

## 4. Public contracts

- NodeId, EdgeId, NodeDefinition, and NodeRuntimeState.
- Move, place, attach, capture, facility, anecdote, furnace, safe-house, and path-unlock commands.
- World mutation result and events.
- Local battle-route entry context, engagement-route constraints, selected mode/story trigger, complete snapshot token, and return-to-sandbox commit/rollback result.
- Read-only graph and ownership queries.
- Terrain/environment modifier inputs.
- `FDemoNodeId`, `FDemoScenarioId`, `FDemoSpawnPointId`, `FDemoSelectNodeRequest`, `FDemoConfirmNodeSelectionRequest`, and `FDemoTransitionContext` for the fixed Demo slice.
- `FDemoWorldTileDefinition` and `FDemoWorldSelectionModel` for map-free fixture lookup and selection state.

Coordinates and level assets are presentation data. Stable IDs are the identity used by events, saves, and tests.

The TASK-007 implementation converts a transient tile hit into `FDemoNodeId`; no Actor pointer or raw coordinate enters `FDemoTransitionContext`. The context is published, peeked, and consumed through `UDemoTransitionSessionSubsystem`, whose lifetime is the GameInstance/session boundary.

## 5. Data and runtime ownership

World consumes WorldGraph, NodeDefinition, MutationDefinition, EncounterDefinition, StoryEvent, TerrainProfile, engagement-route, and environment data. It owns mutable node ownership, discovery, garrison, facility, route, and mutation state. PadmaGame coordinates the transaction; PadmaWorld provides the world-state snapshot boundary and commit/rollback result.

The first map is a rules testbed. World Partition, PCG, large terrain, water, weather, foliage, and final art can replace presentation without changing stable world contracts.

## 6. Dependencies and integration

World depends on PadmaCore IDs, commands, state values, and events. It is composed by PadmaGame and exchanges Encounter/ACT battle-route requests/results with PadmaGameplay.

For TASK-007, World calls only the typed session contract exposed by PadmaGame. The session store validates all three IDs before replacing a pending context, so an invalid fixture leaves an existing context unchanged.

PadmaWorld must not call UI code or use a widget as authority for capture or route state.

## 7. Tests and debug evidence

Required tests:

- Stable graph IDs and adjacency.
- Legal/illegal movement and placement.
- Node capture, facility mutation, and conditional path.
- Anecdote choice changes persistent state.
- Furnace and safe-house operations.
- Local battle-route entry from an ABC-card character reaching an enemy-occupied point, joint engagement-route constraints, mode/story trigger resolution, complete pre-battle snapshot, commit on success, and exact rollback on defeat/exit.
- Same world event sequence restores the same state.
- Coordinates can change without changing node identity.
- TASK-007 automation test: stable node/scenario/spawn IDs survive context creation; invalid replacement and missing fixture fields fail without mutating the prior context.
- DemoSandbox map-load smoke: the project-owned map starts the fixed fixture and emits the user-readable selection instructions.

Debug output should show command IDs, node IDs, ownership before/after, route conditions, mutation source, encounter context, and event order.

## 8. Implementation stages

1. Implement the TASK-007 fixed Demo fixture, selection presentation, and typed transition handoff in the staging module.
2. Implement a data-defined graph with placeholder visuals.
3. Add movement, placement, capture, and one mutation.
4. Add one anecdote, furnace, and safe-house flow.
5. Add ruler response hooks, joint engagement-route constraints, Encounter/ACT/future-mode trigger seams, and complete snapshot transaction boundaries.
6. Add environment seams after the rule graph is testable.

## 9. Learning targets and risks

Learning targets: stable world identity versus presentation coordinates, typed selection-to-session handoff, world-state modeling, graph-based level design, Gameplay Framework/World integration, save boundaries, World Partition planning, PCG seams, and environment systems. TASK-007 introduces the first two targets without claiming the user's teach-back evidence.

Main risk: storing gameplay truth in level Actors or coordinates so a map rebuild destroys saves and replay compatibility.
