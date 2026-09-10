# Native Playable Demo: Play, Content and Code

- Chinese companion: [NativePlayableDemo.zh-CN.md](NativePlayableDemo.zh-CN.md)
- Document ID: CONTENT-NATIVE-PLAYABLE
- Version: 0.4
- Status: Native implementation candidate; user playtest pending
- Delivery and evidence: [TASK-046](../Production/Tasks/TASK-046-Native-UE-HTML-Slice.md)
- Architecture decision: [ADR-0006](../Decisions/ADR-0006-Native-HTML-Slice.md)
- Runtime names and migration: [RuntimeNaming](../Architecture/RuntimeNaming.md), [TASK-047](../Production/Tasks/TASK-047-Runtime-Naming.md)

This is native UE C++/GAS with a CommonUI screen and 3D actors. It implements the supported HTML 0.4 rule subset, using user-authorized temporary UE Demo numbers. Coverage descriptions still identify unimplemented PDF abilities. FPS is preparation only; Combat's full combo system has not been migrated. Build, automation and rendering evidence belongs in TASK-046; this guide does not certify user interaction acceptance or a packaged build.

## Open and play

TASK-049 arranges the live calendar/resources at the top, selected node and garrison on the right, illustrated All/A/B/C hand and codex/synthesis at the bottom, and phase advance at bottom right. Click a selected card again to cancel. Inspecting a destination preserves the source group; only a valid enemy destination shows battle-mode selection and **移动并进入战斗**. Load and battle retreat require confirmation; Esc closes details/modals or cancels targeting before leaving a page. Modal overlays block underlying gameplay input. Preparation/local-scene navigation retains drafts; returning to the map discards unsaved previews. Advanced mobility changes are immediate and live separately in **菜单 → 试玩移动规则**. Current UI evidence: [TASK-049](../Production/Tasks/TASK-049-Native-Strategy-HUD.md).

Open `DreamOfPadma.uproject`, then open `/Game/Padma/MVP/Playable/Maps/L_PadmaWorld` in the Content Browser and press Play. This is now the default map; a new run starts automatically. `L_PadmaBattle` is entered through a pending world move; opening it alone returns to the world map. TASK-048 retires the old `/Game/Padma/Demo` chain. Use [WorldMapAuthoring](WorldMapAuthoring.md) for the new scene map, anchors, seed controls, editing level and PCG theme.

For a short first battle:

1. Press **推进至晌午** once, select **归处**, select the hand card **希律的舞女**, then **部署到此处**.
2. Select the deployed C card in the right-side garrison list. Select **薪火台**, then **移动到此处**. Repeat toward **风蚀原**.
3. Select **王庭关隘**. Select **Encounter** or **ACT** beside the move command, then **移动并进入战斗**. The preset graph edges determine legality; visually adjacent hexes alone do not.
4. Encounter uses the moved C cards. ACT uses the independent character/weapon saved in **全局备战 → ACT**. Its default Demo loadout can play immediately with placeholders.
5. Finish the battle and select **返回沙盘**. Victory commits the destination and surviving values. Defeat or confirmed **退出战斗并回滚** restores the complete pre-move state, including movement payment. Use **菜单 → 开始新局** to repeat the other mode.

