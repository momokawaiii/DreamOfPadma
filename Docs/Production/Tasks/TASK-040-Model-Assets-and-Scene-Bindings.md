# TASK-040 Model Assets and Scene Bindings

- Chinese companion: TASK-040-Model-Assets-and-Scene-Bindings.zh-CN.md
- Document ID: TASK-040
- Version: 0.1
- Status: Verified
- Primary Role / Agent: Model presentation and integration / root, sole writer
- Branch/worktree: Local codex/mvp-demo-foundations at c946591; no additional worktree
- Authorization: user requested model Data Assets and scene-to-selected-character bindings on 2026-09-09. Ready preflight is complete for this independent authoring/preview slice.

## Goal and decisions

Provide UE-editable static/skeletal model assets, domain-aware read-only character/skill resolution, and a native node-slot authoring preview. A selected definition supplies its model and own skill source; scene Actors are replaceable projections. Add the user's three-state mobility metadata to existing card authoring definitions and the future card-catalog contract, without executing world movement.

Accepted: click only inspects all node information; a separate right-side Move Here button submits ABC/card movement. Enemy information is fully public for the first MVP. A and attached B cards form a group and each member is checked. No partial move on failed group validation. FPS is future; Encounter and ACT retain their MVP requirements. Explicit, unambiguous mycard.pdf values may be used unless a later accepted rule conflicts; missing/conflicting fields still require the user.

## Read first

AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, Source/AGENTS.md, Content/AGENTS.md, Scripts/AGENTS.md, ProgramArchitecture, DataDrivenArchitecture, ModuleMap, RuntimeFlow, SaveSchema, World/Game/UI module READMEs, WorldState, ADR-0004/0005, TASK-036 and ACTAuthoring. Preview definitions do not freeze TASK-012's save/run schema.

## Allowed paths

