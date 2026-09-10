# Current UE and HTML Test Plan

- Chinese companion: CurrentBuildTestPlan.zh-CN.md
- Document ID: TESTPLAN-CURRENT-001
- Version: 0.3
- Status: Current native regression plus historical HTML sketch cases; visual acceptance ongoing
- Baseline date: 2026-09-09
- Owner: [TASK-041](Tasks/TASK-041-Current-Build-Test-Plan.md)
- Execution results: TASK-048 integration reports Editor build, final runtime render and 22 relevant automated tests passing (21 clean, one expected warning), plus 200 runtime PCG instances. This documentation edit did not rerun them; manual acceptance below remains the user checklist.

## Current HTML entry update

TASK-042 adds the [playable HTML](../../Prototypes/HTML/index.html). Use steps 1–8 of its [v0.2 guide](../../Prototypes/HTML/README.md) to test home preparation, dialogue and in-place synthesis. The table below and HTML01–HTML13 preserve the older TASK-041 sketch baseline, not the current HTML feature limit. Current code/state evidence is in TASK-042; new browser acceptance is pending. UE production resumed with the current native implementation; follow [NativePlayableDemo](../Content/NativePlayableDemo.md) and [WorldMapAuthoring](../Content/WorldMapAuthoring.md).

## 1. What this plan verifies

Current UE testing covers the native map, frozen runs, checkpoint branches, separate Encounter/ACT GAS and model/ACT authoring. HTML01–HTML13 remain the historical TASK-041 screen-sketch checks; they do not describe current native functionality.

| Area | UE candidate | HTML sketch |
|---|---|---|
| Map/entry | Authored/seeded directed maps and map-to-battle return | Seven historical example nodes, hover and local inspection |
| Character/building display | Real UE components and soft-reference assets; placeholder geometry | CSS silhouettes and static labels |
| Skills | Separate Encounter/ACT sources; generic model preview has no skills | Five shared-slot examples with switchable mode labels |
| Global preparation | Current native preparation UI and ACT Data Asset authoring | Six browsable sketch sections; no actual equip/bonus service |
| Calendar/resources/synthesis/GAS/outcomes/save | Current native fixture implementation; validate through its guide | Historical static sketch placeholders |
| Card movement policy | Native atomic group movement/payment | Sketch Move Here execution remains absent |

Start with MAP01–MAP06 and the [native playable guide](../Content/NativePlayableDemo.md), then UE08–UE13 and current automation. Add fixture Q failures and optional ACT asset checks as needed. Run HTML01–HTML13 only when reviewing the archived screen sketch.

## 2. Baseline and launch points

UE: open [DreamOfPadma.uproject](E:/2026ue/DreamOfPadma/DreamOfPadma.uproject) with installed UE 5.8.2 (`E:/Epic Games/UE_5.8`). Record current branch, HEAD and dirty state; HEAD alone does not identify the candidate. Build the current Editor target if modules are stale; preserve the engine association.

Open maps explicitly in the Content Browser; do not rely on startup-map settings:

- `/Game/Padma/MVP/Playable/Maps/L_PadmaWorld`: current native map and battle route.
- `/Game/Padma/World/Maps/Editing/L_MapAuthoring`: editor graph/handle workflow.
- `/Game/Padma/MVP/Presentation/ModelPreview/L_ModelAuthoringPreview` — UE08–UE11.

Use a single-player PIE session and the normal Play mode, not Simulate. Save your own work first; changes made to test definitions/maps require test copies. Stop PIE before switching maps. The preview map uses a fixed camera; movement/attack input is not implemented.

HTML: open [qa-padma-world.html](C:/Users/lenovo/.codex/visualizations/2026/09/08/01a07f68-77cf-7fe0-8479-93a66fb8a4f6/qa-padma-world.html) with Chrome or Edge, using Ctrl+O if the app displays the file as text. This is the standalone wrapper. The source fragment is [padma-world-design.html](C:/Users/lenovo/.codex/visualizations/2026/09/08/01a07f68-77cf-7fe0-8479-93a66fb8a4f6/padma-world-design.html). These are local files outside the Git repository and need no server. Use the wrapper for theme/encoding/iframe checks. Developer Console means the sketch frame's context when inspecting elements.

