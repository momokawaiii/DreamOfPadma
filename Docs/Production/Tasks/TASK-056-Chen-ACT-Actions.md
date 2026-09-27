# TASK-056 Chen ACT actions

- Status: In Progress; B showcase runtime acceptance in progress; other weapon/input/plunge user acceptance remains.
- Updated: 2026-09-22

## Current outcome and decisions

- ACT role-based consolidation complete: all 1,427 original assets have a per-file purpose/reference/disposition ledger and hash-verified backup in `Artifacts/ACTFinalLayout`. Canonical disk assets total 1,422; five obsolete saved single-attack fixture/preview assets retired, with interrupted old physical copies recoverably quarantined outside Content. See [layout guide](../../Content/ACTAssetLayout.md) and [ADR-0014](../../Decisions/ADR-0014-ACT-Asset-Layout.md). First-attack IDs/timing remain unchanged. Fresh verification passed every package, ACT edge and recorded property, plus scene reload and three daily preflights; all seven targeted D3D12 tests passed (`TestsFinal`), including actual-map SceneMapPIE. Editor build, syntax, docs and strict validation passed. Full-action-suite, visual parity and packaging remain separate acceptance.

- Scene-owned authoring: `L_ChenACT` now saves one `BP_ChenACTPlaced` and three `BP_ACTWoodenDummy` instances, referenced by the lab. Native battle borrows these actors and resets rather than destroys them; body appearance/capsule and weapon corrections survive reconfiguration. Editor appearance and native combat ownership are separated per [ADR-0012](../../Decisions/ADR-0012-Scene-Owned-ACT-Participants.md). Combat-idle/weapon/FX binding recipes no longer read raw Artifacts cache inputs; offline extraction remains separate.
- Current scene evidence: Editor build passed; fresh-process reload verified four saved actors, Blueprint defaults/new placement, three weapons and 1,353 package dependencies. Final D3D12 run passed all five targeted tests: `SceneParticipants`, actual-map `SceneMapPIE` (three resets), `ChenTrainingAnalytics`, `ChenWeaponStow`, `WoodenTrainingDummy` (`Artifacts/ChenSceneParticipants/TestsFinal`). The first run exposed outdated immediate-stow/early-plunge test waits, now aligned with existing profile timing without changing gameplay; the NullRHI Niagara observation failure passed with D3D12. `editor-preview.png` shows all four actors in the editor world without PIE. Combat-idle, weapon-lifecycle and FX-binding read-only UE preflights passed; reports are under `Saved/ChenAuthoring`. Map backup, screenshot and reload report are in `Artifacts/ChenSceneParticipants`. This is not full-action, original-shader-parity or package acceptance.

- The user-selected Chen root is now `Content/Sandbox/ACT/Character/ChenQianyu`. All 1,402 original assets were moved with UE AssetTools after a byte/hash backup; active source/recipes use the new paths. `L_ChenACT` serves both three-target training and manual toon-material learning. Per the latest user decision, its lighting, grey-fog background and double-layer ground now follow Toon `/Game/Render/Map_Main`; seven ground dependencies were copied, without modifying Toon. The old collision floor remains invisible; source character materials/rim-light PP are not imported. The user authors toon graphs incrementally. Guide: [Chen ACT/render lab](../../Content/ChenACTRenderLab.md). Evidence: `Artifacts/ChenACTLayout/ToonReference`.
- Directory verification loaded 96 critical assets with no old-root dependencies, redirectors or broken Game dependencies. Editor build and document/strict validation passed. TrainingAnalytics and TrainingShowcase passed. The broader CharacterActions test recorded six failures in plunge/execution/perfect-dodge timing assertions (`Artifacts/ChenACTLayout/Tests`); it must not be reported as a full pass. Its 0.4 s landing and pre-0.6 s perfect-input expectations conflict with the current staged-plunge/input-lock rules. Gameplay/timing was not changed for this environment work.
- Toon environment verification: fresh reload confirms two non-colliding visual ground layers, retained invisible original floor collision, three unchanged target profiles and source light/fog/exposure settings (`ToonReference/verify.json`). D3D12 standalone capture `ToonReference/render.png` shows Chen, three targets, patterned ground and grey fog horizon; no material compilation errors were reported. This is environment evidence, not character shader parity or fresh manual B/F8 interaction acceptance.

