# PadmaWorld

- Document ID: ARCH-MODULE-WORLD-001
- Version: 0.4
- Status: Implemented logical presentation boundary; Demo bake target pending
- Chinese companion: [README.zh-CN.md](README.zh-CN.md)
- Owner: World Module Agent
- Source authority: Source/DreamOfPadma/{Public,Private}/World/Map and World/PCG; project wrappers in Content/Padma

## Current responsibility

World presents terrain, roads, nodes, garrisons, anchors and feedback. The 3D path caches a continuous procedural surface and uses slope/water/node/road-filtered points with independent-seed PCG Static Mesh Spawner. The default painted tutorial is a separate UI presenter. Current sample art and controls are documented in [StrategyPresentation](../../../Content/StrategyPresentation.md).

Core/WorldMap owns value graph generation/validation; Core/Run owns mutable world state, movement, occupation and checkpoint decisions. Game owns loading/editor bridges/travel/save. The logical world domain spans those adapters; World scene Actors are not the state authority. This clarifies the older wording that assigned all world mutation to World.

## Contracts and integration

- FPadmaMapLayout: frozen values selected by Game; graph edges determine traversability.
- FPadmaWorldNodeView/FPadmaWorldEdgeView: stable IDs and read-only visual state.
- APadmaWorldMapActor / UPadmaMapVisualTheme: cached scene projection, terrain/theme/model references and visual feedback.
- Actor hits return stable node IDs; Game submits commands to Core.
- PCG receives cosmetic input and never creates gameplay edges, moves story anchors or advances gameplay randomness.
- Encounter/ACT site/context and settlement remain typed Core/Game/Gameplay contracts; World displays results.

World depends on Core value contracts and scene/presentation APIs. It must not use UI as capture authority, calculate damage/synthesis, serialize Actor identity or choose a rule by level name.

## Accepted Demo target

Follow [WorldMapAuthoring](../../../Content/WorldMapAuthoring.md) for the generate/validate/freeze/bake pipeline and [ADR-0010](../../../Decisions/ADR-0010-Offline-Demo-Content-and-Map.md) for precedence. Both packaged configurations load baked terrain/decoration with matching MapKey metadata. This pipeline is not delivered by the current runtime PCG sample.

Preserve fixed chapter landmarks, explicit routes and locks. Future procedural chapters can vary approved gameplay inputs; cosmetic PCG remains non-authoritative. No ALandscape, World Partition adoption or fixed terrain algorithm is implied.

## Validation, save and risks

Test geometry rebuild invariance, matching hit/visual coordinates, anchor/road exclusion, deterministic generation and cosmetic RNG isolation. The target also needs bake/cook/MapKey consistency and packaged loading without editor/runtime generation.

[SaveSchema](../../SaveSchema.md) owns frozen map/state compatibility; visuals reconstruct from that state and matching content. No scene pointers enter saves or future command transport. Build/PIE/PCG evidence remains in TASK-048/050/052; final art and packaged target acceptance remain pending.

Learning topics when requested: terrain representation, seeded generation, PCG filtering, instancing, bake/cook and map/state separation. Main risks are visual/gameplay disagreement, lock bypass and save identity changing with an art rebuild.
