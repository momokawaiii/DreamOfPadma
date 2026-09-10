# Dream of Padma Documentation Index

- Chinese companion for user reading: `Docs/00_INDEX.zh-CN.md`

Routing index: search for the needed topic; do not load every linked document. Routine startup uses ProjectState, the active TASK and relevant English sections. History and examples are optional.

## Current state

- [Project State](ProjectState.md) — implemented behavior and outstanding acceptance
- [Offline Demo roadmap](Production/Roadmap.md) and [dual-build acceptance](Production/BuildMatrix.md)
- [Chapter Zero scope and tutorial rules](Rules/ChapterZero.md)
- [ADR-0010 Demo content, baked maps and deferred editor](Decisions/ADR-0010-Offline-Demo-Content-and-Map.md)
- [TASK-053 grill-based technical documentation](Production/Tasks/TASK-053-Grill-Technical-Docs.md)
- [TASK-052 current click/input/PIE-exit fixes](Production/Tasks/TASK-052-Tutorial-Interaction-and-Motion-UI.md)
- [Native playable guide](Content/NativePlayableDemo.md), [map authoring](Content/WorldMapAuthoring.md), [strategy presentation](Content/StrategyPresentation.md)
- [Runtime naming](Architecture/RuntimeNaming.md), [Agent Workflow](Agent/Workflow.md), [Task Template](Production/TaskTemplate.md)
- [Changelog](Changelog.md) — earlier TASK-046–052 evidence and historical presentation decisions

## Design rules

- [Chapter Zero Demo](Rules/ChapterZero.md)

- [Time and Calendar](Rules/Time.md)
- [Card Synthesis](Rules/Synthesis.md)
- [Combat](Rules/Combat.md)
- [World State](Rules/WorldState.md)

## MVP design baseline - accepted

The high-level MVP scope and rule interpretation in these documents were accepted on 2026-09-02 and form the implementation baseline. Items explicitly marked `Open`, `Proposed`, `To confirm`, deferred, or playtest-only remain unresolved and must not be invented during implementation.

### English - Agent-facing

- [Main Planner](Design/EN/01_MainPlanner.md)
- [System Planner](Design/EN/02_SystemPlanner.md)
- [Combat and AI Planner](Design/EN/03_CombatAIPlanner.md)
- [Level and Content Planner](Design/EN/04_LevelContentPlanner.md)
- [Numerical Planner](Design/EN/05_NumericalPlanner.md)

### Chinese - user-facing companions

- [主策划案](Design/ZH/01_MainPlanner.md)
- [系统策划案](Design/ZH/02_SystemPlanner.md)
- [战斗/AI策划案](Design/ZH/03_CombatAIPlanner.md)
- [关卡/内容策划案](Design/ZH/04_LevelContentPlanner.md)
- [数值策划案](Design/ZH/05_NumericalPlanner.md)

## Architecture

- [Module Map](Architecture/ModuleMap.md)
- [Program Architecture Baseline](Architecture/ProgramArchitecture.md)
- [Data-Driven Architecture](Architecture/DataDrivenArchitecture.md)
- [Runtime Flow](Architecture/RuntimeFlow.md)
- [Save Schema](Architecture/SaveSchema.md)
- [Design Pattern Guidance](Architecture/DesignPatterns.md)
- [Architecture Agent Rules](Architecture/AGENTS.md)
- [Module Agent Rules](Architecture/Modules/AGENTS.md)

### Logical module program documents

- [PadmaCore](Architecture/Modules/PadmaCore/README.md)
- [PadmaGame](Architecture/Modules/PadmaGame/README.md)
- [PadmaGameplay](Architecture/Modules/PadmaGameplay/README.md)
- [Future ACT Development Agent Contract](Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md)
- [PadmaWorld](Architecture/Modules/PadmaWorld/README.md)
- [PadmaUI](Architecture/Modules/PadmaUI/README.md)
- [PadmaEditor](Architecture/Modules/PadmaEditor/README.md)
- [PadmaTests](Architecture/Modules/PadmaTests/README.md)