| Screen | Interaction |
| --- | --- |
| World | Hover gives node context; click selects the full inspector. The right-side garrison list selects deployed cards; hands scroll along the bottom. Select cards first, then a 3D destination, then explicitly move. Wheel/middle drag/WASD/QE navigate the camera; Home fits the map. |
| A/B synthesis | Deploy A, select that deployed A, then deploy B to attach. Select A and preview/confirm synthesis. A and B are discarded on either result; successful C appears at their node. A and attached B move together, with each card's mobility checked. |
| Home | Double-click 归处 or press **进入归处**. **节点细节 / 主角展示 / 其他模式** switch the existing screen and presentation stage. **查看局部场景** shows the actual node's contents separately. |
| Preparation | Select protagonist/outfit/weapon/pose or independent ACT/FPS entries, then explicitly save the configuration. Returning to the map discards unsaved preview selections. FPS has no executable combat. |
| Dialogue | After occupying the appropriate node, **与守望人交谈 / 使用熔炉补给** opens lines and choices. Completion grants the one-time reward; leaving early does not grant it. |
| Encounter | The left queue and right action heading identify the current unit. Click ordinary attack, then click the affected enemy; a multi-target attack waits for the selected count. **取消释放** cancels freely. Guard ends that unit's action. |
| Shared basic cards | Drag from the bottom to a legal target, release, then **确认释放**. Clicking a card or pressing 1–5 also arms targeting. Invalid drops/cancel do not pay. All-target cards select the arena. Effects originate from the card/environment; their rules do not belong to the active C card. |
| Unit details | Hold a battlefield unit or its left-list entry for about 0.5 seconds. Enemy and friendly details include current attributes, available descriptions and optional portrait. Level is explicitly unconfigured. |
| ACT | WASD/arrow keys move; clicking the arena moves toward the point; Space attacks. Tab opens/closes the shared-card library. While open, simulation runs at 1/10 speed and rejects new movement/ordinary attack input. |
| Save/load | Use **菜单** outside battle/dialogue. The native slot is `PadmaDemo046`; HTML saves are separate. **开始新局** opens seed/asset-layout confirmation and reloads the catalog on commit. Saves retain their own map and checkpoint values. |

## What lives in a table, an asset and a save

| Kind | Responsibility | Example |
| --- | --- | --- |
| Typed DataTable | Many entries with the same schema: rules, numbers, IDs and supported effect parameters | `DT_Cards_C`, `DT_ACTCharacters`, `DT_FPSWeapons` |
| Data Asset with soft references | Authored content references for an identity: model, AnimBP, skill definition/table, portrait or presentation effect | `DA_Model_card_dancer`, `DA_ACTCharacter_traveller` |
| `DA_PlayableCatalog` | Lists the table shards and resolves stable IDs to presentation/ACT definitions | `CardTables`, `ACTCharacterDefinitions`, `Models` |
| `.umap` | Scene presentation and level entry; optional node display scene | `L_PadmaWorld`, `L_PadmaBattle`, a user-authored node scene |
| SaveGame | This run's instances, positions, resources, calendar, card piles, selections, flags and RNG state | Instance ID + definition ID + node ID; no actor or mesh pointer |

The definition table's `ModelId` resolves through `DA_PlayableCatalog.Models` to a `UPadmaModelDefinition`. Changing the home selection changes the saved identity; the display resolves that identity to the matching asset. The scene actor holds loaded presentation references only while it exists. A model does not inherently own skills: an ACT character definition explicitly supplies its ACT skill table, whereas Encounter uses its own rule rows.

The catalog currently creates **17 domain tables**, plus the separate ACT authoring-binding table. All are under `/Game/Padma/MVP/Playable/Definitions/Tables`:

| Assets | Native row type | Purpose |
| --- | --- | --- |
| `DT_Cards_A`, `DT_Cards_B`, `DT_Cards_C` | `FPadmaCardDefinitionRow` | ABC values, A→F/B→D/C→R, mobility, supported descriptions and `ModelId` |
| `DT_Nodes`, `DT_Edges` | `FPadmaWorldNodeDefinitionRow`, `FPadmaWorldEdgeDefinitionRow` | Static graph, positions, terrain, initial ownership, preset connections/unlocks |
| `DT_Profiles` | `FPadmaBalanceParameterRow` | Labelled temporary starting resources, costs, limits and combat parameters |
| `DT_EncounterSkills`, `DT_ACTSkills` | `FPadmaEncounterSkillEffectRow`, `FPadmaACTSkillEffectRow` | Separate mode effects/costs for shared basic-card IDs and slots |
| `DT_ACTCharacters`, `DT_ACTWeapons` | `FPadmaACTCharacterDefinitionRow`, `FPadmaACTWeaponDefinitionRow` | Independent roster entries, supported terrain trait and weapon multipliers |
| `DT_HomeCharacters`, `DT_HomeOutfits`, `DT_HomeWeapons`, `DT_HomePoses` | `FPadmaAppearanceDefinitionRow` | Home display identities and weapon/pose compatibility |
| `DT_FPSCharacters`, `DT_FPSWeapons` | `FPadmaFPSCharacterDefinitionRow`, `FPadmaFPSWeaponDefinitionRow` | Future FPS selection/display catalog |
| `DT_Dialogues` | `FPadmaDialogueLineRow` | Speaker, line, continuation, choices and completion/cancel flags |
| `DT_ACTAuthoringBindings` | Existing `FPadmaACTSkillRow` | ACT character skill index: skill ID, Slot1–5 binding and soft skill DA reference |

