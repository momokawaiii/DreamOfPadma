# World Map Authoring

- Document ID: CONTENT-WORLD-MAP-048
- Version: 0.2
- Status: Current authoring kit with a separately marked offline Demo bake target; user interaction acceptance pending
- Chinese companion: [WorldMapAuthoring.zh-CN.md](WorldMapAuthoring.zh-CN.md)
- Delivery evidence: [TASK-048](../Production/Tasks/TASK-048-Anchored-World-Maps.md)

Use the map Data Asset for graph values, the editing level for moving node positions, and the visual theme for presentation. A running game uses its frozen layout and checkpoint values.

## Open the authoring kit

Build the current `DreamOfPadmaEditor`, then execute [AuthorWorldMaps.py](../../Scripts/Editor/AuthorWorldMaps.py) in the Unreal Editor Python console:

```python
import runpy, unreal
runpy.run_path(unreal.Paths.project_dir() + "Scripts/Editor/AuthorWorldMaps.py", run_name="__main__")
```

The script creates missing assets and preserves existing definitions on rerun. It fills empty map/theme catalog bindings and adds the branch dialogue table. It opens the editing level and clears its temporary preview after the script checks; press **Rebuild Preview** to display it again.

| Purpose | Asset path |
| --- | --- |
| Map definition | `/Game/Padma/World/Maps/Definitions/DA_FirstRegion_Map` |
| Editing level | `/Game/Padma/World/Maps/Editing/L_MapAuthoring` |
| Visual theme | `/Game/Padma/World/Maps/Presentation/DA_FirstRegion_Theme` |
| Checkpoint table | `/Game/Padma/World/Maps/Story/DT_FirstRegion_Checkpoints` |
| Branch dialogue table | `/Game/Padma/World/Maps/Story/DT_FirstRegion_Branches` |
| NPC model definitions | `/Game/Padma/World/Maps/Models/DA_MapNPC_story`, `/Game/Padma/World/Maps/Models/DA_MapNPC_forge` |
| Runtime catalog | `/Game/Padma/MVP/Playable/Definitions/DA_PlayableCatalog` |
| Playable level | `/Game/Padma/MVP/Playable/Maps/L_PadmaWorld` |

## Edit graph values and positions

1. Open `DA_FirstRegion_Map`. Edit `Template.Nodes` and `Template.Edges` for IDs, terrain, ownership, encounter data and directed connections. `HomeNode` and `BossNode` reference existing, distinct node IDs; home must start owned by `player`. Operation types such as `core` and `rulerCore` remain separate from these terminal references.
2. Open `L_MapAuthoring` and select its `PadmaMapAuthoringPreview` host. Assign `Map` and `Theme`; leave **Edit Generated Layout** disabled to edit `Template`.
3. Press **Rebuild Preview**. In the Outliner select `Edit node: <ID>` and move its yellow handle in the horizontal plane. Press **Apply Node Transforms** on the host, then read `Status`. Handle height does not change map coordinates.
4. Save the map Data Asset explicitly. Press **Rebuild Preview** to display the applied values. **Clear Preview** removes temporary handles and rendered preview actors; the saved definition retains applied edits.
5. Set `bUseGenerated=false` and press **Validate** to validate the authored template, or press **Generate** to build a fresh output from it. Inspect `ValidationError` before saving.

Scene handles edit positions only. Add/remove nodes and edit edges in the Data Asset, then rebuild the preview. Changes to handles are not applied automatically. The editor uses the same coordinate conversion as the runtime presenter.

`FixedNPC` and `MainStory` anchors retain `Id`, `NPCId`, dialogue identity and exact coordinates during generation. Authoring `Template` can deliberately establish new fixed positions. When **Edit Generated Layout** is enabled, applying a moved/changed anchor or terminal is rejected against `Template`.

## Generate and choose the active map

| Control | Effect |
| --- | --- |
| `Seed` + `GenerationSettings` | Configure repeatable branch count, length, spread and generated terrain choices. |
| **Generate** | Validate the template, replace `GeneratedLayout` on success and set `bUseGenerated=true`; failure preserves the previous output. |
| **Validate** | Check the active layout; generated mode also checks fixed identity/position against the template. |
| **Restore Template** | Copy `Template` over `GeneratedLayout` and set `bUseGenerated=false`. This replaces manual generated-output edits. |
| `bUseGenerated` | Select the asset's active graph; the editor host's edit toggle selects the graph being edited independently. |

