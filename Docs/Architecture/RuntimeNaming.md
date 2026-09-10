# Runtime Responsibility Naming

- Chinese companion: [RuntimeNaming.zh-CN.md](RuntimeNaming.zh-CN.md)
- Document ID: ARCH-RUNTIME-NAMING
- Version: 0.1
- Status: Current naming convention; migration verification is tracked in TASK-047
- Delivery and evidence: [TASK-047](../Production/Tasks/TASK-047-Runtime-Naming.md)
- Related implementation: [Native playable guide](../Content/NativePlayableDemo.md), [ADR-0006](../Decisions/ADR-0006-Native-HTML-Slice.md)

## Purpose and scope

Reusable runtime code is named for its responsibility and domain. `Demo`, `Playable` and `MVP` describe a delivery or fixture stage; they do not identify a card schema, run service, controller or screen. TASK-047 applies this convention to the native runtime source formerly under `Source/DreamOfPadma/{Public,Private}/MVP`.

This is an implementation naming and file-organization change within the accepted Core/Game/Gameplay/Presentation/World/UI boundaries. It does not add a UE module, change dependency direction, change gameplay, rename serialized fields or redefine rules. Legacy `Source/DreamOfPadma/{Public,Private}/Demo` fixtures remain outside this migration.

## Name the domain and role

Use Unreal's type prefix followed by `Padma`, the domain and its role, for example `FPadmaCardDefinitionRow`, `FPadmaCardInstance` and `UPadmaRunSubsystem`. Keep the mode when semantics belong to that mode: Encounter and ACT remain distinct. File names omit the Unreal `F/U/A/E` prefix and identify their principal type or coherent responsibility.

| Suffix or role | Meaning and current example |
| --- | --- |
| `DefinitionRow` / `Definition` | Authored, static identity/configuration. `FPadmaCardDefinitionRow` defines a card; `UPadmaModelDefinition` describes model references. Neither is the mutable card in a run. |
| `ParameterRow`, `EffectRow`, `LineRow` | More specific authored schemas when the row represents a balance parameter, supported effect or dialogue line: `FPadmaBalanceParameterRow`, `FPadmaACTSkillEffectRow`, `FPadmaDialogueLineRow`. |
| `State` / `Instance` | Mutable run values or one entity identity: `FPadmaRunState`, `FPadmaWorldNodeState`, `FPadmaCardInstance`. Store stable IDs rather than Actor pointers. |
| `Snapshot` / `View` | Value data for a defined read boundary: `FPadmaContentSnapshot` is a validated content snapshot; `FPadmaGameView` projects state for the screen. These names do not grant mutation authority. |
| `Catalog` | Collect and resolve authored entries/references: `UPadmaContentCatalog`. It does not own the current run or settle commands. |
| `Rules` | Validate and execute domain commands using state and definitions: `FPadmaRunRules`. |
| `Subsystem` | Explicit Unreal lifetime service: `UPadmaRunSubsystem` owns the GameInstance-lifetime run and persistence entry points. |
| `SaveGame` | Versioned persistence envelope: `UPadmaRunSaveGame`. Keep it separate from the service that saves or loads it. |
| `GameMode` / `PlayerController` | Unreal framework role: `APadmaGameMode` sets controller configuration; `APadmaPlayerController` composes play, input and views through existing services. |
| `Actor` / `Screen` | Scene or UI presentation: `APadmaWorldNodeActor`, `APadmaWorldMapActor`, `UPadmaGameScreen`. Send intents and consume values/receipts; presentation does not settle gameplay. |

The existing `FPadmaACTSkillRow` is the ACT authoring binding row (skill ID, slot and skill asset reference). `FPadmaACTSkillEffectRow` is the separate supported-effect/number schema. Do not merge these names or ownership roles merely because both concern ACT skills.

## Source locations and includes

