# Agent Workflow

- Chinese companion for user reading: `Docs/Agent/Workflow.zh-CN.md`

## Shared context

Codex conversations are workers; the repository is the shared source of truth. Agents must not rely on hidden conversation history. Use Git, task files, `ProjectState.md`, ADRs, and test results to communicate.

Authority flows from the current user request to `AGENTS.md`, accepted English design/rule documents, architecture and ADRs, the active task contract, and then implementation evidence. A lower layer may refine but must not silently contradict a higher layer. Chinese documents mirror the English source for user review.

## Coordination vocabulary

| Unit | Purpose | Ownership |
|---|---|---|
| Milestone | Groups outcomes that produce one project capability | Integration Coordinator |
| Integration Goal | Coordinates dependent Goals and owns final repository state | One integration owner |
| Goal | One observable, independently acceptable result | One Primary Agent |
| TASK contract | Versioned scope, decisions, write set, acceptance, verification, and learning evidence for one Goal | Primary Agent; integration owner closes it |
| Worktree | Git isolation for one independently mergeable writable Goal | One Primary Agent; temporary, not a permanent module home |
| Role | A specialist viewpoint and authority boundary | May be reused by many Goals or Agents |
| Subagent | A temporary worker for one bounded package inside a Goal | Parent/Primary Agent remains accountable |
| Skill | A reusable procedure for repeated work | Repository-level process asset, not current project truth |

The hierarchy is:

```text
Project
└─ Milestone
   └─ Integration Goal / integration owner
      ├─ Goal + TASK + worktree + Primary Agent
      │  ├─ read-only specialist subagent
      │  └─ bounded worker subagent when its write set is exclusive
      └─ Goal + TASK + worktree + Primary Agent
```

## Work decomposition

Parallel work should use non-overlapping ownership:

- Core rules and tests
- Gameplay and combat
- World and map
- UI and presentation
- Tools, build, and documentation

Do not parallel-edit the same UE map, `.uasset`, central config file, or public data schema without an integration task.

Use this routing test before creating work:

| Shape | Use when | Result ownership |
|---|---|---|
| Independent Goal/worktree | The output has its own acceptance criteria, spans multiple turns, writes files, and can be reviewed/merged independently | Its Primary Agent |
| Subagent work package | The question is bounded and contributes evidence, a proposal, test design, or a small exclusive patch to the parent's Goal | Parent/Primary Agent |
| Serial integration work | Work touches a shared schema, map, `.uasset`, central config, module dependency, migration, or the same write set | Integration owner or one designated writer |

Initial concurrency policy:

- `.codex/config.toml` permits up to four spawned subagent threads per parent session. This is a safety ceiling, not a target.
- Start with no more than two simultaneous writable worktrees until integration evidence shows the boundaries are stable.
- Use one UE build/Editor/PIE lane at a time. Read-only analysis can continue while it is occupied, but another Agent must not mutate files consumed by that build.
- `Docs/ProjectState.md`, roadmap state, shared task indexes, central schemas, maps, and shared UE assets have one integration writer.

## Task lifecycle

```text
Backlog → Ready → In Progress → Review → Verified → Done
```

Every task needs a goal, allowed paths, non-goals, acceptance criteria, test plan, and learning targets.

- `Backlog`: idea recorded, not authorized for implementation.
- `Ready`: decisions needed for this scope are recorded; dependencies, write set, acceptance, verification, and learning targets are complete.
- `In Progress`: one Primary Agent owns the Goal/worktree and has announced its write set.
- `Review`: implementation is stable; an independent reviewer inspects the actual diff and evidence.
- `Verified`: blocking findings are resolved or explicitly accepted, and required checks pass.
- `Done`: the approved result is integrated, the task completion report and shared state are updated, and the commit is ready for remote backup.

A Goal is not made Ready by guessing an open rule. If an unresolved item affects acceptance or a public contract, stop and ask the named decision owner. If it does not affect the current slice, list it as deferred and keep it out of scope.

## Primary Agent and subagent contract

The Primary Agent is accountable for the whole Goal even when work is delegated. Every subagent request must include:

1. Role and exact work package;
2. required inputs and canonical documents;
3. expected output and evidence format;
4. constraints, non-goals, and stop conditions;
5. read-only or exact write authority;
6. dependency on other work and return deadline/order.

A persona alone is not a work package. The parent verifies subagent output against repository evidence before using it. Subagents do not change Goal scope, close the task, merge, or update shared milestone state.

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
| Integration Coordinator | ProjectState, Production, Tasks, Git evidence | Goal routing, write-set isolation, merge order, shared validation, and final state |
| Chief Planner Agent | Design/EN/01_MainPlanner.md | Product intent, core loop, scope, and acceptance |
| System Planner Agent | Design/EN/02_SystemPlanner.md and Rules/ | System semantics, state transitions, and rule contracts |
| Combat/AI Planner Agent | Design/EN/03_CombatAIPlanner.md | Combat vocabulary, encounter behavior, and ruler policy intent |
| Level/Content Planner Agent | Design/EN/04_LevelContentPlanner.md | Sandbox graph, node content, story mutations, and pacing |
| Numerical Planner Agent | Design/EN/05_NumericalPlanner.md | Coefficients, tables, curves, fixtures, and balance evidence |
| Architect Agent | Architecture/ProgramArchitecture.md, DataDrivenArchitecture.md, and ModuleMap.md | Program boundaries, data pipeline, dependencies, persistence, and extension seams |
| Module Agent | Architecture/Modules/<Module>/README.md | Module implementation, local contracts, tests, and learning evidence |
| Review Agent | Architecture, Decisions, Production, and changed module docs | Cross-document consistency, dependency safety, and acceptance evidence |
| Learning Tutor Agent | Learning/Curriculum.md, Learning/Workflow.md, and task learning contract | Novice orientation, practice, evidence, teach-back, and transfer |

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

## Integration flow

1. The integration owner records or approves task contracts on the integration branch before parallel writes begin.
2. Freeze shared contracts needed by downstream Goals. Contract changes after fan-out return to serial integration work.
3. Create one worktree per independently mergeable writable Goal and assign one Primary Agent to each.
4. Primary Agents may delegate bounded analysis or exclusive patches, then validate their own branch and produce completion reports.
5. A separate Review Agent checks the actual diff, acceptance evidence, dependencies, rule fidelity, and bilingual documents.
6. The integration owner merges in dependency order, never by whichever Agent finishes first.
7. Run consolidated validation in the single build/Editor lane. Resolve integration defects in a new scoped task or by returning the owning Goal to `In Progress`.
8. Update `Docs/ProjectState.md`, close the task, commit the integration state, and push only with user authorization.

Do not use chat summaries as handoff. Use commit hashes, task completion reports, review findings, tests, and explicit unresolved questions.

## Cross-module change flow

1. A planner changes a rule meaning or acceptance criterion.
2. The English design/rule source and Chinese mirror are updated.
3. The Architect Agent maps the change to a data or runtime contract.
4. Each affected Module Agent updates its own README, code, data, and tests.
5. A structural change receives an ADR.
6. The Review Agent checks dependencies, save/network effects, and bilingual synchronization.
7. The integration owner updates ProjectState and closes the task.

Module Agents should not edit the central architecture baseline to record local implementation details. The Architect Agent should not change a rule merely to make an implementation easier.
