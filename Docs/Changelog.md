# Changelog

- Chinese companion for user reading: `Docs/Changelog.zh-CN.md`
- Document ID: `CHANGELOG`
- Version: `Unreleased`
- Status: `Active`

This document is the concise, user-facing release summary for delivered implementation work. Detailed scope, acceptance evidence, design decisions, and unresolved questions remain in the relevant TASK, ADR, module documentation, and `Docs/ProjectState.md`.

## Unreleased

### Added

- 2026-09-10 — **Playable MVP baseline integrated**: commits the complete current MVP snapshot as `00b894d` and fast-forwards local `main`; 472 tracked files covering UE code/assets/maps, data, UI, prototypes and bilingual documentation are now represented in the baseline. UE5.8 Editor Development build, documentation/project validation, staged diff check and Git LFS fsck passed. Packaging and user art/physical-hold acceptance remain unverified.

- 2026-09-10 — **TASK-053 technical documentation**: consolidates architecture/data/runtime/save ownership around the accepted offline Chapter Zero Demo. Records immutable baked MapKey, persisted stage stories and tutorial replay rewards as pending targets; defers the story editor and requires Development/Shipping acceptance. Preserves unanswered grill proposals and existing prototype compatibility. No runtime or asset changes.

- 2026-09-10 — **TASK-052 exit and selection follow-up**: explicitly releases CommonUI stack caches and removes button-material session retention; clicks pin the sidebar and hover no longer changes targets. Fresh tutorials give nearby fire one enemy guard. Editor build, 32 regressions and three actual PIE exits passed; v1/v2 save compatibility is retained. See TASK-052 ClickExit evidence.

- 2026-09-10 — **TASK-052 tutorial interaction and motion UI**
  - Fixes regional hover without rebuilding unrelated HUD/hand state; A target confirmation and B colocated-A choice/no-target cancellation revalidate the existing Core command before payment. The parchment hand wraps into five columns and scrolls vertically.
  - New tutorials use 55 cells around the same seven key positions; saved version-1 162-cell layouts remain intact. Painted Slate owns WASD/Home through the CommonUI page and handles map clicks, middle drag and wheel; cancellation clears held keys/capture. The controller retains the 3D camera without duplicate painted-key polling.
  - Adds matte navy/aged-gold/paper materials, isolated button hover scans, staged native Slate entrances and a fixed-UV portrait reveal. The central war glyph and opposing moving liquid display neutral progress as 50:50 while retaining the actual score and victory threshold.
  - Editor builds and targeted regression checks passed. Native probes demonstrated A deployment with Flow 60→56, B no-A cancellation without payment, attachment to the `earth` A instance (地痕) with Flow 56→55, W/Home navigation and real-RHI liquid motion. Latest input-routing rechecks remain in [TASK-052](Production/Tasks/TASK-052-Tutorial-Interaction-and-Motion-UI.md). Working-tree verified candidate; user art/physical hold feel and packaged behavior remain unverified. No Git integration.

- 2026-09-09 — **TASK-051 painted tutorial and hover interaction**
  - Adds a fixed 2.5D tutorial painting, localized GPU atmosphere, reference-guided navy/gold HUD and immediate hover details with retained sidebar targets. CommonUI modal/held-card input remains isolated.
  - Adds 162 deterministic hex cells around seven existing key nodes, paid habitable-wilderness occupation, bounded dusk AI expansion and cross-map save compatibility. Editor build, 31 focused tests, independent review and native hover/zoom/reset/modal checks passed. User aesthetic/hold-feel and packaged acceptance remain pending. [TASK-051](Production/Tasks/TASK-051-Painted-Tutorial-and-Hover.md).

- 2026-09-09 — **TASK-050 continuous landscape and activatable inspection**
  - Adds a shared-vertex terrain sample with graded roads, river margin, slope/node/road-filtered cosmetic PCG, seven editable models and eighteen materials. Uses navy/gold nine-slice HUD skins and illustrated inspection panels.
  - Adds real CommonUI page/overlay stacks, top-only input, nested Back/focus restoration and 500 ms readonly card/unit inspection. Details use current data and label unconfigured progression/art; Encounter enemy autoplay waits during inspection while ACT timing remains unchanged.
  - Editor build, 26 focused tests, native rendering/Back-focus checks and independent lifecycle review passed. Art/physical hold acceptance and packaged validation remain pending. [TASK-050](Production/Tasks/TASK-050-Landscape-and-Activatable-UI.md), [authoring and layer guide](Content/StrategyPresentation.md).

- 2026-09-09 — **TASK-049 native strategy HUD**
  - Reorganizes the native screen into calendar/resources, central map, right node/garrison actions, illustrated filtered hand, minimap and separate phase advance. Adds a nine-card concept atlas and create-only UE import script.
  - Clarifies source/destination selection, B attachment and preparation drafts; uses actual widget geometry for input, blocks overlays and adds load/retreat confirmation. Accepted Core rules remain unchanged. Validation and outstanding user playtest are recorded in [TASK-049](Production/Tasks/TASK-049-Native-Strategy-HUD.md).