The module remains `Source/DreamOfPadma`. Public headers expose contracts; private source implements those contracts. Keep the responsibility path mirrored under `Public/` and `Private/` where both files exist. Include from the public root, for example `#include "Game/Framework/PadmaGameMode.h"`; do not put `Public/`, `Private/` or a delivery-stage folder in a public include path. A reflected header uses its own matching `.generated.h` name.

| Directory | Responsibility |
| --- | --- |
| `Core/Content`, `Core/Run`, `Core/Cards` | Definition rows, value snapshots, mutable run contracts and deterministic rules; no Actor, UMG, Niagara, map or UGameplayStatics dependency. |
| `Game/Content`, `Game/Run`, `Game/Save` | Authored catalog loading, run lifetime and persistence envelope. |
| `Game/Framework` | GameMode and PlayerController composition. The controller's UI and combat implementation files remain parts of the same controller. |
| `Game/Presentation`, `Presentation/Models` | Preview composition/catalogs and reusable model definitions/presentation. |
| `Gameplay/Combat`, `Gameplay/Encounter`, `Gameplay/ACT` | Combat execution/contracts and separate mode abilities and authoring. |
| `World/Map`, `World/Combat` | World-map/node actors and cosmetic combat feedback. |
| `UI/Screens` | Game screen, value views and user intents. |

### TASK-047 file mapping

Paths below are relative to `Source/DreamOfPadma`. This lists each of the 39 moved files. The machine-readable mapping is [RuntimeNaming047.json](../../Scripts/Migrations/RuntimeNaming047.json).

