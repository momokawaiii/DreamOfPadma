# TASK-011 Full MVP Task Cluster and Source Reconciliation

- Chinese companion: TASK-011-Full-MVP-Task-Cluster.zh-CN.md
- Document ID: TASK-011
- Version: 0.3
- Status: Verified
- Primary Role / Agent: Integration Coordinator / current primary documentation agent
- Parent milestone: complete playable rules MVP
- Workspace: current Local checkout; this contract owns documentation
- Authorization: user requested this cluster on 2026-09-08, confirmed separate GAS modes and card collections, then asked to extract the two source PDFs and implement the result. Runtime execution is bound to the corresponding child contract.

## Goal

Deliver 25 bilingual task contracts, TASK-011 through TASK-035: this planning/reconciliation task plus 24 implementation tasks. Publish [source extraction](../MVPSourceExtraction.md), [decision register](../MVPDecisionRegister.md), ADR-0004/0005 and synchronized rules/architecture/production entry points.

Version 0.3 extends that delivery with the screen design and TASK-037/038/039, incorporating node inspection, global preparation and minimum relationship bonuses into existing contracts, dependencies and acceptance. TASK-036 is the separately delivered static ACT authoring task.

The complete MVP covers one seven-day Era in the seven-Era Chapter structure, approximately eight shared sandbox nodes, at least two A/two B/two C cards plus a basic battle-skill set and a no-reward failure definition, resource operations, synthesis, world mutations/ruler responses, complete Encounter and ACT, safe-boundary persistence and both confirmed campaign victories. Explicit campaign defeat remains a required user decision, not a forced day-seven loss.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, Docs/Agent/Workflow.md, Docs/Agent/CodexSetup.md, TaskTemplate, the four Rules documents, the five English design baselines, ProgramArchitecture, DataDrivenArchitecture, ModuleMap, RuntimeFlow, SaveSchema, affected module READMEs, ACTDevelopmentContract and TASK-003 through TASK-010. Read the exact PDFs referenced in source extraction. English is canonical.

## Confirmed decisions and open boundaries

- Encounter and ACT both use native GAS with independent catalogs, row schemas, clocks, execution/state systems and mode-owned abilities/effects. Shared infrastructure does not require equal effects or one live ability/AttributeSet/cooldown.
- Turn-based/ABC deployment and ACT character cards are different collections. Battle settings selects ACT characters/weapons; shared basic non-A skill cards retain identity/slots and have separately configured effects per mode.
- ACT terrain restrictions name their target: trait, ability, weapon or whole character. A trait restriction does not silently ban the whole character.
- The user supplies placeholder assets after work starts. Final animation/rendering polish is later work. Every unaccepted number, including a temporary test value, needs individual user confirmation.
- SLG is an oblique Civilization VI-like view with nodes and preset connections over a hex-shaped map. This is not automatic hex-neighbor movement. The user accepted a static chapter definition asset, .umap presentation and SaveGame run-state changes (ADR-0005).
- The user excludes the old contract-card sandbox summon effects (mycard 6–7) from the first version pending redesign. Other source conflicts stay explicit; an old PDF passage does not silently override a later accepted rule.
- The PDFs have five explicitly named C-person cards but no separate ACT roster/weapon catalog. TASK-035/030 require that missing input; do not manufacture it by converting C cards.
- Encounter remains the first combat delivery. The existing 007–010 visual fixtures remain narrow. TASK-005 is required before the first random feature.

## Scope and exclusive write set

This task writes the 25 TASK pairs linked below (including this pair), plus exactly these documentation paths:

- `Docs/Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md`
- `Docs/Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md`
- `Docs/Decisions/ADR-0002-Encounter-First-ACT-Preparation.md`
- `Docs/Decisions/ADR-0002-Encounter-First-ACT-Preparation.zh-CN.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/ProgramArchitecture.zh-CN.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.zh-CN.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/RuntimeFlow.zh-CN.md`
- `Docs/Architecture/ModuleMap.md`
- `Docs/Architecture/ModuleMap.zh-CN.md`
- `Docs/Architecture/SaveSchema.md`
- `Docs/Architecture/SaveSchema.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md`
- `Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.zh-CN.md`
- `Docs/Rules/Combat.md`
- `Docs/Rules/Combat.zh-CN.md`
- `Docs/Design/EN/03_CombatAIPlanner.md`
- `Docs/Design/ZH/03_CombatAIPlanner.md`
- `Docs/Production/Roadmap.md`
- `Docs/Production/Roadmap.zh-CN.md`
- `Docs/ProjectState.md`
- `Docs/ProjectState.zh-CN.md`
- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`
- `Docs/Changelog.md`
- `Docs/Changelog.zh-CN.md`
- `Docs/Production/MVPDecisionRegister.md`
- `Docs/Production/MVPDecisionRegister.zh-CN.md`
- `Docs/Design/EN/01_MainPlanner.md`
- `Docs/Design/ZH/01_MainPlanner.md`
- `Docs/Design/EN/02_SystemPlanner.md`
- `Docs/Design/ZH/02_SystemPlanner.md`
- `Docs/Design/EN/04_LevelContentPlanner.md`
- `Docs/Design/ZH/04_LevelContentPlanner.md`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.zh-CN.md`
- `Docs/Production/MVPSourceExtraction.md`
- `Docs/Production/MVPSourceExtraction.zh-CN.md`
- `Docs/Rules/WorldState.md`
- `Docs/Rules/WorldState.zh-CN.md`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaWorld/README.zh-CN.md`
- `Docs/Decisions/ADR-0005-World-Map-Storage.md`
- `Docs/Decisions/ADR-0005-World-Map-Storage.zh-CN.md`

Source, Content, Config, build files, TASK-005 through TASK-010, generated folders, .agents and .codex are outside this documentation contract. The user's subsequent implementation request is executed under its own reviewed task write set. In particular, any travel/bootstrap gap found in the existing fixed slice must be assigned explicit paths before runtime work.

Another Coordinator finished TASK-007 and shared integration updates before this task changed existing shared documents. Preserve main at c946591 and TASK-007 Done / TASK-008 Ready as the preflight baseline. Do not revert the separately committed DefaultEngine.ini change (d8f107f). No merge, push, tag or release is authorized here.

## Delivery matrix

Task numbers are identities, not numerical execution order. New runtime contracts start Backlog; each needs its actual predecessor results and applicable decisions before Ready.

| TASK | Independent deliverable | Required predecessors | Decision gates |
|---|---|---|---|
| [012](TASK-012-Run-State-and-Contracts.md) | Run State and Shared Contracts | 010, 011, 040 | D04 |
| [013](TASK-013-GAS-Infrastructure.md) | Native GAS Infrastructure | 012 | D01, D04 |
| [014](TASK-014-Character-Presentation.md) | Character Rendering and Animation | 012, 040 | D02 |
| [015](TASK-015-Combat-Calculation-Contracts.md) | Combat Calculation Contracts | 012 | D03, D06, D22 |
| [016](TASK-016-Encounter-Timeline.md) | Encounter Action Timeline | 015 | D05, D06 |
| [017](TASK-017-Encounter-GAS-Playable-Loop.md) | Playable Encounter with GAS | 013, 014, 015, 016 | D01, D03, D05, D06 |
| [018](TASK-018-Calendar-and-Phase-Scheduler.md) | Calendar and Phase Scheduler | 012 | D07, D22 |
| [019](TASK-019-Resource-and-Global-Ledgers.md) | Resource and Global Ledgers | 005, 012, 018 | D03, D08 |
| [020](TASK-020-Card-Catalog-and-Lifecycle.md) | Card Catalog and Lifecycle | 005, 012, 018, 019, 040 | D01, D03, D09, D22 |
| [021](TASK-021-ABC-Sandbox-Operations.md) | ABC Sandbox Operations | 019, 020, 024 | D03, D10 |
| [022](TASK-022-Synthesis-Resolution.md) | Synthesis Resolution | 005, 019, 020, 021 | D03, D11, D22 |
| [023](TASK-023-Synthesis-UI.md) | Synthesis Preview and UI | 021, 022 |  |
| [024](TASK-024-Sandbox-Graph-and-Movement.md) | Sandbox Graph and Movement | 012, 019, 020 | D03, D12, D21, D24, D25 |
| [025](TASK-025-Node-Operations-and-Story.md) | Node Operations and Story Mutations | 018, 021, 024 | D03, D12, D19 |
| [026](TASK-026-Ruler-World-AI.md) | Ruler World AI | 005, 015, 018, 019, 024, 025 | D03, D13 |
| [027](TASK-027-Battle-Routing-and-Travel.md) | Battle Routing and Scene Travel | 017, 021, 024, 025, 026, 035 | D14, D24, D26 |
| [028](TASK-028-Battle-Transactions.md) | Battle Snapshot Transactions | 005, 018, 019, 020, 021, 022, 024, 025, 026, 027, 035 | D04, D15 |
| [029](TASK-029-Safe-Boundary-Save-Load.md) | Safe-Boundary Save and Load | 028, 035 | D04, D15 |
| [030](TASK-030-ACT-GAS-Playable-Loop.md) | Playable ACT with GAS | 013, 014, 015, 017, 027, 028, 035 | D01, D03, D04, D06, D16, D23 |
| [031](TASK-031-ACT-Repository-and-Bullet-Time.md) | ACT Repository and Bullet Time | 020, 030 | D01, D09, D17, D23 |
| [032](TASK-032-Run-Outcomes-and-Restart.md) | Run Outcomes and Restart | 019, 025, 026, 027, 029 | D03, D18 |
| [033](TASK-033-First-Era-Content-Pack.md) | First Era Content Pack | 023, 025, 026, 029, 031, 032, 035, 037, 038, 039 | D02, D03, D11, D12, D13, D18, D19, D22, D23, D24, D25, D26, D27, D28 |
| [034](TASK-034-Full-MVP-Integration.md) | Full MVP Integration and Acceptance | 005, 010, 012, 013, 014, 015, 016, 017, 018, 019, 020, 021, 022, 023, 024, 025, 026, 027, 028, 029, 030, 031, 032, 033, 035, 037, 038, 039 | ALL |
| [035](TASK-035-ACT-Character-Cards-and-Roster.md) | ACT Character Cards and Battle Roster | 012, 019, 020, 036, 040 | D01, D02, D03, D20, D23, D27 |

User clarification D23 adds [TASK-036](TASK-036-ACT-Authoring-Definitions.md) as a separate early static-authoring slice. It has no runtime predecessor, transfers static ACT character/weapon/skill assets from 035, and supplies soft-reference definitions to 035/030. It does not close playable-content or numerical gates. This follow-on contract is outside the original 102-path planning delivery manifest.

## Recommended execution order and visible checkpoints

1. Integrate the reviewed TASK-008 after authorization, then 009 → 010 for card/UI and fixed-slice acceptance. This is distinct from the full-map and global-preparation work.
2. 012 → 013/014/015 → 016 → 017 gives the first GAS Encounter. Independently injectable card/payment/lifecycle providers keep this stage testable; 020 supplies production providers and 034 binds them.
3. 018 → 019 → 020 → 024 → 021 → 022 → 023 gives the calendar, economy, graph/deployment and synthesis. TASK-005 precedes relevant random features. 035 can follow 012/019/020; 025 follows 018/021/024, then 026.
4. 027 → 028 → 029 connects reviewed world, Encounter and roster context with complete transactions and safe persistence.
5. 030 → 031 builds ACT after Encounter, routing, transactions and roster are available. It uses its own GAS mode definitions and the shared basic-skill mapping.
6. 032 supplies campaign outcomes, 033 authors the user-approved First Era pack, and 034 proves the complete playable run and recovery paths.

Map/preparation branch: 037 follows 012/014/024/025/040; 039 waits for its listed rule/story services and 038 waits for 035/039. TASK-012 first freezes the neutral bonus-source port; calculation owners implement their consumers with injected fixtures, and 034 later binds the real 039 producer without reverse task dependencies. 033/034 accept the complete screens/effects after these three new tasks are also complete.

No wall-clock estimate is implied: unresolved rule/numerical input and user asset import affect runtime progress. Ask the smallest next blocking question while continuing independent work.

TASK-012 freezes neutral card/payment/lifecycle ports before 017/030 consume, 020 implements and 034 injects them. TASK-019 owns entropy attribution/calamity value contracts, 025 applies world consequences and 026 consumes ruler-response events; 033/034 explicitly verify low-entropy safe and reproducible high-entropy benefit/calamity routes. All triggers and values still require the user's decision.

## MVP coverage

| Accepted requirement | Feature owner | Final observation |
|---|---|---|
| Calendar: Dawn/Morning/Dusk, seven-day Era and seven-Era structure | 018 | Advance a full First Era without skipping scheduled effects |
| Flow/Computation, entropy, faith, threat, war balance, core/stability | 019/026/032 | Separate ledgers, visible HUD and authored costs/consequences |
| ABC cards, basic skills, lifecycle, sandbox abilities and synthesis | 020–023 | Place/attach/use cards, inspect preview, resolve success or no-reward failure |
| Shared nodes, movement, conditional paths, story/furnace/safe-house, ruler AI | 024–026 | Visible ownership/state change and ruler response |
| Mode route, map change and complete commit/rollback | 027/028 | Both local modes return consistent state; failure/exit exactly restores |
| Character rendering/feedback and independent GAS mode execution | 013–017/030/031 | Visible Encounter timeline and ACT action plus Tab repository |
| Separate ACT collection, battle settings and terrain restrictions | 035/027/030 | Select characters/weapons; direct commands enforce trait restrictions |
| Safe save/load, both victories, explicit defeat and restart | 029/032 | Round trip, two winning routes, defeat and clean new run |
| Actual sourced cards/content and complete user-played session | 033/034 | Full seven-day economy/synthesis/world/battle/save/outcome route |

## Ownership and implementation handoff

One Primary Agent owns one task and its exact paths. Use a Local feature branch under the current operating policy; worktrees require an actual independent write-isolation need. Keep one UE build/Editor/PIE lane. Shared headers, mode schemas, module README pairs, maps, binary assets, build settings and final composition edits are serial leases.

State-owning features implement TASK-012 snapshot participation. TASK-035 distinguishes global presets from run-owned loadout before persistence. TASK-034 wires public providers/adapters; it does not repair child source internally. Feature defects return to their owner. An individual interrupt/death is handled by Encounter timeline rules and is not automatically a battle-ending result.

The documentation request and later implementation request supply task intent; unresolved gameplay and numerical choices still require the user's answer. Technical schema choices are reviewed by the Architect. Canonical paths needed to close a gate must be explicitly added at preflight, never treated as blanket write authority.

## Review and acceptance

Docs/Architecture/AGENTS.md requires architecture and affected-rule review. A read-only architect has reviewed mode/state boundaries and identified task/provider/lifetime corrections; a read-only final reviewer checks the actual files and bilingual consistency. Neither is a writer.

- [x] TASK-011–035 bilingual contracts have goal, exact paths, dependencies, acceptance, tests, learning and recovery.
- [x] Separate GAS modes, character collections, shared skills, placeholders and individual numerical authority are explicit.
- [x] Source PDFs are read and recorded; known conflicts are visible.
- [x] Every accepted requirement is covered without cyclic or hidden dependencies.
- [x] Existing shared rules/architecture/production entries are synchronized.
- [x] Companion, link/graph, strict project and whitespace checks pass.
- [x] Independent review has no unresolved P0–P2 findings.

## Verification

Run Scripts/AuditDocs.ps1, Scripts/ValidateProject.ps1 -Strict, a task-link/dependency/write-set audit and scoped git diff --check. AuditDocs only checks companion existence; links and translated meaning require separate checks. No UE build or PIE success is claimed by this documentation task.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| Shared infrastructure versus independent modes | Predict whether an ACT-only table edit should affect Encounter, inspect the boundary and explain a forbidden dependency. | Not started |
| Rule map versus rendered map | Trace NodeId/edge to displayed hex and saved ownership; explain why moving the camera cannot change adjacency. | Not started |

Practice: choose one source card and trace its identity, legal mode, effect owner and pending fields through its TASK. Transfer: explain how another mode can reuse basic-skill identity without inheriting ACT sequences. Agent documents do not demonstrate user learning.

## Risks and recovery

Main risks are cross-mode coupling, stale source rules, missing definitions, hidden dependencies and overlapping shared writers. Resolve a concrete conflict before implementing it; progress on independent approved work. Recovery changes only this task's documented diff after rechecking ownership; preserve all unrelated work.

## Version 0.2 completion report

- Final status: Verified; documentation checked and independently reviewed. Subsequent D22d/D23 user decisions are recorded; runtime work uses separate contracts.
- Changed files: the 25 linked TASK English/Chinese pairs and the exact additional path manifest above.
- Checks: AuditDocs passed (182 Markdown / 91 pairs); strict project validation and git diff --check passed; 102-path / 942-link / 24-child dependency and bilingual audit passed without cycles. Architect, System, Numerical and final Reviewer returned Pass for documentation only. No runtime or user learning evidence claimed.
- Architectural corrections incorporated in child contracts: independent mode providers, typed card/entity identities, interrupt/death lifetime, ACT roster run-state classification and direct-command terrain restriction validation.
- User-produced learning evidence: not provided.
- Integration: working-tree delivery; no merge or remote action.

## Version 0.3 map and preparation amendment

The latest user request adds hover/local modeled scenes and global ACT/FPS/relationship-bonus views. The current root remains the sole documentation writer. Besides the original relevant paths above, this amendment owns exactly:

- `Docs/Design/EN/06_MapAndPreparation.md`
- `Docs/Design/ZH/06_MapAndPreparation.md`
- `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.md`
- `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.zh-CN.md`
- `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.md`
- `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.zh-CN.md`
- `Docs/Production/Tasks/TASK-039-Story-Relationships-and-Bonus-State.md`
- `Docs/Production/Tasks/TASK-039-Story-Relationships-and-Bonus-State.zh-CN.md`

The original v0.2 completion report is historical evidence; v0.3 has its own review. [Screen design](../../Design/EN/06_MapAndPreparation.md) separates accepted requirements, PDF evidence and proposals. D24–D28 keep unanswered behavior and values open. TASK-008/036 remain Verified; no merge is inferred.

| Task | Deliverable | Required predecessors | Gates |
|---|---|---|---|
| [037](TASK-037-Node-Inspection-and-Local-Scenes.md) | Node inspection/local scenes | 012, 014, 024, 025, 040 | D02, D12, D21, D24, D25 |
| [038](TASK-038-Global-Preparation-and-Bonus-UI.md) | Global preparation/bonus views | 012, 019, 020, 035, 039 | D20, D23, D26, D27, D28 |
| [039](TASK-039-Story-Relationships-and-Bonus-State.md) | Minimum relationship/bonus state | 012, 015, 018, 019, 020, 025 | D03, D19, D27, D28 |

033/034 also require 037/038/039. 037 exports navigation intent without depending on 038 implementation; 034 binds real providers. FPS combat awaits D26. Earlier approximate node/content counts remain planning targets subject to user layout/content decisions. This amendment claims no new UE runtime verification.

## Version 0.3 completion report

- Final status: Verified for the documentation amendment only. The screen design remains Draft, TASK-037/038/039 remain Backlog, and unanswered parts of D24–D28 remain Open.
- Delivery: 46 existing Markdown paths updated and 8 added, totaling 27 pairs. The exact manifest follows; Source, Content and configuration were not changed.
- Sources: reverified both PDF paths and hashes under Desktop/cd; mygame pages 3, 5, 6–11 support screen/information design, with later user decisions overriding conflicting old rules.
- Review: independent read-only architecture review returned Pass with no remaining P0–P2. TASK-012's neutral bonus port and consumer ownership are explicit, D28 selects TASK-039's effect target, and stale UI click-to-move wording is corrected. The main delivery matrix mirrors the added dependencies/gates.
- Checks: AuditDocs passed (194 Markdown / 97 pairs), strict project validation and git diff --check passed; 1,034 local links and 28 follow-on task dependency/bilingual-metadata checks passed for the complete working-tree candidate, with no dependency cycles. That candidate totals 144 paths including previous deliveries; this amendment is limited to the 54 documentation paths below.
- Design preview: the conversation visualization passed six browser scenarios at widths 1024/736/360 in light/dark themes; hover, node entry/return, global preparation tabs and shared-skill mode switching passed without horizontal overflow. It is not UE runtime, animation or rule-acceptance evidence.
- Pending: click/movement semantics, enemy disclosure and first-version FPS scope were asked; configuration timing/state ownership and concrete relationship effects/numbers still require individual user decisions.
- Learning: no user-produced practice/teach-back evidence. No commit, merge or push, and no new UE run; TASK-008/036 retain their earlier independent evidence.

| English path | Chinese path |
|---|---|
| `Docs/00_INDEX.md` | `Docs/00_INDEX.zh-CN.md` |
| `Docs/Architecture/Modules/PadmaCore/README.md` | `Docs/Architecture/Modules/PadmaCore/README.zh-CN.md` |
| `Docs/Architecture/Modules/PadmaUI/README.md` | `Docs/Architecture/Modules/PadmaUI/README.zh-CN.md` |
| `Docs/Decisions/ADR-0005-World-Map-Storage.md` | `Docs/Decisions/ADR-0005-World-Map-Storage.zh-CN.md` |
| `Docs/Design/EN/04_LevelContentPlanner.md` | `Docs/Design/ZH/04_LevelContentPlanner.md` |
| `Docs/Design/EN/06_MapAndPreparation.md` | `Docs/Design/ZH/06_MapAndPreparation.md` |
| `Docs/Production/MVPDecisionRegister.md` | `Docs/Production/MVPDecisionRegister.zh-CN.md` |
| `Docs/Production/MVPSourceExtraction.md` | `Docs/Production/MVPSourceExtraction.zh-CN.md` |
| `Docs/Production/Roadmap.md` | `Docs/Production/Roadmap.zh-CN.md` |
| `Docs/Production/Tasks/TASK-011-Full-MVP-Task-Cluster.md` | `Docs/Production/Tasks/TASK-011-Full-MVP-Task-Cluster.zh-CN.md` |
| `Docs/Production/Tasks/TASK-012-Run-State-and-Contracts.md` | `Docs/Production/Tasks/TASK-012-Run-State-and-Contracts.zh-CN.md` |
| `Docs/Production/Tasks/TASK-014-Character-Presentation.md` | `Docs/Production/Tasks/TASK-014-Character-Presentation.zh-CN.md` |
| `Docs/Production/Tasks/TASK-015-Combat-Calculation-Contracts.md` | `Docs/Production/Tasks/TASK-015-Combat-Calculation-Contracts.zh-CN.md` |
| `Docs/Production/Tasks/TASK-019-Resource-and-Global-Ledgers.md` | `Docs/Production/Tasks/TASK-019-Resource-and-Global-Ledgers.zh-CN.md` |
| `Docs/Production/Tasks/TASK-020-Card-Catalog-and-Lifecycle.md` | `Docs/Production/Tasks/TASK-020-Card-Catalog-and-Lifecycle.zh-CN.md` |
| `Docs/Production/Tasks/TASK-022-Synthesis-Resolution.md` | `Docs/Production/Tasks/TASK-022-Synthesis-Resolution.zh-CN.md` |
| `Docs/Production/Tasks/TASK-024-Sandbox-Graph-and-Movement.md` | `Docs/Production/Tasks/TASK-024-Sandbox-Graph-and-Movement.zh-CN.md` |
| `Docs/Production/Tasks/TASK-025-Node-Operations-and-Story.md` | `Docs/Production/Tasks/TASK-025-Node-Operations-and-Story.zh-CN.md` |
| `Docs/Production/Tasks/TASK-027-Battle-Routing-and-Travel.md` | `Docs/Production/Tasks/TASK-027-Battle-Routing-and-Travel.zh-CN.md` |
| `Docs/Production/Tasks/TASK-033-First-Era-Content-Pack.md` | `Docs/Production/Tasks/TASK-033-First-Era-Content-Pack.zh-CN.md` |
| `Docs/Production/Tasks/TASK-034-Full-MVP-Integration.md` | `Docs/Production/Tasks/TASK-034-Full-MVP-Integration.zh-CN.md` |
| `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.md` | `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md` |
| `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.md` | `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.zh-CN.md` |
| `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.md` | `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.zh-CN.md` |
| `Docs/Production/Tasks/TASK-039-Story-Relationships-and-Bonus-State.md` | `Docs/Production/Tasks/TASK-039-Story-Relationships-and-Bonus-State.zh-CN.md` |
| `Docs/ProjectState.md` | `Docs/ProjectState.zh-CN.md` |
| `Docs/Rules/WorldState.md` | `Docs/Rules/WorldState.zh-CN.md` |

## 2026-09-09 authoring continuation

D24/D25/D26 are answered: inspection-only click, separate right-side card movement, full enemy node disclosure, FPS future and ACT playable. D29 fixes three-state mobility and A/attached-B per-card group validation; D03 permits clear nonconflicting mycard.pdf values. [TASK-040](TASK-040-Model-Assets-and-Scene-Bindings.md) adds independent UE model assets/preview. TASK-012/014/020/035/037 now consume 040; consult their required-predecessor lists. Its direct predecessors are the existing Verified 008/036 working-tree schemas, without implied integration. The earlier HTML preview remains design evidence only.
