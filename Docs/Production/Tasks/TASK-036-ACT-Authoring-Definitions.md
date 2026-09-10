# TASK-036 ACT Authoring Definitions

- Chinese companion: TASK-036-ACT-Authoring-Definitions.zh-CN.md
- Document ID: TASK-036
- Version: 0.1
- Status: Verified
- Parent: full-rules MVP / user clarification D23, 2026-09-08
- Primary Role / Agent: Gameplay data / root, sole writer
- Branch/worktree: Local codex/mvp-demo-foundations; no worktree
- Authorization: user explicitly requested editable model and ACT skill-table soft references before ACT content is designed. Ready preflight completed; this is static configuration, not TASK-030 combat or TASK-035 roster state.

## Goal and decisions

Create editable ACT character/weapon/skill assets and a distinct typed ACT skill table. Validate missing content, references, identity, bindings and skeletons without inventing gameplay numbers or starting GAS. The user can create assets, assign imported models and skill definitions, and see actionable validation failures.

Frozen: ACT characters are separate from turn-based cards. Basic-skill identity/slots remain shared only at the future provider boundary. A→F/B→D/C→R does not classify ACT characters. Trait/terrain restrictions are explicit authoring data; execution timing, roster policy and values remain D20/D23. Content version is schema metadata, not a gameplay number.

## Read first

AGENTS.md; Docs/00_INDEX.md; Docs/ProjectState.md; Source/AGENTS.md; Content/AGENTS.md; Docs/Architecture/DataDrivenArchitecture.md; Docs/Architecture/ModuleMap.md; Docs/Architecture/Modules/PadmaGameplay/README.md; Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md; ADR-0004; MVPDecisionRegister D23.

## Allowed paths

- `Source/DreamOfPadma/Public/MVP/Gameplay/ACT/Authoring/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/ACT/Authoring/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-036/`
- `Content/Padma/MVP/Definitions/ACTCharacterCards/`
- `Scripts/Editor/AuthorACTTemplates.py`
- `Docs/Content/ACTAuthoring.md`
- `Docs/Content/ACTAuthoring.zh-CN.md`
- `Docs/Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md`
- `Docs/Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md`
- `Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-030-ACT-GAS-Playable-Loop.md`
- `Docs/Production/Tasks/TASK-030-ACT-GAS-Playable-Loop.zh-CN.md`
- `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.md`
- `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md`
- `Docs/Production/Tasks/TASK-036-ACT-Authoring-Definitions.md`
- `Docs/Production/Tasks/TASK-036-ACT-Authoring-Definitions.zh-CN.md`

Coordinator-owned index/state/Changelog updates are separate serial integration documentation. No other writer owns this schema. TASK-008 assets and travel code remain its own slice, with one serial UE build lane.

## Out of scope

GAS/plugin/build dependencies, ASC lifetime, combat execution, sequences, damage/cost/cooldown defaults, ACT input/UI, roster ownership/eligibility, saving, auto-conversion of C cards and third-party assets. A successfully parsed asset is not a playable ACT character.

## Dependencies and handoff

No runtime predecessor: only existing Engine data-asset/soft-reference APIs. TASK-030 consumes these static definitions and supplies supported GAS executors and execution data. TASK-035 consumes character/weapon identity and visuals; its RosterData contains roster projections/adapters, not duplicate authoring definitions. Static content under ACTCharacterCards is transferred from TASK-035 to this task. TASK-030 ACT/Data and Definitions retain runtime execution/effect configuration only.

## Delegation

Read-only Architect checks schema/ownership and Reviewer checks code/data/tests. Root remains the only writer; no writable delegate is authorized.

## Acceptance criteria

- [x] Separate character, weapon and skill definition classes expose stable IDs, content version, display fields and typed soft references.
- [x] A typed ACT table links skill identity, definition and logical activation binding; wrong row types and missing/duplicate bindings fail.
- [x] Character model/animation/skill references and weapon presentation can be filled without C++ edits; missing content stays an invalid draft.
- [x] Validate loaded reference types, duplicate definition IDs, matching skill identity, skeleton compatibility and external executor/terrain IDs through a supplied read-only catalog.
- [x] Trait restrictions never silently reject the whole character or execute an unresolved terrain policy.
- [x] Editable empty templates and bilingual authoring instructions explain the remaining runtime requirements.
- [x] Editor compile, narrow tests, asset/template checks, bilingual audits and independent review pass.

## Verification and learning

Compile the Editor target. Run DreamOfPadma.MVP.TASK036 automation, inspect template asset types and DataTable RowStruct, run AuditDocs/ValidateProject -Strict/git diff --check. Test incomplete drafts, wrong tables, duplicate identities, dangling bindings and reference resolution. User check: create a character asset, assign the model/table and read the reported missing configuration; no combat success is claimed.

Learning target: distinguish a soft asset reference, a stable definition ID and a live battle instance. Predict whether replacing a model changes the character ID; inspect the asset and explain why a skill table still needs an executor. Status: Not started; user evidence pending.

## Risks and recovery

Soft references do not guarantee asset availability or cook inclusion. Runtime loading/cooking and GAS executor registration belong to their later owners. A table remains a thin binding index; do not insert mode calculations here. Recover only this task's exact diff; no generated-folder edits.

## Completion report

- Final status: Verified; root owns the static-authoring scope on Local codex/mvp-demo-foundations.
- Delivery: PadmaACTAuthoring.h/.cpp, ACTAuthoringTest.cpp, AuthorACTTemplates.py, five editable UE templates and the bilingual ACTAuthoring guide. Character/weapon/skill identities and soft references are separate from turn-based cards, live actors and roster state.
- Verification: UE5.8.2 Editor compile passed. DreamOfPadma.MVP.TASK036.AuthoringValidation passed in automation-reviewed/index.json with zero test warnings/failures. Tests cover incomplete drafts, wrong tables, repeated bindings, mismatched/duplicate identity, external reference IDs, explicit trait selectors and montage skeleton mismatch.
- Assets: author-act.log and author-finalize.log report zero commandlet errors/warnings. Existing-content rerun succeeded. The corrected script also passed the independent reviewer's in-memory valid-existing-catalog regression. Empty templates report missing content instead of playable readiness.
- Review: prior P2 rerun finding fixed; static schema ownership and exact paths reviewed. Independent read-only Reviewer /root/full_mvp_contract_review returned Pass on 2026-09-08 with no remaining P0–P2 findings; final evidence and all 28 implementation paths across TASK-008/036 were checked.
- Evidence directory: C:/Users/lenovo/.codex/visualizations/2026/09/08/01a07f68-77cf-7fe0-8479-93a66fb8a4f6/runtime-review; build-final.log, automation-reviewed.log and automation-reviewed/index.json, author-act.log, author-finalize.log.
- Learning: the guide explains soft asset references versus stable IDs and battle instances. User asset filling and teach-back remain pending.
- Remaining: user content and individual values, ACT GAS executors/input/actions, runtime loading/cooking, battle settings and terrain policy execution. No gameplay/GAS dependency or numerical value was added.
- Changelog: the Coordinator finalized the bilingual Added entry — editable ACT definitions, empty templates and authoring validation.
- Integration: working-tree candidate only; no commit, merge or push.