- Weapon-to-overview handoff: dissolve the held main blade at its current socket while retaining the scabbard; only after the fade finishes show the sheathed blade at its authored `inner` mount. Keep the sheathed blade and scabbard visible throughout overview breathing, with the main blade hidden. Movement/action/reopen/reset releases this presentation override. Regression evidence: `Artifacts/ChenQianyu/Showcase/TestsSheathedRest` (material fade, mount, visibility and extended stationary retention); GPU hand-to-hilt alignment remains unverified.

- Closing B blends from the weapon-to-overview exit into original `A_actor_chen_ui_overview_loop` (2 s), saved as `AS_Chen_ShowcaseRestLoop_CM`. It loops with breathing instead of freezing the exit frame or forcing combat idle. Movement/actions/reopen/reset release only the retained presentation montage. Build and `ChenTrainingShowcase` pass; the test advances stationary montage time. Source spine motion and matching endpoints are recorded in `Artifacts/ChenQianyu/Showcase/breathing-motion-evidence.json`; import and tests are in `rest_import_report.json` and `TestsBreathing`. Final GPU transition acceptance remains separate.

Playable lab: `/Game/Sandbox/ACT/Training/Maps/L_ChenACT`. The user authorized animation delivery followed by native GAS wiring and editable test data. Use the user BP_ACT_CHEN/ABP_ACT_CHEN; do not replace their authored graphs wholesale.

- 23 additional animations and 13 action definitions/Montages cover attacks, E/Q/R, jump, plunge, dodge/perfect dodge and execution. GAS owns playback/cancellation; Montage AN/ANS owns timing. F execution has no target/HP activation prerequisite. Damage/cost/cooldown and invulnerability retain explicit test-tuning status.
- Source-informed combo/hit/interrupt rules, melee sweeps, source-emitter FX and character-owned follow/orbit/lock camera are implemented. Middle mouse locks enemies. Full source FX/camera parity, Q target context and source noise remain incomplete.
- Standing weapon sequence: armed Idle hold 3 s, sheath, stowed hold 3 s, dissolve 1 s. Moving dissolution is 0.3 s. Preserve neutral Idle handoff and reset hidden weapon mounts before the next draw.
- Perfect dodge blocks gameplay/camera input through the 0.6 s Input.All window and until ghosts retire. Clear buffered commands; held movement must release and be pressed again. Administrative reset/UI remains available.
- Plunge requires 60 cm clearance, suspends startup descent until Descend at 19/30 s, then descends at editable 600 cm/s and restores gravity/air control on landing/cancel. This speed is UE test tuning, not confirmed original speed.
- Training HUD has Overview, Character, Details and Appearance pages using real fixtures, definitions, receipts/effects and weapon state. B now enters a character showcase with a depth-tested World-Space WidgetComponent. Weapon switching is not connected.
- Showcase uses original `A_actor_chen_ui_overview_to_weapon` (2.5667 s), `A_actor_chen_ui_overview_weapon_loop` (2 s), and `A_actor_chen_ui_weapon_to_overview` (2.75 s). These UI clips contain complete main-bone Transform tracks, not Humanoid body muscle channels. Preserve their exact decoded local transforms: the first native Humanoid-Animator candidate changed the pelvis and was rejected. Corrected FBX covers all 324 deform bones and passes numeric roundtrip checks.
- The existing DefaultSlot plays the showcase clips. Presentation turns the mesh and blends the camera without changing the gameplay heading. Training battle/actions/movement ticks pause; B exit, F8 and EndPlay restore state. Quick B reversal retains blend progress; reset/teardown never loads or plays a new exit animation. Playback rates, camera framing, depth of field and 0.18 s reveal/module motion are UE tuning, not original timing parity.
- Native WidgetInteraction routes mouse clicks/releases and wheel events. Closing releases pointer capture; the original viewport Screen visibility is restored. Project-owned unlit panel material compensates exposure and retains depth testing.

## Paths and latest evidence

Primary code: Source/DreamOfPadma/{Public,Private}/Gameplay/ACT/Runtime and UI/Screens/PadmaACTTrainingAnalyticsWidget. Assets: Content/Sandbox/ACT/Character/ChenQianyu/Actions. Authoring: Scripts/Editor/ConfigureChenInputAndPlunge.py and related Chen recipes. Preserve unrelated edits and the single UE execution lane.