The kit initially uses 1–3 branches, one node per branch and spread `120` in map coordinates. Increase spread carefully when branches overlap existing tiles. Generation keeps authored nodes and edges and adds explicit directed paths; the same template/settings/seed reproduce the result.

Validation checks IDs, finite coordinates, directed cycles, structural reachability, checkpoint references and known/reachable road-unlock producers. Seed changes affect topology, not only scenery. No branch is automatically made a universal story or victory gate.

In the playable UI, **创建地图新局** offers a seed-generated new game or the configured active map. Starting replaces the current unsaved run. Editing an asset does not rewrite a running or saved map; begin a new game to use the edited definition. Save/load and battle rollback retain the run's frozen layout.

## Author checkpoint branches

Use `FPadmaStoryCheckpointRow` in `DT_FirstRegion_Checkpoints`:

| Field | Meaning |
| --- | --- |
| `Id`, `NodeId` | Unique checkpoint ID and its arrival node; only one checkpoint per node. |
| `NPCId` | Existing anchored NPC identity to evaluate. |
| `bRequireCompletion` | `true`: encounter completed; `false`: NPC seen. |
| `MatchedFlag`, `MissedFlag` | Distinct, nonempty result flags; arrival records exactly one. |
| `MatchedDialogueId`, `MissedDialogueId` | Optional branch dialogues; configure their lines in the catalog's dialogue tables. |

1. Edit and save the checkpoint DataTable. Assign it in the map asset's `CheckpointTables` array.
2. Press **Import Checkpoint Tables**. This explicitly copies the rows into `Template.Checkpoints`, validates them and switches to authored mode. Failed import leaves the template unchanged.
3. Generate again if a generated map should use the new checkpoint rows, then save the map asset. Editing a table alone does not update the frozen template, generated output or existing runs.

The kit checks NPC `watcher` when arriving at `gate`: completion selects `watcher-aid` / `gate-met`; otherwise it selects `watcher-absent` / `gate-missed`. Passing an earlier fork or leaving an NPC unvisited does not resolve the checkpoint early.

Arrival records the branch once. Dialogue completion and rewards still follow the noon operation rules; a cancelled dialogue can be reopened later when eligible, without rerolling the recorded branch. `CompletionFlag` is emitted on completed node dialogue. A legacy `story` operation without an explicit flag emits `story-road`. A road uses a flag only when its `UnlockFlag` explicitly names it.

## Camera, models and PCG

- In the unobstructed map area: wheel zooms, middle-button drag pans, WASD/arrow keys pan, Q/E rotate, and Home or **地图全览** fits the graph. Hover/selection feedback and outgoing-road highlights update separately from static geometry.
- In `DA_FirstRegion_Theme`, `TerrainVisuals` maps exact terrain IDs to `Color`, `Height`, optional `GroundMaterial` and `GroundMesh`. The kit configures `城镇`, `平原`, `林地`, `丘陵`, `山地`.
- `NodeBindings` selects `GroundModel`, `BuildingModel` and `NPCModel` by stable `NodeId`. Each references a `UPadmaModelDefinition`; set its mesh and `ModelTransform`. Missing bindings use low-poly placeholders. The kit's two NPC models use engine cylinders.
- `DecorationMeshes`, `DecorationMaterial`, count and scale configure the actual PCG Static Mesh Spawner. Its independent seed stream samples cosmetic points outside node/road keepouts, including mesh footprint. PCG does not settle movement, generate gameplay edges or consume gameplay RNG.
- `[PadmaMapPCG] requested` and `completed ... instances=...` are runtime evidence hooks. `[PadmaMap] layout-build=...` should remain stable during ordinary hover/state refresh; these hooks are not a claim that this guide's checks have already passed.