- 2026-09-09 — **TASK-048 anchored 3D maps and legacy retirement**
  - Retires six old Demo assets, their 21-file runtime/test chain and old authoring script after generic-preview migration. The default map is L_PadmaWorld; unrelated changes and compatibility IDs remain preserved.
  - Adds editable template/generated map assets, transient scene handles, checkpoint tables, fixed NPC/story coordinates, seeded branches, frozen save topology and once-only story branches. No universal mandatory-anchor gate.
  - Adds camera navigation, continuous hex terrain, model hooks, cached road/selection feedback and real cosmetic PCG. Editor build and 22 focused tests passed; real map/battle/rollback return and 200 generated instances verified. User playtest, production art and packaged compatibility remain pending. [Authoring guide](Content/WorldMapAuthoring.md).

- 2026-09-09 — **TASK-047 runtime responsibility naming**
  - Migrates 39 runtime files into Core/Game/Gameplay/Presentation/World/UI, replaces delivery-stage type names, and separates framework and save declarations. Current bilingual source/art guides include the mapping.
  - Adds exact redirects for eight classes and 31 structs, in-place asset reference migration and a typed old-save reader. Editor build, 12 focused tests, table preservation and the world/battle return route passed. Content paths and stable IDs are retained; packaged compatibility is untested. No Git integration.

- 2026-09-09 — **TASK-046 native UE playable candidate**
  - Ports the supported HTML 0.4 loop into Core run commands, safe saves, separate native Encounter/ACT GAS, hex graph inspection/movement, home preparation, dialogue and CommonUI screens. Includes card/environment effects, explicit targeting/confirmation and character inspection.
  - Adds sharded typed tables, soft model/effect/ACT definition bindings, create-only content authoring, two playable maps and a per-file authoring guide. The user permits HTML temporary numbers in labelled UE Demo data. Build and focused automation passed; real native startup/entry rendering captured. User interaction and production art remain pending; FPS stays configuration only. No Git integration.

- 2026-09-09 — **TASK-045 lean Agent workflow**
  - Short current-state entry with preserved history; scoped English reads, compact task/handoff, bounded child context, risk-triggered review and optional teaching. Existing roles, model defaults, permissions and concurrency configuration remain.
  - Records user acceptance of HTML 0.4 and user-reported completion of initial authoring/UI and GAS studies. No game implementation or Git integration in this task.

- 2026-09-09 — **TASK-044 HTML 0.4 interaction amendment**
  - Ordinary attack now arms targeting before an enemy click releases it; Cancel/Escape is free. Dancer retains optional two-target selection. Long-press either side for portrait, live attributes, abilities/descriptions and catalog details; missing level/art/abilities are labelled.
  - Basic cards drag upward, drop to preview and require Confirm Release. Effects originate from cards/environment; fishbone rain falls onto both sides. Explicit target validation and committed-action playback recovery remain.
  - Existing state/input checks pass; real browser interaction and layout are for the user's playtest. Further extensive review stopped at the user's request. HTML/documentation only; no UE implementation or integration.

- 2026-09-09 — **TASK-043 HTML 0.3.1 freeze fix**
  - Corrected the native browser timer receiver that left controls locked at action windup. Scheduling/painting exceptions cancel remaining presentation and restore the committed view without replaying attacks or charges.
  - Closed the test gap with the shipped default Playback adapter, receiver validation, fault recovery and the screenshot's two-character fixture. 92 tests pass; post-refresh user browser confirmation remains pending. HTML and paired documentation only.

- 2026-09-09 — **TASK-043 Encounter feedback (HTML 0.3 playtest candidate)**
  - Explicit actor/upcoming-action markers; attack trails, impacts, actual HP loss, shield/block, healing/recoil, multiple targets/waves and retained final hits. Includes 1×/2× and reduced-motion support.
  - Fixed rejected-skill enemy-windup stalls, stale disabled UI after browser Back restoration, later rain waves raising already fallen units, and a dead Rain caster retaining the active turn despite surviving allies. Playback boundaries gate input and enemy timers.
  - 85 Node tests pass; actual browser visuals and input pacing still need manual acceptance. Offline launch, existing rules, ACT and saves remain. No C++/UE changes, commit or push.