- Animation import/reload, native pose comparisons and historical action/FX/camera checks: Artifacts/ChenQianyu/Actions. Exact per-revision reports and reproduction steps: [archived evidence](../ProjectCleanup.md).
- Latest input/plunge: successful Editor build and saved author.json in Artifacts/ChenQianyu/Actions/InputAndPlunge. Focused review passed; no new PIE/gameplay automation/GPU acceptance for this slice.
- Recorder fixes: F8 clears the battle-session recorder; rolling DPS uses combat elapsed time. Target cards retain identity across receipts and rebuild only for roster changes. Evidence: Artifacts/ACTProofMap/TestsFinal (2 passed), real damage-floater captures and runtime log.
- Showcase evidence: Artifacts/ChenQianyu/Showcase (source inventory, corrected import, builds, state tests and D3D12 standalone runs). Initial 3 state/UI tests passed; lifecycle review findings were fixed. The early runtime revealed and fixed incorrect pelvis sampling, excessive panel exposure and teardown-time soft loading. Final interaction and exit rechecks remain in progress.
- Material recipe: `Scripts/Editor/AuthorACTTrainingHologram.py`. Run with UnrealEditor-Cmd `<uproject> -run=pythonscript -script=<absolute script> -unattended -NullRHI`; it creates `/Game/Padma/UI/Training/M_ACTTrainingHologram`. Showcase assets are `Animation/Sequences/Imported/AS_Chen_Showcase{Enter,Loop,Exit}_CM`. Source and conversion reports remain under Artifacts/ChenQianyu/Showcase.

## Remaining acceptance

R final-blade repair: source shader variant 1294 accumulates red-channel weight
from a `1-UseWeightTex` baseline. The reconstruction multiplied texture alpha,
removing spatial weighting and prematurely dissolving cut08 blades. Corrected
`Scripts/Editor/ChenFX/material.py`; `RepairChenUltimateBladeMaterials.py` rebuilds
only five affected cut08 materials (2/4/13/16/18), with backups. Source timings and
particle lifetimes were retained. Fresh PIE shows all seven contacts, blade tails
visible at 3.65 and 3.75 s, and normal disappearance afterward. Before/after Montage
exports are identical. Evidence: `Artifacts/TrainingDummy/UltimateBefore`,
`UltimateAfter`, and `UltimateMaterialRepair`. This confirms a material fade bug,
not a missing final attack. Per-sample UV disturbance remains an existing shader
approximation outside this repair; full original-game pixel parity is not claimed.

Training targets now use Wjgz's copied wooden mesh and matching 0.6667 s hit clip;
see [authoring settings](../../Content/ChenACTRenderLab.md). Three combat profiles
remain unchanged. World-tick hit/retrigger/reset checks and analytics/showcase
regressions passed (`Artifacts/TrainingDummy/TestsFinal`); static D3D12 map capture
is `Artifacts/TrainingDummy/dummy.png`. Initial-overlap/volume hit FX now use the
target locator or center instead of the attack query center, while real surface
contacts are preserved. D3D12 fourth-attack/wooden-target and blade-contact tests
passed (2/2, no warnings) in `Artifacts/TrainingDummy/ImpactGPUTests`.
These Niagara spawning tests require rendering; NullRHI cannot verify their effects.
The initial spawn-position checks missed a separate particle-space error: the
common-hit-02 prefab root placement (Unity 0, 2.105, 4.99 m) was baked into emitter
offsets. `AuthorChenActionFX.py` now removes that translation while retaining child
offsets, rotation and shape. `ContactOriginRevision=1` invalidates the previous
asset. Baseline PIE SimCache under `Artifacts/TrainingDummy/FourthIsolation` proves
the component at (120,0,90) had its central flare at (619,0,300.5). Particle-space
and visual checks, rather than component location alone, are required for acceptance.
Fresh PIE captures `FourthFixed` (0.23 s) and `FourthFixedLate` (0.38 s, two contacts)
pass central-flare SimCache checks; SourceP08 now equals the contact root (120,0,90).
The late 1440x900 viewport screenshot shows the burst on the wooden torso. Both
capture processes exited normally. The authoring process saved successfully but
crashed during editor shutdown; the fresh reload/captures verify persisted output.

Playtest weapon draw/sheath/dissolve and movement transitions; verify perfect-dodge input release and staged plunge on real input. Check HUD page switching, target selector, final layout and flicker in the visible lab. Existing camera/FX checks do not cover all later changes. Cook/package and complete source parity remain unverified. No commit, merge or push is implied.
