# Agent Workflow

- Chinese companion for user reading: `Docs/Agent/Workflow.zh-CN.md`

## Shared context

Codex conversations are workers; the repository is the shared source of truth. Agents must not rely on hidden conversation history. Use Git, task files, `ProjectState.md`, ADRs, and test results to communicate.

## Work decomposition

Parallel work should use non-overlapping ownership:

- Core rules and tests
- Gameplay and combat
- World and map
- UI and presentation
- Tools, build, and documentation

Do not parallel-edit the same UE map, `.uasset`, central config file, or public data schema without an integration task.

## Task lifecycle

```text
Backlog → Ready → In Progress → Review → Verified → Done
```

Every task needs a goal, allowed paths, non-goals, acceptance criteria, test plan, and learning targets.

## Completion report

The worker reports changed files, commands/tests, screenshots or packaged-build evidence where relevant, unresolved risks, and documentation changes. The integration task updates `Docs/ProjectState.md`.

## Language synchronization

Agents read English documents for task reasoning. Every Markdown change must update its Chinese companion in the same task and commit. The Chinese file is a user-facing mirror, not an independent source of truth.

## Decision control

Changes to module dependencies, save schema, canonical rules, asset layout, or build targets require an ADR before implementation or in the same change.

## Role and documentation routing

The project uses role ownership rather than one Agent editing every document:

| Role | Primary documentation | Main responsibility |
|---|---|---|
| Chief Planner Agent | Design/EN/01_MainPlanner.md | Product intent, core loop, scope, and acceptance |
| System Planner Agent | Design/EN/02_SystemPlanner.md and Rules/ | System semantics, state transitions, and rule contracts |
| Combat/AI Planner Agent | Design/EN/03_CombatAIPlanner.md | Combat vocabulary, encounter behavior, and ruler policy intent |
| Level/Content Planner Agent | Design/EN/04_LevelContentPlanner.md | Sandbox graph, node content, story mutations, and pacing |
| Numerical Planner Agent | Design/EN/05_NumericalPlanner.md | Coefficients, tables, curves, fixtures, and balance evidence |
| Architect Agent | Architecture/ProgramArchitecture.md, DataDrivenArchitecture.md, and ModuleMap.md | Program boundaries, data pipeline, dependencies, persistence, and extension seams |
| Module Agent | Architecture/Modules/<Module>/README.md | Module implementation, local contracts, tests, and learning evidence |
| Review Agent | Architecture, Decisions, Production, and changed module docs | Cross-document consistency, dependency safety, and acceptance evidence |

The English documents are the Agent-facing source. Chinese companions are synchronized for the user and are never an independent design authority.

## Documentation dependency chain

    Design intent
        -> Rules and system contracts
        -> Program and data architecture
        -> Module technical README
        -> Production task
        -> Code / asset / test evidence
        -> ProjectState update

The design layer answers what and why. The architecture layer answers how, where, and who. A module README answers how one owned boundary is implemented. A task file records one executable change and its acceptance evidence.

## Cross-module change flow

1. A planner changes a rule meaning or acceptance criterion.
2. The English design/rule source and Chinese mirror are updated.
3. The Architect Agent maps the change to a data or runtime contract.
4. Each affected Module Agent updates its own README, code, data, and tests.
5. A structural change receives an ADR.
6. The Review Agent checks dependencies, save/network effects, and bilingual synchronization.
7. The integration owner updates ProjectState and closes the task.

Module Agents should not edit the central architecture baseline to record local implementation details. The Architect Agent should not change a rule merely to make an implementation easier.