To split a growing domain, create another table with the **same row type**, for example `DT_Cards_C_Chapter02`, and append it to `DA_PlayableCatalog.CardTables`. No new C++ file is needed merely to add a shard. IDs must be unique across that domain's shards, including case-insensitive `FName` collisions; duplicate IDs fail validation instead of overwriting a previous table. Keep row names equal to IDs and update `CardOrder`, `NodeOrder` or `SkillOrder` when adding their respective entries. The shared skill ordering must match both mode catalogs. Do not rename IDs used by existing saves without a migration.

This is an entry-time checked snapshot, not continuous table polling or asynchronous streaming. New numbers/models and variants of supported behavior can be authored as data. A new card mechanic, arbitrary terrain predicate, new ACT combo or FPS firing system still requires implementation; a textual skill description cannot execute itself.

## Fill the art assets

The hand uses `/Game/Padma/UI/Textures/T_CardIllustrations`, sourced from `Content/Padma/UI/SourceArt/T_CardIllustrations.png`. Its 3×3 row-major order is dawn, earth, banquet / cherry, reed, cleopatra / vitruvian, dancer, door. `Scripts/Editor/AuthorStrategyUI.py` imports only when absent, sets UI compression/group and preserves existing assets on rerun. Reimport deliberately after changing the PNG. New card identities need an art index in `PadmaPlayerControllerUI.cpp`. This concept atlas does not replace the 3D world models with the reference's painted landscape.

Generated folders are `/Game/Padma/MVP/Playable/Definitions/{Tables,Models,ACT}`, `/Game/Padma/MVP/Playable/Presentation` and `/Game/Padma/MVP/Playable/Maps`. `DA_PlayableCatalog` sits directly under `Definitions`. Imported vendor content remains under `Content/ThirdParty`; project-specific wrappers, AnimBPs and materials belong under `Content/Padma`.