- `Source/DreamOfPadma/Public/MVP/Core/Cards/PadmaCardMobility.h`
- `Source/DreamOfPadma/Private/MVP/Core/Cards/PadmaCardMobility.cpp`
- `Source/DreamOfPadma/Public/MVP/Presentation/Models/PadmaModelPresentation.h`
- `Source/DreamOfPadma/Private/MVP/Presentation/Models/PadmaModelPresentation.cpp`
- `Source/DreamOfPadma/Public/MVP/Game/Presentation/PadmaPresentationCatalog.h`
- `Source/DreamOfPadma/Private/MVP/Game/Presentation/PadmaPresentationCatalog.cpp`
- `Source/DreamOfPadma/Public/MVP/Game/Presentation/PadmaNodePreview.h`
- `Source/DreamOfPadma/Private/MVP/Game/Presentation/PadmaNodePreview.cpp`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-040/ModelPresentationTest.cpp`
- `Source/DreamOfPadma/Public/Demo/Encounter/DemoCharacterDefinition.h`
- `Source/DreamOfPadma/Public/MVP/Gameplay/ACT/Authoring/PadmaACTAuthoring.h`
- `Scripts/Editor/AuthorModelPreview.py`
- `Content/Padma/MVP/Presentation/ModelPreview/`
- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`
- `Docs/ProjectState.md`
- `Docs/ProjectState.zh-CN.md`
- `Docs/Changelog.md`
- `Docs/Changelog.zh-CN.md`
- `Docs/Production/MVPDecisionRegister.md`
- `Docs/Production/MVPDecisionRegister.zh-CN.md`
- `Docs/Production/MVPSourceExtraction.md`
- `Docs/Production/MVPSourceExtraction.zh-CN.md`
- `Docs/Production/Tasks/TASK-011-Full-MVP-Task-Cluster.md`
- `Docs/Production/Tasks/TASK-011-Full-MVP-Task-Cluster.zh-CN.md`
- `Docs/Production/Tasks/TASK-012-Run-State-and-Contracts.md`
- `Docs/Production/Tasks/TASK-012-Run-State-and-Contracts.zh-CN.md`
- `Docs/Production/Tasks/TASK-014-Character-Presentation.md`
- `Docs/Production/Tasks/TASK-014-Character-Presentation.zh-CN.md`
- `Docs/Production/Tasks/TASK-020-Card-Catalog-and-Lifecycle.md`
- `Docs/Production/Tasks/TASK-020-Card-Catalog-and-Lifecycle.zh-CN.md`
- `Docs/Production/Tasks/TASK-021-ABC-Sandbox-Operations.md`
- `Docs/Production/Tasks/TASK-021-ABC-Sandbox-Operations.zh-CN.md`
- `Docs/Production/Tasks/TASK-024-Sandbox-Graph-and-Movement.md`
- `Docs/Production/Tasks/TASK-024-Sandbox-Graph-and-Movement.zh-CN.md`
- `Docs/Production/Tasks/TASK-027-Battle-Routing-and-Travel.md`
- `Docs/Production/Tasks/TASK-027-Battle-Routing-and-Travel.zh-CN.md`
- `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.md`
- `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md`
- `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.md`
- `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.zh-CN.md`
- `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.md`
- `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.zh-CN.md`
- `Docs/Rules/WorldState.md`
- `Docs/Rules/WorldState.zh-CN.md`
- `Docs/Decisions/ADR-0005-World-Map-Storage.md`
- `Docs/Decisions/ADR-0005-World-Map-Storage.zh-CN.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.zh-CN.md`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaWorld/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.zh-CN.md`
- `Docs/Content/ACTAuthoring.md`
- `Docs/Content/ACTAuthoring.zh-CN.md`
- `Docs/Design/EN/06_MapAndPreparation.md`
- `Docs/Design/ZH/06_MapAndPreparation.md`
- `Docs/Production/Tasks/TASK-040-Model-Assets-and-Scene-Bindings.md`
- `Docs/Production/Tasks/TASK-040-Model-Assets-and-Scene-Bindings.zh-CN.md`
- `Docs/Content/ModelAuthoring.md`
- `Docs/Content/ModelAuthoring.zh-CN.md`

The existing TASK-008/036 working-tree candidate is preserved except for the explicitly listed additive mobility header fields. Generated files and third-party content are excluded. Root owns all writes and the serial build/Editor lane.

## Out of scope

Full SLG UI, authoritative movement/payment/attachments, gameplay roster changes, GAS execution, FPS combat, save/load, schema migration, existing map edits, asset renaming, build dependencies and numerical tuning. A preview catalog is not another gameplay card inventory.

## Dependencies and handoff

- Required predecessors: [TASK-036](TASK-036-ACT-Authoring-Definitions.md) static ACT schema; [TASK-008](TASK-008-Encounter-Character-and-Skill-Data.md) only for its existing read-only Demo character adapter and fixture.
- Their current Verified working-tree artifacts are sufficient for this user-authorized additive preview, without merging them or bypassing TASK-009/010 integration gates.
- TASK-014 consumes the general presentation descriptor/Actor. TASK-020/035 reuse mobility fields in actual catalogs. TASK-024 owns validated grouped movement; TASK-037 consumes presentation through TASK-034 composition.
- Static preview node slots contain NodeId, SlotId and illustrative entity/definition keys; they never mutate world state or become SaveGame records.
- ACT reads its existing Model/AnimationClass/SkillTable and weapon fields. General model assets contain presentation only. Mode source data is adapted in Game, never included by the neutral presentation Actor.
- Visual readiness and skill-details readiness are independent. Empty skill tables may leave a model preview usable; unknown/wrong-mode rows must report failure.
- A missing or invalid new selection clears obsolete model/skill information. Invalid node layout does not partially spawn a candidate.

## Acceptance criteria

- [x] Common three-state mobility metadata has explicit unconfigured diagnostics and condition identifiers; no unspecified card is certified movable.
- [x] General Data Assets support exactly one static/skeletal model, optional compatible animation and presentation transform.
- [x] Domain-qualified stable definition IDs resolve to the selected source without duplicating ACT model/skill authority.
- [x] Row schema, row/skill identity, missing assets, duplicates and animation compatibility produce actionable diagnostics; visual and skill readiness remain distinct.
- [x] A native node preview loads slot models and read-only skills and can replace a slot selection; no world/session/payment/battle mutation occurs.
- [x] Re-runnable UE Python authoring preserves existing content and creates editable project-owned assets/map.
- [x] Editor build, scoped automation, asset creation/rerun checks, visual smoke, bilingual/strict/diff checks and independent review pass.

## Verification

Compile DreamOfPadmaEditor with UE5.8. Run DreamOfPadma.MVP.TASK040 tests and the affected existing TASK036/Demo authoring tests. Author and rerun Scripts/Editor/AuthorModelPreview.py. Inspect the resulting UE map and logs. Run AuditDocs.ps1, ValidateProject.ps1 -Strict and scoped git diff --check. Capture logs outside source paths.

## Learning contract

Concepts: stable instance/definition identity versus a soft asset reference; model readiness versus executable skill readiness. The guide gives a small model replacement/slot selection exercise. User-produced prediction/practice/teach-back remains pending; implementation evidence is not learning evidence.

## Review and recovery

Read-only Architect and Reviewer inspect contracts and actual code/assets/evidence; no writable delegate. No other writer owns these paths. Recover only this task's exact changes after checking ownership. No commit, merge or push is authorized.

## Completion report

- Status: Verified on 2026-09-09; implementation acceptance passed. Local integration and user PIE/learning acceptance remain pending. No commit, stage, merge or push.
- Review: independent read-only architecture/acceptance Review returned Pass with no remaining P0–P3. Two P2 findings (rerun validation and an ineffective skeleton-negative fixture) and one P3 bounds-safety finding were fixed and rechecked.
- Changed paths: exactly the 12 source/header/test/script paths and all 52 Markdown paths (26 synchronized pairs) enumerated in Allowed paths above. The content-directory entry resolves to the six files listed below. Previous TASK-008/036 candidate changes are preserved except for the two explicitly authorized additive mobility headers.
- Code result: Core mobility authoring values; neutral model assets/descriptors/Actor; Game-layer Demo/ACT/model adapters; static node layout and replaceable preview bindings; create-only authoring script and focused tests. The dedicated preview GameMode spawns no Pawn. Non-Shipping screenshot capture is opt-in and cancels its callbacks during teardown.
- Asset result under `Content/Padma/MVP/Presentation/ModelPreview/`: `DA_Model_Character_Template.uasset`, `DA_Model_Building_Placeholder.uasset`, `DA_Model_Ground_Placeholder.uasset`, `DA_ModelPreview_Catalog.uasset`, `DA_NodeScene_Preview.uasset`, `L_ModelAuthoringPreview.umap`.
- Evidence root: `C:/Users/lenovo/.codex/visualizations/2026/09/08/01a07f68-77cf-7fe0-8479-93a66fb8a4f6/runtime-review`. Evidence is local-only and is not committed.
- Build: UE5.8.2 DreamOfPadmaEditor Development Win64 succeeded; `model-build.log` records the final no-Pawn GameMode build. UBT initially required its normal per-user log/cache access outside workspace; the authorized elevated retry succeeded.
- Automation: `model-tests-reviewed/index.json` reports 6 Success, 0 warnings/errors in each test: TASK007 transition, both TASK008 cases, TASK036 authoring and the two TASK040 cases. The skeleton test first proves compatibility with a valid UAnimInstance-derived class, then rejection of a different skeleton. Model/skill independence, wrong-mode/row identity, replacement, malformed layout, stale clearing and teardown are covered.
- Asset validation: `model-author.log` records creation; `model-author-rerun.log` validates the saved existing preview. `model-finalize.log` records two expected failures using an invalid in-memory existing binding, followed by valid saved-map reload. The test explicitly checks that the bad fixture was installed; invalid selection was never saved.
- Visual smoke: `model-render.log` and `model-preview.png`; actual UE standalone render exited 0. The image was inspected at 1280×720: garrison placeholder, building and ground, no default Pawn. The garrison log resolves the original three Demo skill definitions. The only post-automation source addition was the preview GameMode, covered by the final build and this render.
- Log limits: standalone engine startup still emits the existing editor widget-factory and r.MotionVectorSimulation warnings. Automation discovery also emits engine self-test condition messages before the project tests; the six exported project records have no warnings/errors. These logs are not claimed globally warning-free.
- Documentation checks: AuditDocs (198 Markdown files / 99 pairs), ValidateProject -Strict and git diff --check passed. Extended audit checks all current candidate Markdown links, 29 TASK-012–040 dependency/metadata pairs, acyclic dependencies, Python syntax and whitespace. Protected Config/DefaultEngine.ini, DreamOfPadma.Build.cs and DreamOfPadma.uproject remain identical to HEAD.
- Remaining boundaries: full SLG/inspection UI and grouped movement/payment are TASK-024/037; actual ABC catalog integration is TASK-020; ACT roster/GAS execution is TASK-035/030. No real user model/animation set has been supplied. The empty generic character and ACT templates intentionally require authoring. Soft-reference preview resolution is not combat readiness.
- Decisions still needed later: authored per-card mobility/conditions and missing/conflicting values, D27 preparation timing/state ownership, D28 relationship content, and the other downstream task gates. The accepted click/disclosure/FPS/group behavior is not re-opened.
- Learning evidence: Agent implementation/reproduction complete. The [English model guide](../../Content/ModelAuthoring.md) and [Chinese companion](../../Content/ModelAuthoring.zh-CN.md) contain prediction, model replacement, invalid selection, teach-back and transfer steps. User practice/teach-back remain pending.
- Changelog entry: UE model definitions and a read-only local scene preview, typed model/skill source resolution and three-state mobility metadata; full gameplay integration remains pending.