| Previous file | Current file |
| --- | --- |
| `Private/MVP/Core/Cards/PadmaCardMobility.cpp` | `Private/Core/Cards/PadmaCardMobility.cpp` |
| `Private/MVP/Core/Content/PadmaDemoContent.cpp` | `Private/Core/Content/PadmaContentTypes.cpp` |
| `Private/MVP/Core/Run/PadmaDemoRunRules.cpp` | `Private/Core/Run/PadmaRunRules.cpp` |
| `Private/MVP/Game/Content/PadmaDemoCatalog.cpp` | `Private/Game/Content/PadmaContentCatalog.cpp` |
| `Private/MVP/Game/Playable/PadmaBattleComposition.cpp` | `Private/Game/Framework/PadmaPlayerControllerCombat.cpp` |
| `Private/MVP/Game/Playable/PadmaPlayableGame.cpp` | `Private/Game/Framework/PadmaPlayerController.cpp` |
| `Private/MVP/Game/Playable/PadmaPlayableInterface.cpp` | `Private/Game/Framework/PadmaPlayerControllerUI.cpp` |
| `Private/MVP/Game/Presentation/PadmaNodePreview.cpp` | `Private/Game/Presentation/PadmaNodePreview.cpp` |
| `Private/MVP/Game/Presentation/PadmaPresentationCatalog.cpp` | `Private/Game/Presentation/PadmaPresentationCatalog.cpp` |
| `Private/MVP/Game/Run/PadmaDemoRunSubsystem.cpp` | `Private/Game/Run/PadmaRunSubsystem.cpp` |
| `Private/MVP/Gameplay/Combat/PadmaCombatAttributes.cpp` | `Private/Gameplay/Combat/PadmaCombatAttributes.cpp` |
| `Private/MVP/Gameplay/Combat/PadmaCombatComponent.cpp` | `Private/Gameplay/Combat/PadmaCombatComponent.cpp` |
| `Private/MVP/Gameplay/Combat/PadmaCombatUnit.cpp` | `Private/Gameplay/Combat/PadmaCombatUnit.cpp` |
| `Private/MVP/Gameplay/Encounter/PadmaEncounterAbility.cpp` | `Private/Gameplay/Encounter/PadmaEncounterAbility.cpp` |
| `Private/MVP/Gameplay/ACT/Authoring/PadmaACTAuthoring.cpp` | `Private/Gameplay/ACT/Authoring/PadmaACTAuthoring.cpp` |
| `Private/MVP/Gameplay/ACT/Runtime/PadmaACTAbility.cpp` | `Private/Gameplay/ACT/Runtime/PadmaACTAbility.cpp` |
| `Private/MVP/Presentation/Models/PadmaModelPresentation.cpp` | `Private/Presentation/Models/PadmaModelPresentation.cpp` |
| `Private/MVP/UI/Playable/PadmaPlayableScreen.cpp` | `Private/UI/Screens/PadmaGameScreen.cpp` |
| `Private/MVP/World/Playable/PadmaCombatFeedback.cpp` | `Private/World/Combat/PadmaCombatFeedback.cpp` |
| `Private/MVP/World/Playable/PadmaPlayableWorld.cpp` | `Private/World/Map/PadmaWorldMapActor.cpp` |
| `Public/MVP/Core/Cards/PadmaCardMobility.h` | `Public/Core/Cards/PadmaCardMobility.h` |
| `Public/MVP/Core/Content/PadmaDemoContent.h` | `Public/Core/Content/PadmaContentTypes.h` |
| `Public/MVP/Core/Run/PadmaDemoRunRules.h` | `Public/Core/Run/PadmaRunRules.h` |
| `Public/MVP/Game/Content/PadmaDemoCatalog.h` | `Public/Game/Content/PadmaContentCatalog.h` |
| `Public/MVP/Game/Playable/PadmaPlayableGame.h` | `Public/Game/Framework/PadmaPlayerController.h` |
| `Public/MVP/Game/Presentation/PadmaNodePreview.h` | `Public/Game/Presentation/PadmaNodePreview.h` |
| `Public/MVP/Game/Presentation/PadmaPresentationCatalog.h` | `Public/Game/Presentation/PadmaPresentationCatalog.h` |
| `Public/MVP/Game/Run/PadmaDemoRunSubsystem.h` | `Public/Game/Run/PadmaRunSubsystem.h` |
| `Public/MVP/Gameplay/Combat/PadmaCombatAttributes.h` | `Public/Gameplay/Combat/PadmaCombatAttributes.h` |
| `Public/MVP/Gameplay/Combat/PadmaCombatComponent.h` | `Public/Gameplay/Combat/PadmaCombatComponent.h` |
| `Public/MVP/Gameplay/Combat/PadmaCombatTypes.h` | `Public/Gameplay/Combat/PadmaCombatTypes.h` |
| `Public/MVP/Gameplay/Combat/PadmaCombatUnit.h` | `Public/Gameplay/Combat/PadmaCombatUnit.h` |
| `Public/MVP/Gameplay/Encounter/PadmaEncounterAbility.h` | `Public/Gameplay/Encounter/PadmaEncounterAbility.h` |
| `Public/MVP/Gameplay/ACT/Authoring/PadmaACTAuthoring.h` | `Public/Gameplay/ACT/Authoring/PadmaACTAuthoring.h` |
| `Public/MVP/Gameplay/ACT/Runtime/PadmaACTAbility.h` | `Public/Gameplay/ACT/Runtime/PadmaACTAbility.h` |
| `Public/MVP/Presentation/Models/PadmaModelPresentation.h` | `Public/Presentation/Models/PadmaModelPresentation.h` |
| `Public/MVP/UI/Playable/PadmaPlayableScreen.h` | `Public/UI/Screens/PadmaGameScreen.h` |
| `Public/MVP/World/Playable/PadmaCombatFeedback.h` | `Public/World/Combat/PadmaCombatFeedback.h` |
| `Public/MVP/World/Playable/PadmaPlayableWorld.h` | `Public/World/Map/PadmaWorldMapActor.h` |

Four additional files isolate framework/save responsibilities and implement save compatibility:

