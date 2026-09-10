# TASK-046 Native UE HTML Playable Slice

- Chinese companion: TASK-046-Native-UE-HTML-Slice.zh-CN.md
- Document ID: TASK-046
- Version: 0.1
- Status: Review - native implementation candidate; user playtest pending
- Primary: root; current Local codex/mvp-demo-foundations
- Authorization: 2026-09-09 user requests native UE implementation of accepted HTML behavior, Combat reference, scalable content tables and authoring explanation. User explicitly permits current HTML temporary numbers in labelled UE Demo content.

## Outcome and scope

Deliver the accepted HTML 0.4 implemented subset as a native UE playable slice: world inspection/card movement, deployment/synthesis/calendar/resources, preparation/dialogue, separate Encounter and ACT GAS, basic skill interaction, battle rollback/commit and safe save/load. FPS remains preparation/catalog only; unsupported PDF abilities and relationship buffs remain labelled. Combat is read-only behavior reference; no dependency or bulk asset copy.

Allowed source additions: Source/DreamOfPadma/{Public,Private}/MVP/{Core/Run,Core/Content,Game/Run,Game/Content,Game/Playable,Gameplay/Combat,Gameplay/Encounter,Gameplay/ACT/Runtime,World/Playable,UI/Playable}; tests under Private/Tests/MVP/TASK-046. Existing ACT authoring/model definitions may receive compatible authored presentation/runtime fields only if needed. Root owns DreamOfPadma.Build.cs, DreamOfPadma.uproject, Config/DefaultGame.ini cook/cue paths and the CommonUI viewport key in Config/DefaultEngine.ini. Preserve existing default-map configuration and maps. Create Content/Padma/MVP/Playable and Scripts/Editor/{ExportPlayableData.cjs,AuthorPlayableContent.py,AuthorPlayableDemo.py}; authoring preserves existing assets and appends only missing documented generated bindings. Docs: this pair, ADR-0006 pair, native play/authoring guide pair, index/state/changelog and affected architecture/module pairs.

Dependencies: reuse TASK-007/008/036/040 working candidates. This user-authorized integration slice activates the relevant TASK-012 through 039 contracts for the HTML subset; it does not claim all of their broader acceptance. Accepted HTML/user choices supersede older generic gates. Concrete ownership/loading/clock decisions are in ADR-0006.

## Packages and evidence

One owner per file. Content package owns typed catalog and fixture authoring inputs; rules package owns neutral run values/commands and persistence adapter; combat package owns native mode execution and transient combatants. Root owns public seam reconciliation, UI/world composition, build lane and docs. Required checks: Editor build, focused state/catalog/GAS/rollback automation, create-only asset authoring, standalone rendering and reproducible user play route. One scoped lifecycle/save review near delivery.

No Git integration, commit, push, third-party edits or engine modifications. Exact source-to-feature mapping and the user's authoring work are in [NativePlayableDemo](../../Content/NativePlayableDemo.md).

## Delivery evidence and limits

- Native Editor build succeeded on UE 5.8.2. Final report `2026.09.09-08.20.49` records 11 successes, zero warnings and zero failures: eight rules/state tests, two catalog tests and one GAS mode/lifecycle test. The initial GAS cue-scan warning is resolved.
- Asset authoring created typed tables, soft-reference DAs, materials and L_PadmaWorld/L_PadmaBattle. A repeat run preserved both maps and existing fields, adding only missing guard/attack art bindings; native catalog validation succeeded.
- Real D3D12 standalone captures exist for world, home, Encounter and ACT in `Artifacts/TASK-046/`. Both battle captures entered through actual world-to-battle OpenLevel. UI uses CommonUI All input mode; Chinese labels render through Slate font fallback. Captures validate startup/presentation, not user input acceptance.
- A final real roundtrip succeeded: world ready at `08.20.24.162 UTC`, battle ready at `.543`, exit/rollback at `08.20.25.458` with no pending battle, world ready again at `.528`. `roundtrip-render.log` and `roundtrip.png` record the return. Non-shipping `-PadmaDemoScenario=roundtrip -PadmaPlayableCapture=<path.png>` reproduces it; it uses native commands, not simulated mouse clicks.
- `AuditDocs.ps1` passed (220 Markdown files / 110 pairs); `ValidateProject.ps1 -Strict` passed. Focused source/config/document whitespace checks passed. The new guide serves as optional learning/authoring evidence; no mandatory lesson or user teach-back was added.
- Scoped independent review: two P2 findings (travel failure recovery and stale target-count carryover) fixed; targeted source recheck Conditional pass. Injected failure after world teardown and manual mixed-roster/last-enemy targeting remain user checks; no broad review repeated.
- Evidence files are ignored local artifacts: `editor-build.log`, `authoring.log`, `Automation/index.json`, `*-render.log` and captures. No packaged build or formal imported-art validation was performed.
- Implemented behavior is the accepted HTML subset. Formal ACT combos, unsupported PDF actions, level progression, relationship buffs and FPS gameplay are not delivered. Current Dialogue is text/choices; Icon data hooks and sequence actor rebinding require later UI/presentation work.
- User play route and input checks are in the guide. Task remains Review until the user's UE playtest; earlier HTML acceptance does not imply UE acceptance.