Record browser version, viewport width, zoom and theme. Both fingerprints were checked while preparing this plan:

- Source SHA-256: `808D281C7F2F3C6FDB36C497CEDAD11A4696EF708590124DF251C3900BD10DF9`.
- Standalone wrapper SHA-256: `6CBAC3D4B01B3A41E87A001FE5ECF6C89937152728832ECCD0DDAA91FE9F47C3`.

## 3. Observation and recovery conventions

For every case, record actual behavior and Pass / Fail / Blocked / Not applicable. Not run is the initial state. A negative case passes when invalid input is rejected as specified. A required missing asset blocks its dependent case; a future feature is not a regression in existing functionality.

UE evidence: the view plus relevant `[PadmaMap]`, `[PadmaMapPCG]`, `[World Map]`, native run/battle, `[TASK-040]` and `[ACT Authoring]` logs. TASK-007/008 log names are historical, not current smoke-test targets.

HTML evidence: affected node/view/tab, actual label, and any Console exception. A short written reproduction and log excerpt are sufficient; screenshots/video are optional, useful for visual defects.

Stop/restart PIE to reset runtime state. It does not undo edited Data Assets. Restore the named field after each negative variant, or use the known test copy. Refresh resets HTML navigation. Do not use git reset/restore or delete project assets to normalize testing.

## 4. UE manual cases

| ID | Start and action | Expected result and evidence |
|---|---|---|
| MAP01 | Open L_PadmaWorld and start PIE; hover/select nodes, wheel zoom, middle-drag, WASD/arrows, Q/E and Home. | Readable map and panels; inspection does not mutate resources. Camera remains where placed through ordinary state refresh; Home fits the map. |
| MAP02 | Start two new maps using the same seed, then try other seeds; record map ID, seed, node/edge identities and NPC positions. | Same inputs reproduce topology and fixed anchors; sampled seeds can vary branch topology. Starting a new game replaces the current unsaved run only after valid generation. |
| MAP03 | In separate new runs, complete watcher before gate or bypass that encounter and reach gate; inspect its later noon dialogue. | No missed flag before the checkpoint. Arrival records matched/missed once; noon dialogue uses that branch. Cancelling/reopening does not resolve again. No universal main-story visit requirement. |
| MAP04 | Follow WorldMapAuthoring on copied assets: move/apply a template handle, generate, attempt a generated-anchor move, import valid/invalid checkpoint rows. | Valid edits save explicitly; invalid edits/import/generation preserve previous values. Generated anchors retain template identity/position. Clear Preview leaves no temporary map/node/model actors. |
| MAP05 | Inspect the hex field, roads, markers and PCG; hover repeatedly, leave/return to map view. | PCG completion logs show actual instances (current kit: 200); collisions do not block picking. Ordinary hover leaves layout-build unchanged. Hidden map children do not leak into other views. Record any visual defects found. |
| MAP06 | Save a generated run, enter/return from battle and reload after changing the configured map. | The saved/battle snapshot restores its own frozen graph and checkpoint state; commit occurs once and exit/defeat follows the existing rollback contract. Use the native playable guide for battle setup. |
| UE08 | Open L_ModelAuthoringPreview. Select Home - Model Authoring Preview → Rebuild Preview twice, then start/stop PIE twice. | Three presenters remain: garrison oval, building cube, ground plane. No duplicate models or extra default Pawn. [TASK-040] shows Garrison Visual=1 Skills=0 and Building/Ground Visual=1 Skills=0. Count only presenters; lights/camera are unrelated Actors. |
| UE09 | Use fixture Q below. Record the QA building definition ID, change only its StaticModel from Cube to Sphere, then rebuild. Clear the model reference and rebuild; finally restore Cube. | Model replacement is visible while the recorded ID stays the same. Clearing the reference hides the old building and reports a model error; restoration works. General model sources have no skills, which is expected. |
| UE10 | In Q, change only the Garrison binding's DefinitionId to QA.Missing, rebuild, then restore Visual.Preview.Garrison and rebuild. Keep Domain=Model and SelectedSkillRow=None. | The old garrison and old skill details clear; its log reports Visual=0 Skills=0. Building/ground may remain valid. Restoring resolves the generic garrison with no skill rows. Details edits are not SetPreviewSelection calls; automatic row-reset behavior is tested in AUT06. |
| UE11 | In Q, test separately: duplicate the selected building catalog key; restore, then duplicate a static SlotId in the copied scene definition; restore and rebuild. | Duplicate selected source is rejected without first-entry fallback. Invalid static layout produces zero presenters, not a partly built scene. Each restored variant returns to three presenters. Keep a log for each variant. |
| UE12 | Inspect Mobility on a current native card row and an ACT character; toggle Configured and policy/ConditionId on a duplicate. | Movable/Immovable/Conditional metadata validates. Editing a field does not execute movement or make ACT characters deployable on the map; current runtime movement is checked separately in the native guide. |
| UE13 | Open the unchanged DA_ACTAuthoring_Template catalog and press Validate Authoring; inspect [ACT Authoring] in Output Log. | The empty drafts fail with actionable missing-reference/field messages, without crashing. This case PASSES when incomplete data is rejected; it is not a playable-ACT test. If templates have been filled, use an empty test catalog instead. |
| UE14 | With fixture A below and a compatible imported skeletal mesh, validate its ACT catalog, then bind the QA ACT character to Q's Garrison slot and rebuild. | Static authoring validates; the selected model and one QA ACT skill resolve. Missing ACT description stays empty. Registered GAS execution, attacks and weapon attachment are not implied. Without a usable mesh, mark the manual case Blocked: missing asset; AUT04/AUT05 still cover synthetic fixtures. |
| UE15 | From a passing fixture A, change one item at a time: unknown SkillId/row, duplicate activation binding, unknown trait/terrain, or incompatible animation if available; validate and restore each. | Each applicable variant is rejected with its source/row/reason. In Q, a valid ACT model may remain visible while skill readiness fails; no Demo skills leak into it. Use AUT04/AUT05 for a wrong-row-type table that the editor's typed picker prevents assigning. |
| UE16 | Advanced, optional: after recording asset hashes and closing UE, rerun the existing authoring scripts against the current completed fixture. See the commandlet appendix. Do not inject invalid data into the shared original map. | Existing authored files are not overwritten; AuthorModelPreview validates the saved existing map each time. Empty ACT drafts are expected to be reported incomplete. New files are expected only if absent before the run. Invalid-existing-map behavior has prior TASK-040 evidence; a fresh negative run requires an isolated project copy. |

