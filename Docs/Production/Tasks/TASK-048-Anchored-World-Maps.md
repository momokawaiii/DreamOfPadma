# TASK-048 Anchored World Maps and Legacy Retirement

- Chinese companion: TASK-048-Anchored-World-Maps.zh-CN.md
- Status: Verified implementation candidate; user interaction acceptance pending
- Primary: root; current Local codex/mvp-demo-foundations
- Authorization: retire the old Demo assets/code/tests; improve the 3D map; open map definition/editing; add seeded generation and PCG. Preserve authored anchor identities and coordinates.

## Scope and acceptance

One integrated map delivery. Preserve unrelated working changes and current Encounter/ACT behavior. Root owns integration, source/config/build changes, asset migration, scripts, shared documents and checks. Bounded workers own disjoint map-definition/generation, world presentation/PCG, and legacy retirement packages. UE builds and asset operations remain serial under root. Preserve third-party content and historical evidence; no Git staging/commit/merge/push.

Allowed paths: Source/DreamOfPadma Core/Game/World/Presentation/UI and affected legacy/test paths; Config startup/packaging changes; DreamOfPadma.uproject and Build.cs PCG dependencies; project-owned map/model-preview assets; Scripts/Editor retirement/map-authoring scripts; relevant paired Docs. Back up the exact old assets before UE-mediated deletion and verify referencers. Do not delete unrelated content.

Acceptance: old Demo maps/definitions and runtime chain retired after preview migration; current map starts by default. Scene map has camera navigation, hover/selection/path feedback, configurable terrain/building/garrison visuals. Authored definition and generated layout use stable IDs and explicit edges, support editing and validation, and preserve fixed anchors. Seed generation is reproducible; PCG creates cosmetic decoration without altering topology. Run/save/rollback use the same frozen map. Tests cover valid bypass branches, reject invalid generated layouts, impossible road prerequisites and incompatible saves. Editor build, focused automation, actual asset/PCG rendering and map-to-battle return, doc audit and strict validation pass; user tests interaction.

Confirmed: FixedNPC always appears in its authored region/location. MainStory encounters/completion affect later related story. An authored checkpoint evaluates Seen/Completed once on arrival and freezes matched/missed flags and dialogue. Bypassing earlier nodes is allowed; no premature missed decision and no universal all-anchors movement/victory gate. Dialogues retain the existing noon operation rule; arrival bookkeeping is independent. Editor closure was confirmed before migration.

## Evidence

Delivered the map authoring kit at `/Game/Padma/World/Maps`, with default first-region seed 12345 (7 authored nodes, 2 generated branch nodes, 13 edges). The runtime default is `L_PadmaWorld`. All 6 legacy assets and 21 old source files (including 2 obsolete test files / 3 test cases) plus `AuthorDemoEncounter.py` are retired. `Artifacts/TASK-048/legacy-backup` retains exact backups. Preview catalog/map now bind a general garrison model; current TASK-040 tests, TASK-046 tests and naming/save compatibility remain active. UE deletion left five disk residues; hashes matched backups before explicit cleanup. The repeatable retirement script now checks disk as well as registry state.

| Files / directory | Responsibility |
| --- | --- |
| `Core/Content/PadmaContentTypes.*`, `Core/WorldMap/PadmaWorldMapDefinition.*` | Map/anchor/checkpoint value contracts, deterministic branch generator, graph/prerequisite validation and signatures |
| `Core/Run/PadmaRunRules.*`, new `PadmaRunStory.cpp` | Frozen run topology, NPC encounter/completion, once-only checkpoints, compatible atomic restore and rollback |
| `Game/WorldMap/PadmaWorldMapAsset.*`, `PadmaMapAuthoringPreview.*` | Editable asset, explicit checkpoint-table import, generation/validation controls and transient scene handles |
| `Game/Content/PadmaContentCatalog.*`, `Game/Run/PadmaRunSubsystem.*` | Map/theme loading, legacy graph retention, atomic configured new run |
| `Game/Framework/PadmaPlayerController*`, `UI/Screens/PadmaGameScreen.*` | Camera/3D inspection, deployed-card/hand layout, seed confirmation and command submission |
| `World/Map/PadmaWorldMapActor.*`, `PadmaMapVisualTheme.*`, `World/PCG/PadmaDecorationPoints.*` | Cached extruded terrain, models/feedback, independent cosmetic points and real PCG Static Mesh Spawner |
| `Game/Presentation/PadmaPresentationCatalog.*`, TASK-040 model test | Remove retired character adapter while preserving explicit remaining enum values and generic preview |
| `Scripts/Editor/AuthorWorldMaps.py`, `RetireLegacyDemo.py`, `AuthorModelPreview.py`, `ExportPlayableData.cjs` | Create/preserve map kit, exact retirement and residual audit, retained generic preview, compatible legacy export defaults |
| `Tests/WorldMap/*`, `.uproject`, `Build.cs`, `Config/DefaultGame.ini`, existing default-map config | Map/run coverage, built-in PCG dependency and world content cooking; retain the user's current L_PadmaWorld startup selection |

Paths above are relative to `Source/DreamOfPadma/{Public,Private}` except explicitly named scripts/config. These layers keep rule values independent of PCG/Actors/UI, separate content loading from authoring, and make visuals replaceable. No new universal manager or separate project plugin is needed for this scope. The [authoring guide](../../Content/WorldMapAuthoring.md) lists every user-facing asset and workflow; [ADR-0007](../../Decisions/ADR-0007-Anchored-Map-Generation.md) records the frozen-layout save amendment.

## Validation and limits

- UE 5.8.2 `DreamOfPadmaEditor Win64 Development` passed after source fixes. Current native code has no known new compiler or reflection errors.
- 22 focused automation tests passed: 8 world-map tests, 11 retained TASK-046 tests, 2 TASK-040 tests and 1 TASK-047 compatibility test. Evidence: `Artifacts/TASK-048/Tests/index.json`, `automation.log`. A test's initial self-referential TArray insertion and missing newly introduced defaults in the old JSON export were fixed before the passing run.
- The authoring kit validates and regenerates; the scene preview creates 7 handles and clears all transient map/node/handle actors. Existing generic preview resolves garrison, building and ground with `Visual=1`; no retired character type is required.
- Real native rendering: `WorldMap.png` (1920×1080), `render-world.log`, `render-roundtrip.log`. Map → Encounter → rollback → map passed; returned map generated 200 PCG instances, `layout-build=1`, with no observed runtime errors. Vertex winding, localization-history import and material output/instance-usage defects found in the first screenshots were corrected. Cached geometry is separate from hover/selection state.
- Final `assets-final.log` passed from a fresh UE process: repeated authoring preserved the kit, all three generic previews resolved, registry and disk had no legacy assets. The empty old directory was removed after checking for remaining files. `EditorBuild.log` records the successful build.
- Independent focused review closed all five map/save findings with **Pass**, using the 22/22 report. `AuditDocs.ps1` passed (230 Markdown files / 115 pairs), `ValidateProject.ps1 -Strict` passed and scoped `git diff --check` passed. Generated evidence remains ignored; no staging/commit/merge/push.

User still tests interaction. Art remains a configurable low-poly placeholder; supplied screenshots are actual UE, not final visual targets. This is a small authored graph plus seeded branches, not free adjacency, a full custom graph-editor plugin, large-world terrain sculpting or competitive symmetry/fairness guarantees. Branch spacing needs visual authoring checks. Full story content and packaged-build compatibility remain unverified scope; the NPC branch text and existing balance are temporary fixture content.
