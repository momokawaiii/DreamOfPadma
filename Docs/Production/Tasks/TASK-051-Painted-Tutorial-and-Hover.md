# TASK-051 Painted tutorial and hover sidebar

- Chinese companion: TASK-051-Painted-Tutorial-and-Hover.zh-CN.md
- Status: Review (implementation verified; user art acceptance pending)
- Primary / Local checkout: Codex Primary / codex/mvp-demo-foundations
- User authorization: 2026-09-09 request for a fixed tutorial painting with shader atmosphere, reference-style HUD and hover-driven sidebar; retain existing key nodes and generate multi-terrain habitable wilderness that player and AI can occupy.

## Outcome and scope

Deliver a native painted tutorial map with real territory and contextual actions. The gold/navy interface uses a proportional 1672×941 design canvas, top calendar/resources, central painting, right node preview and bottom cards. Displayed resources, ownership, garrisons and movement costs come from the current run. The fixed source painting is 1774×887 (2:1); its visual input is the user's reference image, with the art request recorded in [art-prompt.txt](../../../Artifacts/TASK-051/art-prompt.txt). This is a reference-guided reconstruction, not a pixel-perfect reproduction claim.

The value-only tutorial generator produces 162 cells including the seven existing key nodes at exact painting positions, plus seeded multi-terrain wilderness. `tutorial-painted-v1` opts into explicit hex adjacency and occupiable habitable cells. Fresh runs select this layout unless `-PadmaLegacyMap` or the `encounter`, `act`, `roundtrip` scenarios request the legacy setup. Existing runs retain their frozen layout. [ADR-0009](../../Decisions/ADR-0009-Painted-Tutorial-Hex-Map.md) records topology, fixed positions and presentation ownership.

Player occupation uses validated movement and existing costs. At dusk, the added AI frontier chooses at most one adjacent empty neutral habitable wilderness cell; it adds no player-territory attacks, units or economy. Wilderness gives no war-balance/core/resource rewards, and generated paths preserve authored locks. Saved layout metadata and runtime ownership carry the result. CommonUI activation and inspection timing remain under [ADR-0008](../../Decisions/ADR-0008-Activatable-Presentation-Layers.md).

Hover changes preview state only. The last preview persists while entering the sidebar; selected cards remain selected, and explicit actions use the previewed stable node ID. Modal overlays and drag gestures gate map input. Existing battle, synthesis, dialogue, card inspection and preparation rules remain outside this presentation change except for the explicitly authorized tutorial wilderness behavior.

### Exact allowed paths

All source paths below are relative to `Source/DreamOfPadma/`:

- UI: `Public/UI/Screens/PadmaGameScreen.h`, `Private/UI/Screens/PadmaGameScreen.cpp`, `Public/UI/Screens/PadmaTutorialMapWidget.h`, `Private/UI/Screens/PadmaTutorialMapWidget.cpp`.
- Controller: `Public/Game/Framework/PadmaPlayerController.h`; `Private/Game/Framework/PadmaPlayerController.cpp`, `PadmaPlayerControllerMap.cpp`, `PadmaPlayerControllerUI.cpp`, `PadmaPlayerControllerInspection.cpp`.
- Run integration: `Public/Game/Run/PadmaRunSubsystem.h`, `Private/Game/Run/PadmaRunSubsystem.cpp`.
- Core: `Public/Core/Content/PadmaContentTypes.h`, `Private/Core/Content/PadmaContentTypes.cpp`, `Private/Core/WorldMap/PadmaWorldMapDefinition.cpp`, `Public/Core/WorldMap/PadmaTutorialMap.h`, `Private/Core/WorldMap/PadmaTutorialMap.cpp`, `Public/Core/Run/PadmaRunRules.h`, `Private/Core/Run/PadmaRunRules.cpp`, `Private/Core/Run/PadmaRunWilderness.cpp`.
- Tests: `Private/Tests/World/PadmaTutorialHexTest.cpp`, `Private/Tests/UI/PadmaStrategyInteractionTest.cpp`.

Project-relative authoring paths: `Scripts/Editor/AuthorTutorialPresentation.py`, `Scripts/Editor/ExportPlayableData.cjs`, `Content/Padma/UI/SourceArt/T_TutorialMap.png`, `Content/Padma/UI/Textures/T_TutorialMap.uasset`, `Content/Padma/UI/Materials/M_TutorialMap.uasset`, `Content/Padma/UI/Materials/MI_TutorialMap.uasset`. Evidence is restricted to `Artifacts/TASK-051/`.

Documentation: this TASK pair; `Docs/Decisions/ADR-0009-Painted-Tutorial-Hex-Map` and `Docs/Content/StrategyPresentation` pairs. Primary owns the necessary `Docs/Rules/WorldState`, `Docs/Rules/Time`, `Docs/ProjectState`, `Docs/00_INDEX` and `Docs/Changelog` pair updates. The delegated documentation writer touches only the first three pairs. Existing run rules and serialized layout fields carry calendar/save values; `PadmaRunSubsystem` additionally forwards the original authored map reference for cross-map save validation. Preserve unrelated local edits and keep one serial UE execution lane.