### Fixture Q: isolated model edits for UE09–UE11

Create a manual test folder under `/Game/Padma/Tests/Manual/` in the Editor. Duplicate the preview map, its scene definition, catalog and building model asset into it. Open the copied map and point the host's SceneDefinition and Catalog to the copies. Point only the copied building catalog entry to the copied model; set that model's VisualDefinitionId, catalog key and Building binding key to the same `QA.Visual.Building`. Leave garrison/ground sources read-only. Ensure SelectedSkillRow is None. Press Rebuild Preview and confirm the three-model baseline before making any negative edit.

Changes to the duplicate map must not modify original definitions. The slot layout asset does not own current garrison state, and its Level soft reference is not a request to travel. No user art is needed for Cube/Sphere tests.

### Fixture A: optional ACT authoring for UE14–UE15

Duplicate the five ACT templates from `/Game/Padma/MVP/Definitions/ACTCharacterCards/` into the manual test folder. This is static QA content, not a new playable character or numerical configuration:

1. Give character, weapon and skill unique IDs/display names, for example QA.ACT.Character, QA.ACT.Weapon and QA.ACT.Skill.
2. Assign a usable skeletal Model to the character. Leave AnimationClass and the skill Montage empty initially; optional compatible assets are tested later.
3. In the copied PadmaACTSkillRow DataTable add one row QA_Skill: SkillId=QA.ACT.Skill, ActivationBindingId=QA.Input.Primary, Definition=the copied skill.
4. Set the skill's AbilityImplementationId=QA.Executor.Preview. Point the character's SkillTable to the copied table.
5. Give the weapon exactly one model, such as the engine Cube. Add character/weapon references to the copied ACT catalog.
6. Add QA.Input.Primary and QA.Executor.Preview to the catalog's corresponding References sets. Leave optional traits/terrain restrictions empty for the baseline.
7. Validate Authoring. For Q's preview, add an ACTCharacter catalog entry with the same character DefinitionId and its typed soft reference; select that key in Garrison. Do not fill Model in this same entry.
8. Read the resulting Presentation; then test individual failures and restore the passing setup.

