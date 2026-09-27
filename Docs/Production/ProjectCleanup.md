# Retained Project Scope

Updated 2026-09-27. The user retained **the turn-based maps, the Chen ACT scene and PadmaNPR**. This is a repository cleanup boundary, not Shipping acceptance.

## Runtime maps

- `/Game/Padma/MVP/Playable/Maps/L_PadmaWorld`: main map, menus, strategy, cards, preparation, divination and their supporting services.
- `/Game/Padma/MVP/Playable/Maps/L_PadmaBattle`: main-map battle dependency; removing it would break existing travel.
- `/Game/Sandbox/ACT/Training/Maps/L_ChenACT`: Chen actions, wooden targets, analytics, hologram, camera and render study.
- `Plugins/PadmaNPR`: the retained runtime/editor NPR implementation, profiles, materials, shaders and authoring tools used by the character path.

Shared code, data, fonts, materials, authoring tools, tests and Chen reconstruction inputs remain where these entry points depend on them. Chen paths stay unchanged to preserve serialized and dynamic references. This cleanup does not turn reference-derived assets or temporary balance into approved release content.

## Removed material

The standalone model/map/Attack01 preview levels, unused earlier divination assets, unused source UI textures, HTML prototype application, superseded migration/preview scripts, generic ACT templates, TASK-001 through TASK-053, and old History are retired. Selected obsolete backup/audit output is removed as well. Old task links now lead here; retired records are not current contracts. The `MVP/Playable` directory is retained because it contains the current turn-based maps and their dependencies; its historical name is not itself a deletion rule.

The cleanup originally created and verified `E:/2026ue/DreamOfPadma_Cleanup_20260922.zip`. The user subsequently explicitly requested deletion of rollback backups on 2026-09-22. That archive and identified rollback copies in Artifacts were permanently deleted, without creating another backup. Original manifests remain audit records only and cannot restore deleted bytes. Conversion projects, source inputs, export deliverables and live Content were not deleted by this follow-up.

Detailed dependency inventory, removal manifest and verification output live in `Artifacts/ProjectCleanup`. Asset decisions use UE package references plus native/dynamic-load inspection; an Asset Registry orphan alone is not grounds for deletion.

## Maintenance

- Main-map authoring data is now `Scripts/Data/PlayableData.cjs`; the export command remains `node Scripts/Editor/ExportPlayableData.cjs --check`. The data bytes and historical provenance remain unchanged.
- `AuthorWorldMaps.py` authors definitions without creating a separate preview map. Edit node positions in the map definition and verify in L_PadmaWorld.
- `AuthorChenACTActions.py` updates the existing L_ChenACT; it no longer copies a retired Attack01 map.
- Packaging lists the three retained maps and explicitly includes the runtime UI, Chen ACT and PadmaNPR directories for code-loaded resources. Cook/package acceptance is still separate.
- Generated engine folders are not source content and were not manually cleaned. Chen extraction/reconstruction inputs remain because current authoring scripts consume them.

See [ProjectState](../ProjectState.md) for current entry points and open acceptance.

## Verification

- The historical cleanup manifest recorded 31 removed UE packages and 1,488 non-asset files (about 669 MiB before compression); rollback archives were later deleted at the user's request.
- Current disk inspection has exactly the three retained project maps. A fresh UE reference scan, build and runtime pass must be recorded again after the current Chen and PadmaNPR native changes; older reports are not reused as current proof.
- Existing Chen action failures and visual/source-parity limits remain tracked in [ProjectState](../ProjectState.md) and [TASK-056](Tasks/TASK-056-Chen-ACT-Actions.md). They are not hidden by cleanup.
- Complete manual playthrough, Cook/package and clean-install acceptance remain unverified.

Evidence: `Artifacts/ProjectCleanup/verified.json`, `Tests/index.json`, `Baseline/index.json`, `backup.json`, and `plan.json`.
