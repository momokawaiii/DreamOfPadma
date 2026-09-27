# Chen ACT Authoring

- Chinese companion: [ACTAuthoring.zh-CN.md](ACTAuthoring.zh-CN.md)
- Document ID: CONTENT-ACT-AUTHORING-001
- Version: 1.0
- Updated: 2026-09-27
- Status: Retained Chen ACT authoring path; generic template assets are retired

## Retained assets

Use the existing Chen assets under `Content/Sandbox/ACT/Character/ChenQianyu`:

| Asset group | Role |
|---|---|
| `DA_ACTCatalog_CHEN`, `DA_ACTCharacter_CHEN`, `DA_Chen_Equipment` | Stable character, roster and weapon definitions |
| `DT_ACTSkills_CHEN` and `AbilitySystem/Abilities/DA_Chen_*` | Input/action identity and native GAS action data |
| `Animation/AnimBlueprints/ABP_ACT_CHEN`, Montages and sequences | Animation playback and Montage timing |
| `Blueprints/BP_ChenACTPlaced`, `BP_ACT_CHEN` | Scene-owned/editor and runtime character presentation |
| `Training/Maps/L_ChenACT` and `Training/WoodenDummy` | Retained training scene and target fixtures |

Do not recreate the retired `Content/Padma/MVP/Definitions/ACTCharacterCards` template folder. The `MVP/Playable` assets belong to the retained turn-based path; they are not a second Chen ACT catalog.

## Authoring rules

Keep stable IDs independent from display names and filenames. Character, ABP, Montage and weapon assets must use compatible skeletons and explicit sockets. Montage sections and AN/ANS timing own action windows; native GAS owns activation, commit, cancellation and teardown. Do not add damage, cost, cooldown or source-parity claims merely because an asset loads.

The `L_ChenACT` map owns one `BP_ChenACTPlaced` and three `BP_ACTWoodenDummy` participants. Native combat may borrow and reset them; it must not silently replace the saved scene layout. Scene ownership and runtime borrowing are defined in [ADR-0012](../Decisions/ADR-0012-Scene-Owned-ACT-Participants.md).

## Validation

Use the Chen authoring scripts under `Scripts/Editor` only with the Editor closed for resource writes and one UE process at a time. Relevant checks include:

- `Scripts/Editor/AuditACTAssets.py` and `VerifyACTLayout.py` for path/layout/reference inventory.
- `Scripts/Editor/VerifyChenSceneParticipants.py` for the saved map participants.
- The `DreamOfPadma.ACT.*` automation tests for action, camera, weapon, target and analytics contracts.
- [Chen ACT/render lab](ChenACTRenderLab.md) for scene, camera, material and manual visual checks.

Static authoring or a successful build does not prove PIE behavior, GPU output, full source visual parity or packaged play. Known timing/visual limits remain in [ProjectState](../ProjectState.md) and [TASK-056](../Production/Tasks/TASK-056-Chen-ACT-Actions.md).

## Related contracts

- [ACT development contract](../Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md)
- [ACT migration matrix](ACTMigrationMatrix.md)
- [Chen asset layout](ACTAssetLayout.md)
- [PadmaNPR character binding](../../Plugins/PadmaNPR/Character.md)
