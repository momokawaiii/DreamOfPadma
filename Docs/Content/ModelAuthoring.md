# Models and Local Scene Authoring

- Chinese companion: ModelAuthoring.zh-CN.md
- Document ID: CONTENT-MODEL-AUTHORING-001
- Version: 0.2
- Status: Generic/ACT model preview; legacy Demo sources retired by TASK-048
- Owner: TASK-040

## What is available in UE

Open `/Game/Padma/MVP/Presentation/ModelPreview/L_ModelAuthoringPreview` in the Content Browser. Select **Home - Model Authoring Preview** and press **Rebuild Preview** in Details, or start PIE. The dedicated preview GameMode shows a fixed oblique camera with no player Pawn. The map contains a garrison placeholder, a building cube and a ground plane.

Runtime source paths are relative to `Source/DreamOfPadma/{Public,Private}`: `Presentation/Models/PadmaModelPresentation.h/.cpp`, `Game/Presentation/PadmaPresentationCatalog.h/.cpp` and `Game/Presentation/PadmaNodePreview.h/.cpp`. TASK-047 removes the source `MVP/` parent while preserving the existing preview content package paths and fixture names. See [Runtime naming](../Architecture/RuntimeNaming.md).

This preview isolates model/slot authoring. Use [NativePlayableDemo](NativePlayableDemo.md) for current map/movement/Encounter/ACT runtime and [WorldMapAuthoring](WorldMapAuthoring.md) for map editing and PCG themes.

| Asset in the same ModelPreview folder | Purpose |
|---|---|
| DA_Model_Character_Template | Empty general character-model draft to duplicate and fill |
| DA_Model_Garrison_Placeholder | Generic garrison sphere; no skill source |
| DA_Model_Building_Placeholder | General static model definition using an engine cube |
| DA_Model_Ground_Placeholder | General static model definition using an engine plane |
| DA_ModelPreview_Catalog | Domain-qualified definition keys and typed soft source references |
| DA_NodeScene_Preview | Static scene level reference and named slot transforms |

The garrison fixture uses `/Game/Padma/MVP/Presentation/ModelPreview/DA_Model_Garrison_Placeholder`, key `Model / Visual.Preview.Garrison`, with no skill rows. TASK-048 retired the old Demo character and three-skill adapter; historical TASK-040 logs describe the former fixture.

## Fill a model and its source

Create or duplicate a **PadmaModelDefinition** Data Asset for general characters, buildings, terrain or props. Set a stable VisualDefinitionId, DisplayName, and exactly one of StaticModel or SkeletalModel. AnimationClass is optional and must be compatible with the skeletal model; ModelTransform adjusts presentation only. Both model references are soft references.

For an ACT character, fill the existing **PadmaACTCharacterDefinition**: Model, optional AnimationClass and its own ACT SkillTable. For a weapon use **PadmaACTWeaponDefinition**. Follow [ACTAuthoring](ACTAuthoring.md) for typed rows and skill assets. These remain the single ACT model/skill source; do not duplicate their authority in a general model asset.

A general model asset carries no skills. Game resolves Model, ACTCharacter or ACTWeapon into a neutral presentation descriptor. The reusable model Actor remains independent of battle-mode types; DemoCharacter is retired.

## Bind the selected identity to a scene slot

For the user's Home/garrison example, the intended production chain is:

```text
World garrison state: NodeId + EntityInstanceId + domain/DefinitionId
    -> catalog entry: a typed soft reference to that definition Data Asset
    -> the selected definition's model and its own skill source
    -> a temporary scene Actor at the named Garrison slot
```

World state and SaveGame keep stable identities. The catalog keeps soft asset references. The live Actor pointer exists only while rendering the scene and is not saved as gameplay state. Switching garrison characters will be a validated service command producing an event; TASK-040 only implements the read-only resolution and preview end of this chain.

In **DA_ModelPreview_Catalog**, add an entry with a Domain and DefinitionId matching the referenced source. Fill exactly the corresponding Model, ACTCharacter or ACTWeapon field. Unknown/duplicate keys and extra sources fail validation; resolution never silently chooses the first entry.

In **DA_NodeScene_Preview**, configure Slots with unique SlotId values and transforms. This is static layout; do not put current ownership or occupants here.

On the preview host, PreviewBindings associate each SlotId with a unique illustrative EntityInstanceId and a domain-qualified definition key. SetPreviewSelection is a preview-only API; it clears the previous SelectedSkillRow when switching definitions. After editing Details, press Rebuild Preview. An invalid new selection clears the old model/skills and reports Diagnostics. Invalid layout does not produce a partially populated candidate. The generated presenters are transient and are rebuilt for PIE.

## Readiness and diagnostics

The resolved view exposes VisualReady and SkillDetailsReady independently. A model may render while its table is missing. A wrong ACT row structure, missing selected row, mismatched skill identity, duplicate binding or incompatible animation produces a source-specific diagnostic. An empty ACT description stays empty; there is no fallback to Encounter text or effects.

Neither readiness flag proves a GAS executor is registered or a battle is playable. The preview performs synchronous soft-reference loading only on explicit rebuild/selection or BeginPlay; runtime streaming and scene transition ownership are later integration work.

## Mobility metadata

Current native card rows and ACT character definitions contain Mobility. After confirming the policy, set Configured and choose Movable, Immovable or Conditional. Conditional requires ConditionId; other policies leave it empty. Unconfigured data fails validation.

The shared type is consumed by native card/run rules and separate ACT authoring. It does not make ACT characters sandbox-deployable. Current movement behavior and data limits are in [NativePlayableDemo](NativePlayableDemo.md).

Accepted movement behavior: click only inspects; the right-side Move Here button requests movement of selected card instances. A and all attached B cards form a group, with each card checked. Any failure rejects the whole group before movement or payment. Current values are the accepted native fixture, not final balance. Enemy node information is fully public in the first MVP; FPS is future scope.

## Reproduce and inspect

Run `Scripts/Editor/AuthorModelPreview.py` after building DreamOfPadmaEditor. It creates missing general-model preview assets, preserves existing content and validates the saved preview on rerun. No TASK-008 fixture or retired Demo authoring script is required. Logs retain the historical `[TASK-040]` prefix.

Automation: `DreamOfPadma.MVP.TASK040.ModelAndSkillResolution` and `DreamOfPadma.MVP.TASK040.NodeBindingReplacement`. The task completion report records build, rerun, negative-case and real UE render evidence.

## Small learning exercise

Predict what changes if only the model soft reference changes: the image should change while the definition identity and its skill source remain the same. Duplicate a template, assign an imported model, give it a new stable identity, bind it to the preview slot and rebuild. Then select an invalid identity and check that stale content is cleared. Explain the difference between instance identity, definition identity, soft asset reference and a live Actor pointer. Finally apply the same process to a building slot.

The user has not yet supplied prediction, practice or teach-back evidence; automated implementation checks do not substitute for that learning evidence.