- `Public/Game/Framework/PadmaGameMode.h`: `APadmaGameMode`, extracted from the old `Public/MVP/Game/Playable/PadmaPlayableGame.h`.
- `Private/Game/Framework/PadmaGameMode.cpp`: GameMode setup, extracted from the old `Private/MVP/Game/Playable/PadmaPlayableGame.cpp`.
- `Public/Game/Save/PadmaRunSaveGame.h`: `UPadmaRunSaveGame`, extracted from the old `Public/MVP/Game/Run/PadmaDemoRunSubsystem.h`.
- `Private/Game/Save/PadmaRunSaveGame.cpp`: current/legacy envelope reader required by the save-class rename.

`PadmaPlayerController.cpp`, `PadmaPlayerControllerUI.cpp` and `PadmaPlayerControllerCombat.cpp` implement the one controller declared in `PadmaPlayerController.h`; their filenames distinguish composition, UI and combat responsibilities. `PadmaContentTypes.h` intentionally groups the related content row contracts and snapshot rather than pretending that its contents are one mutable object.

## Reflected type migration

`Config/DefaultEngine.ini` uses exact CoreRedirects for eight classes and 31 structs. All entries are within `/Script/DreamOfPadma`; Unreal reflection names omit the C++ `A/U/F` prefix. For example, the first class row maps `/Script/DreamOfPadma.PadmaDemoCatalog` to `/Script/DreamOfPadma.PadmaContentCatalog`. The manifest contains every full path; there are no substring or package-wide redirects.

### Classes: 8

| Previous C++ type | Current C++ type |
| --- | --- |
| `UPadmaDemoCatalog` | `UPadmaContentCatalog` |
| `UPadmaDemoSaveGame` | `UPadmaRunSaveGame` |
| `UPadmaDemoRunSubsystem` | `UPadmaRunSubsystem` |
| `APadmaPlayableGameMode` | `APadmaGameMode` |
| `APadmaPlayableController` | `APadmaPlayerController` |
| `APadmaPlayableHex` | `APadmaWorldNodeActor` |
| `APadmaPlayableWorld` | `APadmaWorldMapActor` |
| `UPadmaPlayableScreen` | `UPadmaGameScreen` |

### Structs: 31

| Previous C++ type | Current C++ type |
| --- | --- |
| `FPadmaDemoCardRow` | `FPadmaCardDefinitionRow` |
| `FPadmaDemoNodeRow` | `FPadmaWorldNodeDefinitionRow` |
| `FPadmaDemoEdgeRow` | `FPadmaWorldEdgeDefinitionRow` |
| `FPadmaDemoProfileRow` | `FPadmaBalanceParameterRow` |
| `FPadmaDemoACTCharacterRow` | `FPadmaACTCharacterDefinitionRow` |
| `FPadmaDemoACTWeaponRow` | `FPadmaACTWeaponDefinitionRow` |
| `FPadmaDemoDisplayRow` | `FPadmaAppearanceDefinitionRow` |
| `FPadmaDemoFPSCharacterRow` | `FPadmaFPSCharacterDefinitionRow` |
| `FPadmaDemoFPSWeaponRow` | `FPadmaFPSWeaponDefinitionRow` |
| `FPadmaDemoSkillRow` | `FPadmaSkillEffectRowBase` |
| `FPadmaDemoEncounterSkillRow` | `FPadmaEncounterSkillEffectRow` |
| `FPadmaDemoACTSkillRow` | `FPadmaACTSkillEffectRow` |
| `FPadmaDemoDialogueChoice` | `FPadmaDialogueChoice` |
| `FPadmaDemoDialogueRow` | `FPadmaDialogueLineRow` |
| `FPadmaDemoCalendar` | `FPadmaCalendarState` |
| `FPadmaDemoResources` | `FPadmaResourceState` |
| `FPadmaDemoRunCard` | `FPadmaCardInstance` |
| `FPadmaDemoRunNode` | `FPadmaWorldNodeState` |
| `FPadmaDemoMobilityPolicy` | `FPadmaCardMobilityPolicy` |
| `FPadmaDemoPreparation` | `FPadmaPreparationState` |
| `FPadmaDemoSynthesisCandidate` | `FPadmaSynthesisCandidate` |
| `FPadmaDemoSynthesisRecord` | `FPadmaSynthesisRecord` |
| `FPadmaDemoRunLog` | `FPadmaRunLogEntry` |
| `FPadmaDemoBattleRecord` | `FPadmaBattleRecord` |
| `FPadmaDemoRunState` | `FPadmaRunState` |
| `FPadmaDemoModelBinding` | `FPadmaModelBinding` |
| `FPadmaDemoEffectBinding` | `FPadmaEffectBinding` |
| `FPadmaDemoNodePresentationBinding` | `FPadmaNodePresentationBinding` |
| `FPadmaDemoACTCharacterBinding` | `FPadmaACTCharacterBinding` |
| `FPadmaDemoACTWeaponBinding` | `FPadmaACTWeaponBinding` |
| `FPadmaDemoACTSkillBinding` | `FPadmaACTSkillBinding` |