1. **Character/building appearance:** open the matching `DA_Model_*` in `Definitions/Models`. Assign one of `StaticModel` or `SkeletalModel`, and assign a skeleton-compatible `AnimationClass` for animated models. Adjust `ModelTransform` in the display stage. Check battlefield scale separately: its combat presenter currently uses its own capsule/mesh offsets. Materials are authored on the mesh/material assets; there is no universal material-override table in this slice.
2. **Portrait:** in `DA_PlayableCatalog.Models`, find the matching `Id` and assign `Portrait`. Combat long-press details consume this reference. The enemy placeholder uses `guard`; C cards use IDs such as `card.dancer`. `Pose` is an optional AnimSequence used by display presenters.
3. **Home outfit/pose/weapon:** `DT_Home*` rows select their `ModelId`. An authored skeletal outfit replaces the displayed body; this is not yet a modular cloth/garment simulation system. Assign the pose binding's `Pose` to an animation on that skeleton. Weapon display is beside the character; the stage does not automatically make a hand-held weapon fit a pose.
4. **ACT body:** open `Definitions/ACT/DA_ACTCharacter_<id>`, assign `Model`, `AnimationClass` and `SkillTable`. These references are authoritative for ACT runtime; the `ModelId` binding remains the fallback/metadata presentation hook. The generated skill table points to the five supported native Demo bindings. Retarget animations before mixing skeletons, and provide a Montage Slot in the AnimBP when using montages.
5. **ACT weapon:** open `DA_ACTWeapon_<id>`, assign exactly one of `StaticModel`/`SkeletalModel`, and fill `AttachmentSocket` with a real socket on the character skeleton. Runtime attaches the weapon there and uses the row's `ModelId` model asset's `ModelTransform` as its socket-relative offset/rotation/scale. Also replace that model asset if the separate preparation preview should show the same weapon. Adjust damage/range/interval multipliers in `DT_ACTWeapons`.
6. **Effects:** fill `DA_PlayableCatalog.Effects` entries such as `cut`, `rain` and `attack` with `Niagara`/`LevelSequence`. Battle receipts spawn Niagara at the affected unit and start an assigned sequence, alongside the readable fallback trail/rain and number. C card bindings `vitruvian`, `dancer` and `door` can supply their ordinary-attack `Montage`; ACT skill DAs have their own optional `Montage`. This is presentation playback, not automatic Sequencer source/target binding, hit timing or damage authoring. `Icon` exists as an authoring field but is not consumed by the current native screen.
7. **Node scene:** fill `NodePresentation` for the stable node ID with `GroundModelId`, `BuildingModelId` and optionally `Scene`. The latter is a display level instance loaded around the local display-stage origin. Author its contents near their local origin. Rules and deployed card instances still come from the run; static scenery should not contain duplicate gameplay settlement logic. Node `EffectId` is reserved and does not currently launch a node effect.
8. **Verify:** save assets, stop PIE, restart Play, view the relevant node/home page, then enter combat. Check skeleton/pose compatibility, sockets, scale and frame cost with your actual assets. Keep IDs and the catalog's provenance/version intact. Changing an existing asset does not require re-exporting HTML.

A useful first art set is one home body plus pose, one ACT body/AnimBP, one weapon/socket, two distinguishable C/guard models, a small ground/building kit, five readable skill effects and portrait textures. Dialogue currently displays text/choices only; voice playback, dialogue portraits and cinematic binding need a subsequent presentation integration.

No gameplay Blueprint is required to start this slice. Blueprint work is primarily AnimBPs, montage slots, compatible poses/sockets and authored materials/effects. The screen derives from CommonUI but is composed in native Slate; replacing it with a designer-authored Widget Blueprint requires an explicit view binding step. The current controller does not expose a drop-in WBP skin property. Rules continue through commands when that view is replaced.

## Code map and layering

All code paths below are relative to `Source/DreamOfPadma`. For paired entries, `.h` is under `Public/` and `.cpp` under `Private/`, followed by the listed responsibility path. Implementation-only files show their `Private/` prefix explicitly. Core, Game, Gameplay, Presentation, World and UI are logical folders in the existing `DreamOfPadma` module. Runtime source no longer has an `MVP/` or `Playable/` parent.