These QA strings declare validation vocabulary, not installed GAS abilities. Missing imported skeletal content is a legitimate Blocked reason. Do not manufacture damage, costs or cooldown values.

## 5. HTML manual cases

| ID | Start and action | Expected result and evidence |
|---|---|---|
| HTML01 | Open qa-padma-world.html in Chrome/Edge. Refresh once with DevTools Console open. | Chinese text is readable and the world view has seven sample node buttons over hex terrain; two are named 归处, distinguished by side/type. Resource values are — placeholders. No uncaught page exception. These counts are layout fixtures, not gameplay limits. |
| HTML02 | Hover/focus each node without clicking, especially player 归处, 薪火 and ruler 关隘. | The side panel follows name/type/terrain/owner. Player 归处 shows A 佛罗伦萨的曙光, B 合一珠 attached to A, C 维特鲁威人; empty fixtures say 未配置部署. Hover never enters the local view. Enemy-policy text is tracked separately as GAP01. |
| HTML03 | Click the player 归处 node, return using 返回沙盘; then use the side-panel 进入地块 button and return again. | Both entry paths show the same node's local inspection and restore that node's selection on return. No movement/resource/battle occurs. Selection highlighting is inspection focus, not ownership or occupied-card position. |
| HTML04 | In player 归处 local view, click 人物模型, then 工事模型. | Character details show 维特鲁威人 · C / R and 完美肉体、黄金分割、食烟火. Building selection shows its separate placeholder detail. The silhouettes are CSS shapes; no real 3D model/animation or effect execution is expected. |
| HTML05 | Return, enter 薪火 or another empty fixture; also inspect ruler 关隘. | Previous player models/details do not leak into the new local view. Empty fixtures have no character/building buttons. Ruler content is unauthored; this is not evidence that the accepted full enemy disclosure is implemented (GAP01). |
| HTML06 | Open 全局配置 from world and return; repeat from a local scene. While settings are open, click 全局配置 again, then 返回. | Returns to the original world/local view and retains the active node. Reopening settings does not overwrite the saved return destination or fabricate equipment changes. |
| HTML07 | Visit ACT 出战, FPS 出战, 共享基础技能, 关系与成长, 增益总览 and 卡牌图鉴. | Active tab and content match. ACT shows pending character/weapon/skill bindings; relationship/bonus sections show placeholders; codex shows A·F / B·D / C·R. FPS stale scope wording is GAP02, not accepted design. |
| HTML08 | In shared skills, switch Encounter → ACT → Encounter and inspect all five entries. | Names/order/slot numbers remain 1–5: 鱼骸骤雨、一刀两断、天鹤之拥、梦中彩笔、马尔可夫链. Only the mode label changes; effects remain 待配置. This verifies the sketch's shared-slot display, not complete card-library data. |
| HTML09 | Select a skill entry in each mode, then visit relationships and bonus overview. | Footer identifies the selected card/current mode. Browsing never casts a skill, equips an item, increments affection or applies a buff. There is no gameplay service behind these fields yet. |
| HTML10 | At viewport widths 1280, 768 and 390 CSS pixels, repeat world → local → settings. Use DevTools Rendering to emulate light and dark prefers-color-scheme. | Node/buttons/text remain accessible; no clipped return/config controls or unusable overlap. Narrow layout stacks the panel below. Frame-internal vertical scroll is acceptable. Test all tabs; an actual obstruction is a layout defect, not a deferred gameplay feature. |
| HTML11 | Focus inside the sketch iframe; use Tab/Shift+Tab to reach a node, Enter to open it, then Escape. Repeat Escape from settings. | Keyboard focus can reach controls; Escape returns local → world and settings → its previous view. In a browser, Tab is focus navigation, not the future ACT repository/bullet-time key. If focus is in DevTools/address bar, refocus the sketch before judging. |
| HTML12 | After visiting several views/modes, reload the page. | Returns to initial world/player 归处/ACT settings tab and Encounter skill-mode defaults. No owned cards or persistence is claimed; refresh losing sketch navigation is expected. Record any script exception. |
| HTML13 | Optional: if the host shows Tweak controls, toggle 地块信息密度 and 路线展示, then change nodes. | Compact hides the terrain/facility block; expanded restores it. Route display toggles lines without changing node identity/count. No Tweak control in a plain fragment host is Not applicable, not a failure. |