- 2026-09-09 — **TASK-042 HTML rule prototype v0.2 (playtest candidate)**
  - Playable loop: offline HTML connects deployment, calendar/economy, synthesis, preset-route movement, Encounter/ACT, battle rollback, saves and two victories. Editable temporary values do not feed UE.
  - New interaction: double-click home opens Node Details / Protagonist Display / Other Modes; independent protagonist outfit/weapon/pose preview and separate ACT/FPS recruit catalogs. FPS remains non-playable.
  - Fix: success creates C at the current A/B node, without hand entry or another deployment fee; inputs discard on either outcome. Forge/watchman dialogue precedes one-time grants, and cancellation grants nothing.
  - Validation: 47 engine cases, 42 pure template renders, independent Review Conditional pass. User confirmed the initial entry; new browser interaction/visual acceptance remains incomplete because browser tooling rejected the local file URL. TASK remains Review.
  - No C++, UE asset/dependency changes this turn; CommonUI, display-stage and story-system continuation is advice only. Full card coverage/final presentation remain pending. No commit, merge or push.

- 2026-09-09 — **TASK-040 model assets and local scene preview**
  - UE result: five editable Data Assets and one native preview map show character, building and ground slots from typed soft-reference sources. Selecting another preview identity clears stale models/skills. ACT uses its existing model and skill-table authority.
  - Data/rules: added explicit three-state mobility metadata to existing character/basic-skill authoring definitions. Recorded inspection-only click, a separate right-side card movement command, full first-MVP enemy disclosure and A/attached-B group validation. Actual movement, full SLG UI and GAS execution remain downstream work.
  - Verification: UE5.8.2 Editor build, six project tests with zero test warnings/errors, asset creation/valid reruns/two invalid-existing rerun rejections, actual UE graybox render, bilingual/strict/dependency checks and independent Review Pass. Engine startup warnings are recorded in the completion report.
  - Delivery: Verified working-tree candidate; no commit, merge or push. The bilingual ModelAuthoring guide explains model assets, scene identities and skill-source binding; user PIE/learning evidence remains pending.

- 2026-09-08 — **TASK-008 Encounter foundation / TASK-036 ACT authoring**
  - User-visible result: selecting and confirming the Demo tile enters the authored Encounter scene; F8 requests one placeholder character through a validated service. The character and three source skill descriptions are data assets. A dedicated scene GameMode and camera show one representation with readable status.
  - Authoring result: five empty ACT character, weapon, skill, skill-table and catalog templates expose typed soft references, stable IDs, logical bindings and trait/terrain selectors. The bilingual ACTAuthoring guide explains filling and validation; actual ACT actions and GAS execution remain later work.
  - Verification: UE5.8.2 Editor build; four project automation tests with zero test warnings/failures; successful asset creation/rerun; actual cross-map summon/readback smoke and frame-11 render; independent read-only Review `Pass` for both tasks. Detailed evidence is in their completion reports.
  - Delivery state: both tasks are `Verified` working-tree candidates on `codex/mvp-demo-foundations`. Local integration and user PIE/learning acceptance remain pending. No commit, merge or push; protected project/build/default-map settings are unchanged.

- 2026-09-08 — **TASK-007 SLG Demo World Selection and Transition**
  - User-visible result: added a project-owned Demo Sandbox with one selectable tile and a typed node/scenario/spawn transition context retained within the same GameInstance/session across scene changes.
  - Verification: follow-up UE5.8 Editor compile, TASK-007 transition-context automation, unattended DemoSandbox map-load smoke, user-completed post-fix PIE selection/highlight/deselect/confirm/invalid-fixture checks, and independent Review `Pass`.
  - Integration: locally closed after completion-report commit `9ea99c7`; no remote push or release tag.
  - Scope boundary: the TASK-010-owned default-map configuration is present in separate local commit `d8f107f` and was not included as TASK-007 evidence; the Encounter scene, a dedicated lifecycle regression test, and all deferred production world rules remain outside this delivery.

- 2026-09-07 — **Bilingual implementation change-log workflow**
  - Goal: require every implementation Goal that changes code, data, UE assets, configuration, build files, or tests to leave a concise release-style summary.
  - Workflow: the Primary Agent supplies the draft; the Integration Coordinator writes the synchronized English and Chinese entries before `Verified` or `Done`.
  - Verification: `Scripts/AuditDocs.ps1`, `Scripts/ValidateProject.ps1 -Strict`, and `git diff --check`.
  - Runtime impact: none; no gameplay rule was changed.

### Planning and source clarification

- 2026-09-08 — **TASK-011 full-rules MVP contracts**: TASK-012–035 define separate GAS modes, ACT character/weapon roster, shared basic skills, calendar, economy, synthesis, world, persistence and campaign acceptance. The source record maps the two user PDFs to real card categories and screen requirements. User-confirmed rules take precedence over older conflicts; unaccepted numbers are not assigned defaults.
- This is working-tree documentation, with checks/review recorded in TASK-011. It is not a runtime release and does not change TASK-007's local integration evidence above.


- 2026-09-08 user clarification: retain A→F, B→D, C→R and revise conflicting collection labels. ACT content is not yet designed; editable model/skill-table soft-reference definitions are authorized, with no invented combat data.
