# ACT Migration Matrix

- Document ID: CONTENT-ACT-MIGRATION-001
- Version: 1.0
- Status: Requirements and source candidates inventoried; no migration certified
- Chinese companion: [ACTMigrationMatrix.zh-CN.md](ACTMigrationMatrix.zh-CN.md)
- Owner: Gameplay / content author
- Authority: [ACT contract](../Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md), grill Q76/Q82/Q88–Q90

## Priority and status

P0 is required for the golden path; P1 is optional separately playable Chapter Zero content; P2 is data/asset preparation or later chapters. **Requested priority is not verified promotion.** A candidate may be required P0 while implementation/tests/audit remain pending. Promote to the playable P0/P1 set only after independent behavior checks and asset/dependency audit pass. A private research test cannot be labelled public-release cleared.

Each future concrete entry records: source asset/behavior, Padma target, dependencies, requested P0/P1/P2, migration status, replacement status, test evidence and prohibited-asset dependency result. Missing source choices are unresolved, not invented asset paths.

## Initial required rows

Local reference root observed on 2026-09-10: E:/2026ue/Combat. Paths below are relative to its Content. File presence is verified; Blueprint behavior, rights and skeleton compatibility are not certified in this document task.

| ID / requested priority | Source candidate or missing input | Padma target and dependencies | Migration / replacement / evidence / audit |
|---|---|---|---|
| ACT-M01 / P0 | New player model: user selection pending; Combat/GAS_Xia.uasset is behavior reference only | New character, rig/retarget, AnimBP, keyboard/mouse 3C and camera | Pending / pending / none / not run |
| ACT-M02 / P0 | Combat/GAS/GA/GA_ComboBase.uasset; Combat/GAS/GA/ShuangDao/GA_Combo_X.uasset and GA_Combo_XX.uasset | Native lifecycle + AbilityDefinition/Sequence, ordinary attack and selected combo; M01 | Pending / pending / none / not run |
| ACT-M03 / P0 | Combat/GAS/GA/ANS/ANS_PreInput.uasset and ANS_Input_Window.uasset | Input-buffer/window state, legal branch/interrupt logic; M02 | Pending / pending / none / not run |
| ACT-M04 / P0 | Hit/GE/Cue paths need source-chain tracing; do not infer them from a VFX name | TargetData, hit deduplication, GE and cue ownership; M02/M03 | Pending / pending / none / not run |
| ACT-M05 / P0 | ShuangDao/GA_Combo_X_Throw under Combat/GAS/GA is a candidate; projectile spawn/impact chain not replayed | Representative projectile ability, collision/lifetime/cleanup; M04 | Pending / pending / none / not run |
| ACT-M06 / P0 | Summon behavior/asset choice not yet mapped | Representative summon ability, faction/AI/lifetime/exit cleanup; M04 | Pending / pending / none / not run |
| ACT-M07 / P0 | Combat/GAS/GA/ShuangDao/GA_Execution.uasset | Execution eligibility, attacker/victim animation, alignment and interruption; M04, compatible enemy | Pending / pending / none / not run |
| ACT-M08 / P0 | Fixed Padma ACT scene and enemy configuration to author | Strategy entry, one character covering required families, victory/defeat/exit/return; M01–M07 | Existing simple route only / pending / new coverage none / not run |
| ACT-M09 / P1/P2 candidate | Remaining Combat characters, alternate combos/maps and optional payloads | Select individually after P0; no wholesale migration requirement | Not selected / pending / none / not run |

The weapon/animation values in Combat are reference observations, not approved Padma damage/cost/cooldown/buffer values. Final rules still need the user's production content.

## Per-entry behavior capture

Record input/grant/activation chain, actor ownership, montage/sections, phase/window timing, required/blocked tags, target/hit/GE/Cue flow, cleanup and failure cases. For projectile/summon/execution include spawn ownership, collision/faction/eligibility, limits, death/cancel and map-return behavior. Distinguish observed behavior, defects and proposed Padma improvements.

Do not port GASCompanion parents, whole character Blueprints, copied per-combo Event Graphs or all-purpose NotifyStates. Rebuild with native GAS/C++ while keeping selected data/animation presentation references replaceable.

## Asset-audit acceptance

Track direct and transitive references, including soft references, maps, tables, Blueprint defaults, animation, material, Niagara, Sequence and cooked packages. Record original provenance even after a rename. Private learning use is distinct from cleared public release.

Public/Steam-target RC fails if `civili` or known prohibited source dependencies remain. A passed filename grep is not a passed asset audit. See [BuildMatrix](../Production/BuildMatrix.md) and [asset plan](../Production/DemoDeliveryPlan.md). No replacement tool, asset migration or runtime test was performed here.