## 6. Known gaps versus the latest accepted design

GAP01/GAP02 describe the historical HTML sketch. GAP03/GAP04 below distinguish that old baseline from the implemented native runtime; they are not blanket claims that UE systems remain missing.

| ID | Current gap | Expected future acceptance / owner |
|---|---|---|
| GAP01 | HTML still says enemy intelligence rules are pending; enemy model/content fixtures are absent | First-MVP full enemy disclosure, including card identities/attachments/state; TASK-024/037 and a sketch update |
| GAP02 | HTML FPS tab still says scope is pending | Clearly future/unavailable in the first playable MVP; ACT remains the real-time MVP; TASK-038 and a sketch update |
| GAP03 | Historical sketch has no Move Here executor; current UE has native atomic movement | Regress current movement through NativePlayableDemo; sketch acceptance stays scoped to its static behavior |
| GAP04 | The old unconnected-UE assessment is superseded by the native runtime | Current map/checkpoint/PCG visual and interaction acceptance remains ongoing under TASK-048 |

Opening a node remains read-only. The old Demo Enter route is retired; current acceptance uses MAP01–MAP06 and the native guide. Passing historical sketch/model cases alone does not certify the complete game.

## 7. Existing automation and optional authoring rerun

| ID | Exact automation name | Coverage |
|---|---|---|
| AUT04 | `DreamOfPadma.MVP.TASK036.AuthoringValidation` | ACT identities, row types, references, skeleton and trait/terrain schema |
| AUT05 | `DreamOfPadma.MVP.TASK040.ModelAndSkillResolution` | Mobility shape; static/skeletal models; independent model/skill readiness |
| AUT06 | `DreamOfPadma.MVP.TASK040.NodeBindingReplacement` | Replacement, cleared row/skills, invalid layout and presenter lifecycle |
| AUT07 | `DreamOfPadma.WorldMap.Generation.` | Four generation/validation/atomic editor cases |
| AUT08 | `DreamOfPadma.WorldMap.Run.` | Four checkpoint/rollback/frozen-map/phase cases |
| AUT09 | `DreamOfPadma.MVP.TASK046.` | Current catalog, core run and separate GAS regression |

Close the Editor after saving your work and use one UE process at a time. The following command writes a fresh report under the user's temporary folder and uses process-local DDC flags that previously avoided this machine's Installed DDC startup failure. It does not change project configuration.

```powershell
$qaRun = Join-Path $env:TEMP ('PadmaQA-' + (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Path $qaRun | Out-Null
& 'E:/Epic Games/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' `
  'E:/2026ue/DreamOfPadma/DreamOfPadma.uproject' `
  -unattended -nop4 -nosplash -NullRHI -NoSound `
  -DDC-ForceMemoryCache -ddc=NoZenLocalFallback `
  '-ExecCmds=Automation RunTests DreamOfPadma.; Quit' `
  '-TestExit=Automation Test Queue Empty' `
  "-ReportOutputPath=$qaRun/Automation" `
  "-abslog=$qaRun/Automation.log"
