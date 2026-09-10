# TASK-049 Native Strategy HUD and Interaction Refresh

- Chinese companion: TASK-049-Native-Strategy-HUD.zh-CN.md
- Status: Review — implementation checked; user playtest pending
- Primary: root, current Local codex/mvp-demo-foundations
- Authorization: user requested the attached strategy UI reference and explicitly selected the native UE playable version.

## Outcome and scope

Recompose the native screen around the live 3D map: compact calendar/resources header, right node inspector and contextual actions, bottom illustrated hand and local garrison selection, distinct phase advance, and secondary menu/preparation surfaces. Preserve accepted run/combat commands, numbers, topology, save format and mode separation. Reference-image names, values and scenery are visual examples, not new rules.

Allowed paths: Public/UI/Screens/PadmaGameScreen.h and Private/UI/Screens/PadmaGameScreen.cpp; Public/Game/Framework/PadmaPlayerController.h and Private/Game/Framework/PadmaPlayerController{,UI,Map}.cpp under Source/DreamOfPadma; a focused UI test under Private/Tests/UI; new Content/Padma/UI assets/source art; Scripts/Editor/AuthorStrategyUI.py; this TASK pair, NativePlayableDemo guide pair, 00_INDEX pair, ProjectState pair and Changelog pair.

Use actual widget geometry for map/arena input boundaries. Clarify source-card selection versus destination inspection; hide irrelevant battle-route controls; preserve free previews/cancellation. Confirm destructive in-game load/retreat actions and block underlying input while any modal is visible. No Core rules, combat settlement, world assets, dependency configuration, historical prototype or unrelated working changes are edited.

Delegation: one read-only controller-flow audit; one image-only worker owns the new card-art atlas PNG. Root owns source, UE lane and closure.

## Acceptance

- [x] Native rendered layout has navy/gold hierarchy, illustrated cards, contextual inspector and distinct phase action.
- [x] Automated command regressions cover card selection/filtering/attachment/cancel, preparation drafts and modal priority; existing move/synthesis/combat/save tests pass.
- [x] Input uses arranged widget bounds and shared modal guards; scoped code review passed. Physical pointer/drag/keyboard playtest remains below.
- [x] Editor compile, focused automation, real native rendered checks and documentation audit pass.
- [ ] User aesthetic acceptance and physical input/drag playtest.

## Evidence and remaining work

Implemented: compact live resource/calendar header; actual war progress; right node/garrison inspector and contextual actions; persistent source selection while inspecting destinations; first valid local A as B attachment target; illustrated All/A/B/C hand, codex and pinned synthesis source; actual-topology minimap; distinct phase advance; menu load/retreat confirmation; coherent preparation/local-scene navigation; modal input guards; geometry-based camera/input bounds; readable selected combat entries and short buttons at 1280×720; empty preparation skill panels hidden.

Changes are the six runtime files named in scope, `Private/Tests/UI/PadmaStrategyInteractionTest.cpp`, atlas PNG and imported `Textures/T_CardIllustrations.uasset` under `Content/Padma/UI`, the import script and listed documentation pairs. Art is a generated 1254×1254 concept atlas (nine 418×418 cells); it does not alter world models or game data.

Evidence root: `C:/Users/lenovo/.codex/visualizations/2026/09/09/01a084b7-b919-7d61-8ca1-bf77da2ef822/task049`.

Final delivery preview: `world-final.png`. `import-repeat.log` reports zero errors/warnings and the existing texture SHA-256 remained `BA313CFA35FDC687B7F632FBD8D750081ADE205E503AC4F4CC515220625B4460` after rerun.

- `build.log`: final DreamOfPadmaEditor Win64 Development build succeeded.
- `Tests/index.json`: 22 existing WorldMap/TASK046/TASK040/TASK047 tests passed (21 clean, one expected invalid-generation warning), zero failures. Filter: `DreamOfPadma.WorldMap+DreamOfPadma.MVP.TASK046+Project.MVP.TASK046+Project.Naming.TASK047+DreamOfPadma.MVP.TASK040`.
- `UITests/index.json`: `DreamOfPadma.UI.StrategyInteraction` passed; exercises real controller intents, first-A attachment in mixed C/A/A selection, cross-node targeting rejection, filters/toggle, retained drafts, load confirmation and Esc priority. No real save slot written. Engine startup emitted `Condition failed` messages before test queuing; selected-test reports above contain no failures.
- Native `-game` captures: `world.png` at 1920×1080, `small-final.png` at 1280×720, `home-final.png`, `encounter-final.png`, `act.png`. `home-final.log` confirms both scenario and capture page are home. The earlier `home.png` captured the map and is not home evidence.
- `import.log`: atlas imported without Python errors. Source snapshot comparisons and new-file `git diff --check` passed. AuditDocs checked 232 Markdown files / 116 pairs; ValidateProject -Strict passed.
- Read-only controller review found attachment-target inconsistency and a draft reset regression; both were corrected and the same reviewer returned Pass on the fixes.

Remaining user check: open `L_PadmaWorld`, test hand filtering, noon C deployment and destination movement, A/B attachment and synthesis preview/cancel, menu dismissal, preparation/local-scene return, and battle drag/confirmation/retreat. Runtime command tests and screenshots do not certify pointer feel; the computer-use target closed before interactive verification. ACT labels can still overlap when units cluster. World/character assets remain placeholders; production art, long-session balance and packaged-build compatibility are outside this delivery. No staging, commit, merge or push.
