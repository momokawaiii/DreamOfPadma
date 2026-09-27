# ACT asset layout

Chinese summary: [ACTAssetLayout.zh-CN.md](ACTAssetLayout.zh-CN.md).

## Canonical authoring locations

The experimental root remains `/Game/Sandbox/ACT`. This consolidation is not a
promotion to shipping content and does not change gameplay IDs or action timing.

```text
ACT/
  Character/ChenQianyu/
    AbilitySystem/             character, equipment and catalog DA; skill binding DT
      Abilities/               13 action definitions, including the first combo attack
    Blueprints/                base character and placed-character Blueprint
    Animation/
      AnimBlueprints/          locomotion graph and DefaultSlot
      Sequences/Imported/      original/direct-use animation clips
      Sequences/Runtime/       root-motion and in-place adaptations
      Montages/                action/weapon timelines and AN/ANS
    Art/
      Meshes/ Skeletons/ Materials/ Textures/
      Equipment/               offhand sword and scabbard meshes/materials/textures
      Niagara/
        Systems/ Materials/ Textures/ Meshes/ Skeletons/
    Presentation/Camera/       camera definition
  Weapon/Fuyao/
    AbilitySystem/             existing weapon presentation definition
    Art/Meshes/ Materials/ Textures/
  Training/
    Maps/L_ChenACT
    Environment/ Calibration/
    WoodenDummy/Blueprints/ Art/ Animation/
```

`BladeContact` under Niagara resource-type folders distinguishes the five original
blade-contact systems and their resources from independently authored action FX.
Some names overlap; they were not proven semantically interchangeable. Materials,
texture settings and emitters are retained rather than overwritten by filename.
Likewise Imported and Runtime animation variants are not redundant: root-motion
and root-lock configuration differs.

The old `Actions`, `Attack01`, `Centimeter`, `Weapons` and `Rendering` organization
is retired. The first attack (`Chen.Attack01`) remains part of the canonical skill
table. `_CM` asset suffixes retain import identity/scale provenance, not a second
character implementation. The old single-attack character, melee profile, slots
table, Montage and standalone preview Blueprint are archived. Regression fixtures
are transient C++ objects built from the canonical shared assets.

## What designers edit

- `AbilitySystem/DA_ACTCharacter_CHEN`: character-level animation, movement, camera,
  equipment and skill table references.
- `AbilitySystem/DT_ACTSkills_CHEN`: stable SkillId, input binding and skill DA.
- `AbilitySystem/Abilities/DA_Chen_*`: existing cost/cooldown/damage/transition fields.
- `Animation/Montages`: sections, hit windows, cancel/combo windows and effects.
- Placed Blueprint instances in `Training/Maps/L_ChenACT`: appearance and layout,
  visible before PIE. C++ still owns combat rules and runtime lifecycle.

The proposed ACT Table/Character, Table/Weapon, non-A-card and Buff catalogs are
not fabricated as empty assets. Existing production catalogs remain in
`/Game/Padma/MVP/Playable/Definitions/Tables`, outside Sandbox. General Buff grants,
fields, projectiles and summons need schema/runtime work before they can be
truthfully advertised as table-only authoring. Reserve those roles when implemented;
do not create a second authoritative identity table just to populate a folder.

## Audit, migration and recovery

`Artifacts/ACTFinalLayout/AssetLedger.md` and `plan.json` cover every one of the
1,427 original packages: original/new path, class, purpose, disposition, properties
and hard/soft referencers/dependencies. `before-migration.zip` contains the original
Content files, validated against SHA-256 hashes. Artifacts is evidence/recovery only;
no runtime asset references it.

Scripts under `Scripts/Editor`:

- `AuditACTAssets.py`: UE read-only original inventory.
- `PlanACTLayout.py`: offline Python plan/ledger generation; no Content writes.
- `RewriteACTLayoutReferences.py`: one-shot mechanical text migration, not a daily recipe.
- `MigrateACTLayout.py`: reviewed-plan AssetTools rename; explicit
  `-PadmaApplyACTLayout`, unchanged-source checks and verified backup required.
- `FinalizeACTLayout.py`: guarded redirector/fixture retirement. Always confirm disk state.
- `CleanupACTMigrationResidue.py`: read-only reference/hash manifest for interrupted
  old physical copies; it never deletes files. The completed run quarantined these
  copies under `Artifacts/ACTFinalLayout/InterruptedResidue` after closing UE.
- `VerifyACTLayout.py`: fresh-process per-package/reference/property comparison,
  scene reload and daily authoring preflight. Run with `-PadmaValidateOnly`.

Use `UnrealEditor-Cmd <uproject> -run=pythonscript -script=<absolute script path>
-unattended -nop4 -NullRHI -NoShaderCompile` for asset checks, adding the explicit
flag required by that script. These flags do not prove visual/PIE acceptance.
Keep the editor closed during migration/build and only one UE execution lane.
The process-local viewport-history cleanup never saves user configuration.

For recovery, close UE and inspect the original-to-final manifest. Restore only
the intended packages from the verified zip and reverse their references through
Unreal; do not unzip the entire old tree into a working canonical tree, which would
reintroduce duplicate active libraries. Preserve newer user material/level edits.

Daily Content-only scripts are `ConfigureChenCombatIdle.py`,
`ConfigureChenWeaponLifecycle.py`, `BindChenActionFX.py` and scene configuration.
Offline extraction/reconstruction scripts may still require explicitly supplied
source files; they are not runtime dependencies. `AuthorChenACTActions.py` requires
`-PadmaReseedChenActions` because reseeding replaces editable skill tuning.

## Accepted verification

The canonical disk set is 1,422 packages, with no old-directory assets or
redirectors. Fresh-process loading, ACT dependency-edge mapping and recorded
properties passed; the placed scene and three Content-only authoring preflights
passed. Editor build, Python syntax, AuditDocs and strict project validation passed.
All seven targeted D3D12 regressions passed under `Artifacts/ACTFinalLayout/TestsFinal`:
AnimatedBladeContact, ChenSavedMaterialBindings, ChenTrainingAnalytics,
ChenWeaponStow, SceneMapPIE, SceneParticipants and WoodenTrainingDummy.
The first run found an unplayable nested duplicate dynamic Montage in the new
transient edge fixture; constructing a separate rooted fixture fixed it without
changing gameplay code or weakening its assertions. This is targeted migration
acceptance, not full-action-suite, visual-parity or cook/package acceptance.