$LASTEXITCODE
$qaRun
```

Pass requires the current relevant test records to be present and successful, with failures/skips investigated; use the TASK-048 suite manifest (22 relevant tests at this integration baseline). AUT01–AUT03 and UE01–UE07 belong to retired TASK-007/008 fixtures and are not required. Read the actual report; exit code 0 or an empty report is insufficient. The `DreamOfPadma.` namespace can also include other active project tests.

NullRHI cannot validate visible rendering. Keep MAP01–MAP06 and UE08 visual checks separate. Known engine startup widget-factory/r.MotionVectorSimulation warnings and engine self-test discovery messages are documented in TASK-040; compare them with the baseline and still investigate new project-specific errors.

For UE16, compare existing .uasset/.umap hashes before/after AuthorModelPreview. AuthorACTTemplates.py remains an optional separate rerun with intentionally incomplete draft handling. Use [WorldMapAuthoring](../Content/WorldMapAuthoring.md) for the editor map script; do not run the retired Demo authoring script.

```powershell
# Use the same $qaRun from the automation command, or create a fresh evidence folder.
& 'E:/Epic Games/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' `
  'E:/2026ue/DreamOfPadma/DreamOfPadma.uproject' `
  -run=pythonscript -EnablePlugins=PythonScriptPlugin `
  '-script=E:/2026ue/DreamOfPadma/Scripts/Editor/AuthorModelPreview.py' `
  -unattended -nop4 -nosplash -NullRHI -NoSound `
  -DDC-ForceMemoryCache -ddc=NoZenLocalFallback `
  "-abslog=$qaRun/AuthorModelPreview.log"
```

The generic model preview no longer requires Demo authoring. If a script unexpectedly changes an existing asset, stop that test and record the paths rather than overwriting originals.

## 8. Evidence baseline and acceptance

Prior evidence, inspected while preparing this plan: `C:/Users/lenovo/.codex/visualizations/2026/09/08/01a07f68-77cf-7fe0-8479-93a66fb8a4f6/runtime-review/model-tests-reviewed/index.json` reports six successful project tests and zero per-test warnings/errors. TASK-040 also records `model-build.log`, `model-finalize.log`, `model-render.log` and `model-preview.png`. Earlier HTML three-width/light-dark checks are recorded in TASK-011. These are historical TASK-040/011 results; current TASK-048 build/22-test/200-PCG-instance and final-render evidence is recorded separately.

Call the existing-function regression a pass only when the required smoke and automation cases pass, and no unresolved critical defect blocks their scope. Record conditional-asset cases as Blocked with the missing fixture, optional host features as Not applicable and the historical sketch gaps separately from current native defects. Do not turn those statuses into a claim that the complete game is finished.

Severity suggestions: P0 = project/data damage or completely unusable build; P1 = existing key route fails, duplicates state, or invalid input bypasses its gate; P2 = wrong details, stale model/skill, unusable layout or rule-label mismatch; P3 = cosmetic issue with a usable control. State severity and reproduction separately from whether the feature is implemented.

Copy this record for each executed case:

| Field | Fill in |
|---|---|
| Case / date / tester | MAP01 or UE08 etc.; timestamp |
| Build | branch + HEAD + dirty candidate; HTML fingerprint when relevant |
| Environment | UE/browser version, map, viewport, theme |
| Starting data and steps | copied fixture IDs, each changed field and operation |
| Expected / actual | exact symptom, ID/count/message |
| Result / severity | Pass, Fail, Blocked, Not applicable / P0–P3 if defective |
| Evidence / recovery | relevant log excerpt or optional image; reset/restore result |

## 9. Source references

- [Current project state](../ProjectState.md)
- [UE model guide](../Content/ModelAuthoring.md) and [ACT authoring guide](../Content/ACTAuthoring.md)
- [TASK-008 evidence](Tasks/TASK-008-Encounter-Character-and-Skill-Data.md), [TASK-036 evidence](Tasks/TASK-036-ACT-Authoring-Definitions.md), [TASK-040 evidence](Tasks/TASK-040-Model-Assets-and-Scene-Bindings.md)
- [Accepted decisions](MVPDecisionRegister.md) and [screen design](../Design/EN/06_MapAndPreparation.md)
- Source cross-check: Core/Run, Core/WorldMap, Game/WorldMap, World/Map, World/PCG, PadmaNodePreview/PresentationCatalog, separate Gameplay/Encounter and Gameplay/ACT, current tests and the historical HTML script. No new rule is introduced.