TASK-050 replaces the extruded hex field with a continuous procedural surface, graded roads and controlled surface-following PCG. Project-owned tree, rock, landmark and material samples are described in [StrategyPresentation](StrategyPresentation.md). This is not an ALandscape sculpting asset. A custom graph editor, symmetrical maps and competitive fairness guarantees remain outside the delivery; generated branch spacing and custom model scale still need authoring checks.

Source entry points: [map asset](../../Source/DreamOfPadma/Public/Game/WorldMap/PadmaWorldMapAsset.h), [editor adapter](../../Source/DreamOfPadma/Public/Game/WorldMap/PadmaMapAuthoringPreview.h), [generator](../../Source/DreamOfPadma/Public/Core/WorldMap/PadmaWorldMapDefinition.h), [visual theme](../../Source/DreamOfPadma/Public/World/Map/PadmaMapVisualTheme.h).

## Chapter Zero bake target — not implemented by the current kit

[ADR-0010](../Decisions/ADR-0010-Offline-Demo-Content-and-Map.md) selects editor generation/baking for the offline Demo. The existing **Generate** button creates graph values; it is not a complete terrain/PCG bake-and-cook action.

| Path | Current behavior | Role in the target |
|---|---|---|
| Default painted tutorial | Fixed painting with GPU atmosphere and versioned hex values | Existing prototype; visual replacement/mapping still needs implementation and art acceptance |
| Legacy authored/generated 3D map | Continuous procedural surface and runtime cosmetic PCG | Reusable sample components; not yet the shipping bake pipeline |
| Target Chapter Zero | Fixed Seed/anchors/edges, algorithmic terrain, editor-generated decoration and baked outputs | Packaged Development and Shipping load the same frozen content |

The legacy kit may add seeded gameplay branches. That capability is not permission to randomize Chapter Zero's fixed teaching route. Preserve the seven existing landmark identities (`home/fire/story/plain/gate/forge/boss`) and their roles. Exact coordinates for a new terrain renderer must be authored and versioned; do not silently apply painting coordinates as world units.

### Authoring pipeline

1. Author chapter anchors, explicit edges, locks, fixed Seed and map configuration. Establish protected corridors before terrain generation.
2. Build terrain values in stages: land/water shape, relief, climate/terrain classification, rivers/water and vegetation/resource suitability. Respect fixed sites and legal routes throughout; this is a proposed algorithm decomposition, not a requirement to copy Civ6 code or use one named noise algorithm.
3. Validate the gameplay result: stable IDs, anchor positions, usable routes, reachability and locked partitions. A pretty terrain result cannot grant an extra edge or bypass a lock.
4. Freeze the value layout and its MapKey. If a validation fails, keep the prior accepted output and report the violated node/edge/configuration.
5. Run cosmetic PCG using an independent decoration seed/theme: sample accepted terrain, reject water/steep slopes and node/road keepouts, place meshes/decals. Preview and tune density/scale without changing gameplay state.
6. Bake the required terrain/material/decoration outputs into project assets; retain matching MapKey metadata and immutable generator manifest references. Ensure results are loaded without rerunning PCG in packaged play.
7. Validate packaged asset inclusion and open the same map in both Development and Shipping. Save frozen gameplay layout/state and restore against the matching baked metadata.

The exact terrain representation (procedural mesh, Landscape, authored tiles or another renderer), bake API, mesh/material budget and final assets are not frozen by this document. Height, surface visuals and movement classification must agree; decoration is not terrain authority.

### Version and iteration rules

- MapKey and manifest fields are owned by [DataDrivenArchitecture](../Architecture/DataDrivenArchitecture.md); load/mismatch behavior by [SaveSchema](../Architecture/SaveSchema.md).
- Any changed input affecting the frozen map output gets a new immutable manifest version. A Seed alone is insufficient; dialogue-only edits use ChapterContentVersion.
- Keep source configuration distinct from generated/baked output. Rebuilding a preview does not overwrite a player's saved map or advance gameplay RNG.
- Future chapters may vary non-anchor terrain/topology only through their approved gameplay generator. PCG remains cosmetic.
- Required evidence: same-input output determinism, anchor/locked-route validation, decoration exclusion and RNG isolation, matching bake metadata, and package loading without runtime generation. No completed bake evidence is claimed here.