| File(s) | Function |
| --- | --- |
| `Core/Cards/PadmaCardMobility.h` / `.cpp` | Card mobility definition and deterministic movement eligibility. |
| `Core/Content/PadmaContentTypes.h` / `.cpp` | Reflected definition/effect rows and `FPadmaContentSnapshot`; validate fields, references and ordering, and query parameters. |
| `Core/Run/PadmaRunRules.h` / `.cpp` | Saveable state and command/preview/result contracts; calendar/resources, piles, deployment, grouped movement, synthesis, dialogue/preparation, seeded RNG and battle commit/rollback. |
| `Game/Content/PadmaContentCatalog.h` / `.cpp` | `UPadmaContentCatalog` typed soft table arrays and art/ACT bindings; load/merge shards into one validated snapshot and resolve assets. |
| `Game/Run/PadmaRunSubsystem.h` / `.cpp` | `UPadmaRunSubsystem` owns the GameInstance-lifetime run service: new run, safe save/load and version/state validation. |
| `Game/Save/PadmaRunSaveGame.h` / `.cpp` | Separate `UPadmaRunSaveGame` envelope and exact current/legacy class-name reader; preserves format/app identity and persisted run values. |
| `Game/Framework/PadmaGameMode.h` / `.cpp` | Separate `APadmaGameMode` declaration and controller-class setup. |
| `Game/Framework/PadmaPlayerController.h` / `.cpp` | `APadmaPlayerController` declarations, transient selection/page state, startup, world/camera/stage composition, input, travel and failure recovery. |
| `Private/Game/Framework/PadmaPlayerControllerUI.cpp` | Build screen views from values; translate button/drag/hold intents into rule/combat commands. |
| `Private/Game/Framework/PadmaPlayerControllerCombat.cpp` | Compose mode-specific participants/definitions, connect payment and settlement, target selection and cosmetic receipts. |
| `Game/Presentation/PadmaPresentationCatalog.h` / `.cpp` | Resolve presentation identities and model/character/weapon references for authored preview content. |
| `Game/Presentation/PadmaNodePreview.h` / `.cpp` | Node scene definitions, preview bindings, preview actor and model-preview GameMode. |
| `Gameplay/Combat/PadmaCombatTypes.h` | Shared battle setup, unit spec/snapshot and source/target receipt contracts; no `.cpp`. |
| `Gameplay/Combat/PadmaCombatAttributes.h` / `.cpp` | GAS health/max health, shield and block attributes; constrain attribute changes. |
| `Gameplay/Combat/PadmaCombatUnit.h` / `.cpp` | Transient combat Character/ASC, attribute effects, presentation and granted-ability cleanup. |
| `Gameplay/Combat/PadmaCombatComponent.h` / `.cpp` | Battle API, Encounter timeline, ACT movement/attacks, basic-card sources, damage, receipts and lifecycle. |
| `Gameplay/Encounter/PadmaEncounterAbility.h` / `.cpp` | Encounter-specific GameplayAbility entry and activation gate. |
| `Gameplay/ACT/Runtime/PadmaACTAbility.h` / `.cpp` | Separate ACT GameplayAbility entry and activation gate. |
| `Gameplay/ACT/Authoring/PadmaACTAuthoring.h` / `.cpp` | ACT character, weapon and skill authoring assets and the separate `FPadmaACTSkillRow` binding schema. |
| `Presentation/Models/PadmaModelPresentation.h` / `.cpp` | Model definitions, loading and scene presentation; model assets are not a second authoritative rule store. |
| `World/Map/PadmaWorldMapActor.h` / `.cpp` | `APadmaWorldMapActor` and `APadmaWorldNodeActor`: hex/graph presentation, geometry, labels and stable node hit identities. |
| `World/Combat/PadmaCombatFeedback.h` / `.cpp` | Cosmetic source-to-target trails, falling impacts and floating results. No rule settlement. |
| `UI/Screens/PadmaGameScreen.h` / `.cpp` | `UPadmaGameScreen` consumes `FPadmaGameView` and emits intents; Slate panels, modal/portrait, long press, drag and blur. No direct run mutation. |

Core stays free of actors, maps, UI and effects, making transaction rules testable without a rendered world. Game is the composition/lifetime boundary. Gameplay owns temporary GAS execution. Presentation, World and UI project state and send intent. Encounter and ACT share neutral combat vocabulary while retaining distinct ability classes and table types. These accepted boundaries remain unchanged; [ADR-0006](../Decisions/ADR-0006-Native-HTML-Slice.md) records the folder-based implementation.

The relevant supporting files are relative to the project root unless marked as source paths:

| Path | Function |
| --- | --- |
| `Source/DreamOfPadma/Private/Tests/MVP/TASK-046/DemoCatalogTest.cpp` | Exported fixture, typed shards, duplicate rejection and atomic catalog publication. |
| `Source/DreamOfPadma/Private/Tests/MVP/TASK-046/RunRulesTest.cpp` | Transactional rules, HTML synthesis fixture, calendar, movement, dialogue, save rejection and rollback. |
| `Source/DreamOfPadma/Private/Tests/MVP/TASK-046/GASCombatTest.cpp` | Native mode/source/lifecycle behavior. |
| `Source/DreamOfPadma/DreamOfPadma.Build.cs` | Built-in GAS, CommonUI/UMG/Slate, procedural geometry and presentation dependencies. |
| `DreamOfPadma.uproject` | Enable the corresponding built-in plugins. |
| `Config/DefaultEngine.ini` | CommonUI viewport class and exact TASK-047 CoreRedirects; retain existing default maps. |
| `Config/DefaultGame.ini` | Existing content-root cook coverage and scoped GameplayCue path. Cook configuration is not proof of packaging. |
| `Scripts/Editor/ExportPlayableData.cjs` | Export the accepted HTML fixture into local JSON for initial authoring. |
| `Scripts/Editor/AuthorPlayableContent.py` | Create typed tables, model/ACT definition assets and catalog; preserve existing content and append only missing generated guard/attack hooks. |
| `Scripts/Editor/AuthorPlayableDemo.py` | Run content authoring, create presentation materials and two maps with native GameMode. Existing maps must have the expected GameMode and are preserved. |
| `Scripts/Migrations/RuntimeNaming047.json` | Exact source/type migration mapping and compatibility identifiers. |
| `Scripts/Editor/MigrateRuntimeTypes.py` | Check the migration manifest and load/re-save existing authored assets with their redirected runtime types; see TASK-047 for execution and evidence. |

## Runtime naming and existing assets

The current catalog class is `UPadmaContentCatalog`; `DA_PlayableCatalog` remains the existing asset's package name. The `/Game/Padma/MVP/Playable` packages, save slot `PadmaDemo046`, app marker `padma-native-demo` and save format version 1 remain compatible. They are serialized identities, not runtime class names. CoreRedirects map exactly eight old classes and 31 old structs to their current types. Existing assets are migrated through Unreal loading/re-saving, without moving their packages or changing gameplay rules.

[Runtime naming](../Architecture/RuntimeNaming.md) defines responsibility-based names and the old-to-current mappings. Demo/MVP/Playable labels remain appropriate for fixture content, test paths and this play guide's delivery description. Migration verification belongs to [TASK-047](../Production/Tasks/TASK-047-Runtime-Naming.md); the authoring commands below create missing Demo assets and do not establish migration acceptance.

## Recreate missing generated Demo assets

Existing generated assets can be edited normally. Scripts are **create/preserve** tools, not a destructive refresh: re-exporting HTML does not overwrite existing authored tables or models. To update a table, edit/reimport that table deliberately and validate its IDs/references.

Close the Editor before a build/commandlet. From the project root in PowerShell, use your installed UE 5.8 path and Node executable:

```powershell
$EngineRoot = 'E:/Epic Games/UE_5.8' # Replace with the installed engine root.
$ProjectRoot = (Get-Location).Path
$ProjectFile = Join-Path $ProjectRoot 'DreamOfPadma.uproject'
$NodeExe = (Get-Command node -ErrorAction Stop).Source # Or assign its absolute path.
& (Join-Path $EngineRoot 'Engine/Build/BatchFiles/Build.bat') DreamOfPadmaEditor Win64 Development $ProjectFile -WaitMutex -NoHotReloadFromIDE
& $NodeExe 'Scripts/Editor/ExportPlayableData.cjs'
& (Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe') $ProjectFile -unattended -nop4 -nosplash -nullrhi -nosound '-EnablePlugins=PythonScriptPlugin,EditorScriptingUtilities' "-ExecutePythonScript=$ProjectRoot/Scripts/Editor/AuthorPlayableDemo.py"
```

Run the authoring step only after the Editor build succeeds. Check its log for the `[TASK-046]` catalog-validation/map-ready messages and absence of Python errors; the process exit code alone does not certify a Python script's success. The scripts do not edit `Content/ThirdParty`, copy Combat plugins or overwrite the project's startup map.