## Production and learning

- [Roadmap](Production/Roadmap.md)
- [Build Matrix](Production/BuildMatrix.md)
- [First Foundation Task](Production/Tasks/TASK-001-Foundation.md)
- [Agent Workflow Bootstrap Task](Production/Tasks/TASK-002-Agent-Workflow-Bootstrap.md)
- [First Core Rule Contract Slice](Production/Tasks/TASK-003-Core-Contracts.md)
- [ACT Architecture Decision Integration](Production/Tasks/TASK-004-ACT-Architecture-Decision.md)
- [Deterministic Random Foundation](Production/Tasks/TASK-005-Deterministic-Random-Foundation.md)
- [Playable Prototype Vertical Slice Cluster](Production/Tasks/TASK-006-Playable-Prototype-Cluster.md)
- [SLG World Selection and Transition](Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.md)
- [Encounter Character and Skill Data](Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.md)
- [Card Play and Character Summon](Production/Tasks/TASK-009-Card-Play-and-Character-Summon.md)
- [Playable Prototype Integration](Production/Tasks/TASK-010-Playable-Prototype-Integration.md)
- [Learning Curriculum](Learning/Curriculum.md)
- [Development Learning Loop](Learning/Workflow.md)
- [GM Commands](Tools/GMCommands.md)

## Full-rules MVP task cluster

- [TASK-011](Production/Tasks/TASK-011-Full-MVP-Task-Cluster.md): delivery matrix and dependency order
- [MVP Decision Register](Production/MVPDecisionRegister.md)
- [PDF card, rule and screen source extraction](Production/MVPSourceExtraction.md)
- [TASK-012](Production/Tasks/TASK-012-Run-State-and-Contracts.md): Run State and Shared Contracts
- [TASK-013](Production/Tasks/TASK-013-GAS-Infrastructure.md): Native GAS Infrastructure
- [TASK-014](Production/Tasks/TASK-014-Character-Presentation.md): Character Rendering and Animation
- [TASK-015](Production/Tasks/TASK-015-Combat-Calculation-Contracts.md): Combat Calculation Contracts
- [TASK-016](Production/Tasks/TASK-016-Encounter-Timeline.md): Encounter Action Timeline
- [TASK-017](Production/Tasks/TASK-017-Encounter-GAS-Playable-Loop.md): Playable Encounter with GAS
- [TASK-018](Production/Tasks/TASK-018-Calendar-and-Phase-Scheduler.md): Calendar and Phase Scheduler
- [TASK-019](Production/Tasks/TASK-019-Resource-and-Global-Ledgers.md): Resource and Global Ledgers
- [TASK-020](Production/Tasks/TASK-020-Card-Catalog-and-Lifecycle.md): Card Catalog and Lifecycle
- [TASK-021](Production/Tasks/TASK-021-ABC-Sandbox-Operations.md): ABC Sandbox Operations
- [TASK-022](Production/Tasks/TASK-022-Synthesis-Resolution.md): Synthesis Resolution
- [TASK-023](Production/Tasks/TASK-023-Synthesis-UI.md): Synthesis Preview and UI
- [TASK-024](Production/Tasks/TASK-024-Sandbox-Graph-and-Movement.md): Sandbox Graph and Movement
- [TASK-025](Production/Tasks/TASK-025-Node-Operations-and-Story.md): Node Operations and Story Mutations
- [TASK-026](Production/Tasks/TASK-026-Ruler-World-AI.md): Ruler World AI
- [TASK-027](Production/Tasks/TASK-027-Battle-Routing-and-Travel.md): Battle Routing and Scene Travel
- [TASK-028](Production/Tasks/TASK-028-Battle-Transactions.md): Battle Snapshot Transactions
- [TASK-029](Production/Tasks/TASK-029-Safe-Boundary-Save-Load.md): Safe-Boundary Save and Load
- [TASK-030](Production/Tasks/TASK-030-ACT-GAS-Playable-Loop.md): Playable ACT with GAS
- [TASK-031](Production/Tasks/TASK-031-ACT-Repository-and-Bullet-Time.md): ACT Repository and Bullet Time
- [TASK-032](Production/Tasks/TASK-032-Run-Outcomes-and-Restart.md): Run Outcomes and Restart
- [TASK-033](Production/Tasks/TASK-033-First-Era-Content-Pack.md): First Era Content Pack
- [TASK-034](Production/Tasks/TASK-034-Full-MVP-Integration.md): Full MVP Integration and Acceptance
- [TASK-035](Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.md): ACT Character Cards and Battle Roster