### Non-reflected contracts: 9

These C++ contracts are renamed at their callers; they are not additional reflected redirects.

| Previous C++ type | Current C++ type |
| --- | --- |
| `FPadmaDemoContent` | `FPadmaContentSnapshot` |
| `FPadmaDemoRunRules` | `FPadmaRunRules` |
| `EPadmaDemoCommandEffect` | `EPadmaRunCommandEffect` |
| `FPadmaDemoCommandResult` | `FPadmaRunCommandResult` |
| `FPadmaDemoMoveCheck` | `FPadmaCardMoveCheck` |
| `FPadmaDemoMovePreview` | `FPadmaCardMovePreview` |
| `FPadmaDemoPendingBattle` | `FPadmaBattleRequest` |
| `FPadmaDemoDialogue` | `FPadmaDialogueState` |
| `FPadmaPlayableView` | `FPadmaGameView` |

## Serialized compatibility and fixture names

The existing `/Game/Padma/MVP/Playable` package root, including `DA_PlayableCatalog` and the two maps, remains unchanged. The catalog asset is now an instance of `UPadmaContentCatalog`; its package name is not its native class name. Keep stable gameplay IDs, serialized property names, save slot `PadmaDemo046`, app marker `padma-native-demo` and save format version 1 intact.

`Scripts/Editor/MigrateRuntimeTypes.py` checks the manifest and loads/re-saves existing authored assets with the redirected types through Unreal. Loading and re-saving must preserve authored values and package locations. Execution, old-save compatibility, asset/map checks and runtime verification belong in TASK-047; neither a redirect entry nor this document establishes that a migration check passed.

`Private/Game/Save/PadmaRunSaveGame.cpp` additionally implements the save-class compatibility reader. UE's generic `LoadGameFromMemory` does not apply class redirects to the stored string. The run service reads bytes through this typed reader: UE parses the GVAS header, its final serialized class name must match the exact current or legacy envelope, and the unchanged payload is read with UE's property archive. Existing app/version/state validation still applies; loading does not rewrite the slot.

After closing the Editor and rebuilding, run asset migration from the project root with the project's UE installation:

```powershell
& 'E:/Epic Games/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/2026ue/DreamOfPadma/DreamOfPadma.uproject' -unattended -nop4 -nullrhi -ExecutePythonScript='E:/2026ue/DreamOfPadma/Scripts/Editor/MigrateRuntimeTypes.py'
```

The script backs up existing fixture packages to ignored `Artifacts/TASK-047/content-before`, validates rows and map GameMode references, then re-saves in place and checks row-type picker tags. It never relocates content packages.

Delivery-stage words remain valid for historical TASK descriptions, this Demo play guide, fixture content roots, `Private/Tests/MVP/TASK-046`, legacy Demo fixtures and their authoring scripts. They must not return as names for reusable runtime types, source files or responsibility directories. Fixture names and preserved serialized identifiers are deliberate exceptions to the runtime naming convention.
