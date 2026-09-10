# ADR-0005: Layered World Map Storage

- Chinese companion: ADR-0005-World-Map-Storage.zh-CN.md
- Document ID: ADR-0005
- Version: 0.3
- Status: Accepted direction; concrete schema gated
- Date: 2026-09-09
- Owner: Integration Coordinator / Architect, with user gameplay approval
- Task: TASK-011; implementation owners TASK-012/024/025/028/029

## Context

The source rules describe shared sandbox nodes and conditional routes. The user selected nodes with preset connections, shown over a hex-shaped map with a Civilization VI-like oblique camera, then explicitly accepted separate static definition, .umap presentation and SaveGame run changes. The old TASK-007 Actor fixture is a fixed visual prototype, not the production world model.

## Decision

1. A versioned chapter map definition asset owns MapId, stable NodeId/EdgeId values, authored connections, node types/terrain/event references and initial configuration references. The first implementation uses a project-owned typed UE data asset; exact fields and asset paths are frozen in TASK-024 preflight.
2. Explicit connections are topology truth. Display coordinates or touching hex meshes cannot create legal travel. Edge direction, conditions and exact map layout remain authored data; unconfirmed details are not inferred.
3. The .umap owns terrain/props, light, camera/presentation setup and a reference to the map definition. Node visuals map to IDs. It does not own the authoritative current world state.
4. World state services own mutable discovery, ownership, facilities, road state, garrisons and entity locations. Player input becomes a typed command; validation and payment precede an atomic state update and event. Failure preserves location and resources.
5. Versioned SaveGame records MapId/definition version and all approved mutable run state. TASK-012 establishes participant contracts, TASK-024/025 supply World state, and TASK-028/029 coordinate complete battle snapshots and safe persistence. Static topology and live Actor pointers are not duplicated as save truth.
6. Validate map compatibility before restore; missing IDs, incompatible definitions or interrupted loads fail readably without partially applying state. Visual rebuilding or camera movement cannot alter node identity or run progression.
7. ACT characters/roster remain a separate domain. World entry passes stable location/terrain/context; no implicit card conversion or Actor persistence is introduced.

## Consequences and alternatives

The user can revise presentation while preserving node-based progression. Graph validation and state tests do not require the rendered level. Manual authoring remains possible in UE; a future node editor can author the definition asset. Keeping every rule field on map Actors was considered and not selected.

This ADR does not require World Partition, PCG, a new standalone module, a final import format, an eight-node layout chosen by the Agent or free movement. Exact costs/limits require user confirmation. Large-world streaming remains a future presentation decision.

## Verification

TASK-024 tests graph IDs/edges, illegal movement with no payment, state restoration after visual rebuild and data validation. TASK-028/029 test complete snapshot coverage, version mismatch and safe load failure. The same tests use explicit node connections regardless of the hex-shaped presentation.

## Local scene extension

TASK-037 preflight freezes NodeId → SceneDefinitionId, typed soft level/model references and authored local placements. Multiple nodes may reuse a scene template. Initial placements are static content; mutable entity placements remain approved world state. Local Actors bind existing card-instance/EntityId values and a versioned observer-filtered world view from TASK-024; they cannot own a second card/occupancy store. Apply visibility before constructing Actors; stale/wrong-node loads or missing assets preserve prior authoritative state. D24/D25 gate inspection/movement/battle composition and disclosure. Only actual battle entry invokes TASK-028. This extends the accepted storage layers, not completed runtime behavior.

## Selected model assets and card inspection — 2026-09-09

Click is read-only; Move Here separately moves selected card instances and validates the entire A/attached-B group. Enemy node content is fully public in the first MVP. A static scene definition owns SceneDefinitionId, soft Level and SlotId/transforms. World state supplies NodeId, instance ID and a domain-qualified definition key; Game resolves that key to the selected model/skill Data Asset. Model Actors and loaded pointers are transient projections. General model profiles are separate visual assets; ACT continues using its existing character/weapon asset fields as the only authority. TASK-040 independently provides authoring-only catalogs/bindings and a graybox map; they are not world occupancy or a new SaveGame schema. TASK-020/024/037/034 supply production binding and commands later.