## Agent automation

- Project custom-Agent configuration: `.codex/config.toml` and `.codex/agents/*.toml`
- Task execution procedure: `.agents/skills/padma-task-runner/SKILL.md`
- Independent review procedure: `.agents/skills/padma-review/SKILL.md`
- Evidence-based learning procedure: `.agents/skills/padma-learning-loop/SKILL.md`

## Content

- [Asset Naming](Content/Naming.md)

## Decision records

- [ADR-0001 Project Structure](Decisions/ADR-0001-ProjectStructure.md)
- [ADR-0002 Encounter-First ACT Preparation](Decisions/ADR-0002-Encounter-First-ACT-Preparation.md)
- [ADR-0003 Fixed Playable Prototype Vertical Slice](Decisions/ADR-0003-Playable-Prototype-Vertical-Slice.md)
- [ADR-0004 Separate Encounter and ACT GAS](Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md)

The design-baseline rules are documented in [Design Agent Rules](Design/AGENTS.md).

The Chinese documentation index is available at [00_INDEX.zh-CN.md](00_INDEX.zh-CN.md).

- [ADR-0005 Layered World Map Storage](Decisions/ADR-0005-World-Map-Storage.md)


- [TASK-036 ACT authoring definitions](Production/Tasks/TASK-036-ACT-Authoring-Definitions.md)
- [ACT authoring guide](Content/ACTAuthoring.md)

## Map and global preparation

- [Screen design](Design/EN/06_MapAndPreparation.md)
- [TASK 037 Node Inspection and Local Scenes](Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.md)
- [TASK 038 Global Preparation and Bonus UI](Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.md)
- [TASK 039 Story Relationships and Bonus State](Production/Tasks/TASK-039-Story-Relationships-and-Bonus-State.md)

## UE model authoring preview

- [TASK-040 model assets and scene bindings](Production/Tasks/TASK-040-Model-Assets-and-Scene-Bindings.md)
- [Model authoring guide](Content/ModelAuthoring.md)

## Current build testing

- [TASK-052 tutorial interaction and motion UI](Production/Tasks/TASK-052-Tutorial-Interaction-and-Motion-UI.md)

- [UE and HTML test plan](Production/CurrentBuildTestPlan.md): launch points, current-function cases, known gaps and result recording
- [TASK-041 test plan delivery](Production/Tasks/TASK-041-Current-Build-Test-Plan.md)

## HTML rule prototype first

- [Playable HTML entry](../Prototypes/HTML/index.html)
- [Home/dialogue/synthesis play guide and proposed UE continuation](../Prototypes/HTML/README.md)
- [TASK-042 playable prototype](Production/Tasks/TASK-042-HTML-Playable-Prototype.md)
- [TASK-043 Encounter visual feedback](Production/Tasks/TASK-043-Encounter-Visual-Feedback.md)
- [TASK-044 targeting, card casting and character details](Production/Tasks/TASK-044-Encounter-Targeting-and-Card-Casting.md)

- [TASK-045 lean Agent workflow](Production/Tasks/TASK-045-Lean-Agent-Workflow.md)
