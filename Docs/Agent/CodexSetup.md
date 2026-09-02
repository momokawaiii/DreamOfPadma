# Codex Project Setup and Roles

- Chinese companion for user reading: `Docs/Agent/CodexSetup.zh-CN.md`

## Purpose

This repository configures Codex as a coordinated development environment, not as a collection of chats that share hidden memory. New tasks can discover the same project contracts, Role profiles, and Skills from version-controlled files.

## Directory responsibilities

| Location | Meaning | What belongs here |
|---|---|---|
| `AGENTS.md` and nested `AGENTS.md` files | Mandatory instructions | Safety, boundaries, reading order, language contract, and directory-specific rules |
| `Docs/` | Mutable project truth | Design, rules, architecture, ADRs, task contracts, current state, review evidence, and learning evidence |
| `.codex/config.toml` | Project multi-Agent settings | Enablement and conservative concurrency defaults |
| `.codex/agents/*.toml` | Custom project Role profiles | Role name, routing description, core instructions, and default sandbox mode |
| `.agents/skills/<skill>/SKILL.md` | Reusable operating procedure | Stable steps for task execution, review, and learning |
| `Docs/Production/Tasks/` | Goal contracts | One bilingual `TASK-xxx` pair per observable delivery Goal |
| Git branch/worktree | Write isolation and history | One independently mergeable writable Goal and its commits; a managed worktree begins detached until a branch is created |

Do not duplicate current rules or progress inside Role files or Skills. Those facts change and belong in `Docs/`; profiles and Skills point Agents to the canonical documents.

## Project Agent settings

`.codex/config.toml` enables multi-Agent work, preserves interruption context, and permits at most four spawned subagent threads per parent session. The project does not pin a model or reasoning effort, so each task can use the current supported model or an explicit task-level choice.

Four concurrent subagents are a capacity ceiling. The project workflow initially permits no more than two simultaneous writable worktrees and only one UE build/Editor/PIE lane.

Codex detects repository Skill changes automatically; if a Skill does not appear, restart Codex. After changing project Agent configuration, restart or open a fresh task before relying on newly spawned sessions to use the new profile.

## Available Role profiles

| Codex name | Default access | Use for | Must not do |
|---|---|---|---|
| `chief_planner` | Read-only | Product intent, core loop, MVP scope, player-facing acceptance | Resolve deferred rules or choose implementation architecture |
| `system_planner` | Read-only | System semantics, transitions, invariants, cards, resources, time, outcomes | Invent missing values or conflate rules with implementation |
| `combat_ai_planner` | Read-only | Encounter/ACT behavior, timing, AI intent, combat-route scenarios | Fill deferred action precedence or card lifecycle |
| `level_content_planner` | Read-only | Sandbox graph, nodes, encounters, story mutations, content pacing | Claim shared maps/assets or invent unresolved canon |
| `numerical_planner` | Read-only | Formula domains, coefficients, fixtures, balance hypotheses, exploit analysis | Treat untested numbers as final or overwrite fixed formulas |
| `architect` | Read-only | Dependencies, contracts, data flow, save/network seams, ADR review | Change game meaning for implementation convenience |
| `module_worker` | Workspace-write | Execute one approved task inside one exclusive write set | Broaden scope, edit owned files of another Goal, merge, or push |
| `reviewer` | Read-only | Independent findings against task, rules, architecture, tests, and docs | Trust the worker summary without examining evidence or edit during review |
| `learning_tutor` | Read-only | Novice explanation, prediction, practice, evidence, teach-back, transfer | Perform the learner's exercise or expand feature scope |

The built-in `explorer` remains useful for fast read-only repository discovery. Role profiles describe specialists; they are not permanent module owners.

## Top-level tasks, Primary Agents, and subagents

A top-level Codex task is user-visible, long-lived collaboration space. Use a separate project task/worktree when a Goal writes an independently mergeable result. Put these fields in its opening prompt:

```text
Primary Role: module_worker
Goal: <one observable result>
Task contract: Docs/Production/Tasks/TASK-xxx-Name.md
Worktree: <selected when the project task starts>
Branch: <create in the task header before the result must persist or be pushed>
Allowed paths: <exact write set>
Required checks: <tests/manual evidence>
Learning targets: <one or two targets>
Stop if: <rule, ownership, or dependency condition>
```

Naming a Primary Role tells the task which viewpoint to use; the `TASK-xxx` file remains the enforceable contract. One Primary Agent owns the Goal from preflight through handoff. Codex-managed worktrees start in detached `HEAD`; use **Create branch here** before preserving the result as a branch or pushing it.

Inside that task, the Primary Agent may spawn a custom Role as a subagent. The delegation prompt must contain more than a role name:

```text
Role: architect
Work package: Check whether the proposed calendar contract crosses PadmaCore boundaries.
Inputs: TASK-xxx plus the named architecture and module documents.
Output: Findings with exact document references and a minimal recommendation.
Authority: Read-only.
Non-goals: Do not redesign calendar rules or edit files.
Return before: implementation begins.
```

The parent remains accountable for checking and integrating the answer. Use a new top-level task instead of a subagent when the result needs its own branch, acceptance criteria, several turns, or user ownership.

## Project Skills

- `$padma-task-runner`: preflight, execute, validate, teach, and hand off one task contract.
- `$padma-review`: independently review a task, branch, diff, design, or architecture change; findings come first.
- `$padma-learning-loop`: turn task learning targets into prediction, practice, evidence, teach-back, and transfer.

Agents may discover these Skills from `.agents/skills/`. A user or parent Agent can also invoke one explicitly by name. The English `SKILL.md` is executable guidance; `SKILL.zh-CN.md` is the synchronized user-facing mirror.

## Starting an integration task

Use one task on the saved project checkout as Integration Coordinator. Its job is to prepare contracts, create or coordinate independent worktrees, monitor results, order reviews, integrate commits, run shared validation, and update project state. It should not also become the default implementation worker for every module.

A suitable opening prompt is:

```text
Act as the Dream of Padma Integration Coordinator. Read AGENTS.md, Docs/00_INDEX.md,
Docs/ProjectState.md, Docs/Agent/Workflow.md, and the active integration task.
Do not implement module work in this checkout. Freeze task contracts, detect write-set
overlap, route independent Goals to worktrees, delegate bounded read-only specialist
reviews, merge only after evidence, and keep bilingual state synchronized.
```

## Permission and safety rules

- Choose the parent task's permission mode deliberately; spawned Agents may inherit active runtime permission overrides.
- Planner, architect, reviewer, and tutor profiles remain read-only by default.
- Only a worker with an approved exact write set may write.
- Never grant multiple Agents write access to the same file or binary UE asset.
- User approval is required before remote push, tag, release, destructive Git operation, or meaningful scope expansion.
- The user remains the decision owner for unresolved game design and the final acceptance owner for the playable result.

## Official mechanism references

- [Codex subagents and custom agents](https://learn.chatgpt.com/docs/agent-configuration/subagents)
- [Build Skills for Codex](https://learn.chatgpt.com/docs/build-skills)
- [Git worktrees](https://learn.chatgpt.com/docs/environments/git-worktrees)
- [Long-running work and Goals](https://learn.chatgpt.com/docs/long-running-work)