## Acceptance

- [x] Generated texture/material/instance validate successfully; source and imported texture retain 1774×887 pixels, UI parameters read back correctly, and the asset report contains no warnings.
- [x] Native tutorial renders the painting, gold/navy HUD and actual values at 1920×1080 from the proportionally scaled design canvas; the Primary inspected its reference-style composition without claiming pixel identity.
- [x] Water, haze and forge effects stay localized in fixed-time captures; landmark UVs remain fixed. `FreezeAnimation`/`PreviewTime` are verified through captures; the zero-strength source-art branch is checked in the material authoring code.
- [x] The 162-cell graph is deterministic, retains all seven exact key positions and story semantics, preserves locked routes and contains only validated traversable connections.
- [x] Player movement pays existing costs; dusk AI expands only to eligible neutral wilderness; no wilderness war/reward farming, player-territory attack or frontier change during battle/dialogue.
- [x] Save/load restores hex metadata, topology and ownership; authored/tutorial restore order and atomic rejection of invalid anchors pass alongside legacy regressions.
- [x] Hover immediately projects the correct node, retains the sidebar preview and selected cards, and submits no gameplay mutation. Modal/drag gates and explicit action targets pass focused regressions; native hover, sidebar retention, zoom, fit and menu gating were exercised.
- [x] Editor build, focused Core/UI and relevant legacy regressions, independent boundary review and native runtime inspection pass.
- [x] Final AuditDocs and strict structural checks after integration edits.

## Evidence and remaining work

Evidence root: `Artifacts/TASK-051/`. Final animation captures and integration checks are complete.

- `art-prompt.txt`: recorded image-generation request based on the user-supplied reference. Delivered source: `Content/Padma/UI/SourceArt/T_TutorialMap.png`.
- `author-presentation.log` and `tutorial-presentation.json`: completed asset-authoring rerun, report `status=ready`, no asset-report warnings. Texture and parent material were preserved; the material instance was created. Both source and imported texture are 1774×887. Shader hash and effective parameter values are recorded in the JSON.
- Authoring defaults: `AnimationStrength=0.7`, `CloudStrength=0.55`, `FreezeAnimation=0`, `PreviewTime=0`; `MapTexture` references the tutorial painting. Deterministic atmosphere uses `FreezeAnimation=1`, `PreviewTime=0`; `AnimationStrength=0` shows base art. Usage and exact asset paths are maintained in [StrategyPresentation](../../Content/StrategyPresentation.md).
- `build-final.log`: final DreamOfPadmaEditor build succeeded, 9.58 seconds, after the deck-counter layout fix. The later fit-button change was checked in the native session; the counter-only layout fix was checked in the final render.
- `Tests/index.json`, report `2026.09.09-14.53.22`: **31 passed** (30 clean, one expected invalid-generation warning), **0 failed / 0 not run**. `tests-final.log` ends with test exit code 0. Coverage includes tutorial generation/locked partitions, movement costs/frontier/reward boundaries, battle freeze, hover/gesture behavior and relevant legacy/save/combat regressions.
- Cross-map save authority fix: startup and `StartTutorialMap` copy the original authored layout before tutorial generation; `UPadmaRunSubsystem` forwards it to `FPadmaRunRules::Initialize(Content, AuthoredReference)`. Restore validates against that retained trusted reference. `AuthoredAndTutorialRestoreOrder` verifies both restore orders, unchanged saved values and atomic rejection of moved anchors or unconfigured map identity.
- Independent read-only boundary review: **Pass**, no P0–P3 findings remaining.
- `tutorial-time0.png`: actual native 1920×1080 render inspected by the Primary. `interactive.log` records the native Windows session after the map-fit fix. Pointer movement using a zero-scroll input updated the `plain` node sidebar without a click; moving into the sidebar preserved that target. The plus button zoomed, Fit reset after release, Escape opened a menu that blocked underlying hover, and closing it with Escape restored home hover. These interaction results are Primary-observed; the log records the session and layer activation.
- Sustained physical 500 ms press was unavailable through the current native automation API. Hold timing, move cancellation and consumed-release behavior have gesture-automation evidence; physical hold feel remains for user playtest.
- Documentation package check: `AuditDocs.ps1` passed (248 Markdown files / 124 pairs); scoped whitespace checks passed for all six files. Final integration checks remain with the Primary.
- `tutorial-final.png` and `tutorial-time4.png`: final 1920×1080 native renders with `-PadmaMapArtTime=0` / `4`. `animation-frame-check.json` records sampled pixel differences in water (572/816), valley haze (714/1620) and forge (331/440), while the sampled fixed HUD is identical (0/1260). Landmarks retain their source UVs; the final counter layout is legible. Neither log reports runtime errors.
- Final `AuditDocs.ps1`: 248 Markdown files / 124 pairs passed. `ValidateProject.ps1 -Strict` and `ExportPlayableData.cjs --check` passed. Scoped source/document whitespace checks include untracked files. Assets/evidence were left local; no Git integration was performed.

User visual/playtest acceptance and packaged-build behavior remain unverified. No commit, merge or push was authorized or performed by this package.
