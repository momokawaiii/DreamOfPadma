# ADR-0003: Fixed Playable Prototype Vertical Slice

- Chinese companion: `ADR-0003-Playable-Prototype-Vertical-Slice.zh-CN.md`
- Status: `Accepted`
- Date: `2026-09-07`
- Implementation status: Not authorized; documentation only
- Owner: Integration Coordinator / Architect Agent
- Task contract: `../Production/Tasks/TASK-006-Playable-Prototype-Cluster.md`

## Context

The clean DreamOfPadma project has verified PIE startup but still contains only the generated runtime module. The reference project at `E:\2026ue\padma` demonstrates a useful first playable path: select a strategic tile, carry tile context into a local scene, present card data, spawn a character, and inspect the character's skills.

The project needs visible progress before the complete M1 rule foundation is implemented. A narrow vertical slice can provide that progress without copying the old prototype's monolithic manager, direct UI mutation, random calls, unresolved gameplay defaults, or entire asset tree.

## Decision

1. The near-term integration Goal is a fixed-fixture playable slice:
   `Sandbox map -> select one demo tile -> enter its mapped Encounter scene -> show one demo card -> play it -> spawn one character -> inspect its skill list`.
2. The slice uses explicitly marked playtest fixtures. A demo tile-to-scene mapping, one character, one card, and a small skill list are presentation fixtures, not the final world graph, card lifecycle, combat, or content namespace.
3. The slice stays in the current generated `DreamOfPadma` runtime module. It does not create permanent UE module splits, enable GAS, add external gameplay plugins, or change the engine association.
4. A replaceable session/transition context carries stable demo identifiers such as node, scenario, and spawn-point identity across the scene change. A direct level load may be used for this demo behind that boundary; this does not decide the final World Partition or streaming strategy.
5. Project-owned demo assets live under `Content/Padma/Demo/`. The old prototype remains reference-only. Bulk copying `E:\2026ue\padma\Content`, editing external assets in place, or importing an unreviewed asset dependency tree is not authorized.
6. UI converts input into the approved typed command path and consumes results/events or read-only views. UI, map Actors, and card widgets do not become authoritative gameplay state.
7. The first slice reads a character's skill definitions and demonstrates card-to-character spawning. It does not activate skills, resolve combat, choose damage/status formulas, or implement Encounter timeline details.
8. A broad Unreal automation suite and the in-PIE debug panel are separate follow-up work. The first slice requires compile evidence and manual PIE evidence; a narrow service test may be added when it does not expand the scope.

## Consequences

- The user receives a reproducible, playable demonstration early.
- Later World, Gameplay, and UI implementations can replace the fixture without changing the required command/result/event direction.
- The slice intentionally leaves production ID namespaces, import pipelines, resource policy, card lifecycle probabilities, route semantics, and exact Encounter timing unresolved.
- The first implementation may need the minimum UMG dependency in the existing staging module. That dependency must be added only by an approved implementation task and its single designated writer; no dependency change is made by this ADR itself.
- End-to-end validation remains serial because maps, binary assets, central build configuration, Editor sessions, and the PIE lane are shared resources.

## Rejected alternatives

- Porting the reference project's `CoreGameManager` and all related assets as the first implementation.
- Building a complete SLG graph, card lifecycle, combat system, and debug panel before showing one playable route.
- Letting a Widget call `SpawnActor` or mutate a world/gameplay manager directly.
- Making TASK-005 deterministic random infrastructure a hard prerequisite for a fixed, non-random demo fixture.
- Treating a working debug panel as a substitute for repeatable service or rule validation.

## Deferred decisions preserved

- The exact joint battle-route resolution and whether the overworld and Encounter scene share World Partition.
- Movement cost, resource policy, card lifecycle, deck/draw behavior, and any reward or failure semantics.
- Encounter action-bar precedence, response/interrupt windows, and exact card allowance in unresolved windows.
- ABC-card legality inside local battle, ACT implementation, GAS ownership, and external plugin adoption.
- Final stable-ID namespace, import format, save schema changes, network authority, and presentation asset licensing/selection.
- All other items explicitly marked Open, Proposed, Deferred, or playtest-only in the accepted design baseline.
