# End-to-End Multi-Agent Workflow Example

- Chinese companion for user reading: `Docs/Agent/WorkflowExample.zh-CN.md`

## Status of this example

This is an illustrative M1 scenario, not an active backlog and not authorization to create `TASK-003` through `TASK-006` or write code. Exact paths and contracts must be verified when implementation begins.

## Scenario

The integration outcome is: **the first core-rule foundation can represent calendar progression, resource transactions, and reproducible random outcomes without presentation dependencies**.

This outcome is too large for one undifferentiated Agent, but its shared contracts must be settled before implementation can safely fan out.

## Phase 1: integration contract Goal

The user opens one Codex task in the saved project checkout and assigns it the Integration Coordinator role.

```text
Goal: Prepare and integrate the M1 core-rule foundation.
Do not implement module code in this checkout.
First create bilingual task contracts, freeze shared IDs/value types and dependency
direction, identify write-set conflicts, and route only independently mergeable work
to project worktrees. Use $padma-task-runner and require $padma-review before merge.
```

The coordinator proposes task contracts such as:

| Illustrative task | Goal | Execution shape |
|---|---|---|
| `TASK-003 Core Contracts` | Freeze shared IDs, value semantics, error/result shape, ownership, and tests expected by downstream work | Serial integration work; shared public contract |
| `TASK-004 Calendar Slice` | Advance and reproduce the fixed phase/day/era/chapter calendar contract | Independent writable Goal after TASK-003 |
| `TASK-005 Resource Ledger Slice` | Apply explicit, inspectable resource transactions and reject invalid operations according to its contract | Independent writable Goal after TASK-003 |
| `TASK-006 Deterministic Random Fixtures` | Reproduce rule outcomes from a recorded seed and fixture | Independent writable Goal after TASK-003; may need to merge before consumers |

`TASK-003` is completed serially because every downstream Goal consumes its public contract. The coordinator obtains system-planner and architect read-only reviews, records any ADR need, gets user approval for design-impacting questions, and commits the frozen contract before creating writer worktrees.

## Phase 2: controlled fan-out

Once `TASK-003` is frozen, create separate Codex project tasks/worktrees for independently mergeable Goals, each starting from the frozen integration commit. Codex-managed worktrees begin in detached `HEAD`; use **Create branch here** before the task's result is committed for long-term branch use or pushed. Start only two writable Goals at once under the initial policy:

| Worktree Goal | Primary Role | Exclusive write set | Read-only support |
|---|---|---|---|
| Calendar slice | `module_worker` | Calendar-owned implementation/tests, its module README pair, and its task report | `system_planner`, `architect`, `learning_tutor` |
| Deterministic random fixtures | `module_worker` | Random/fixture-owned implementation/tests, its module README pair, and its task report | `numerical_planner`, `architect`, `reviewer` |

The resource-ledger Goal stays `Ready` until one writable slot is free. Parallelism is used to reduce waiting, not to maximize the number of open tasks.

An opening prompt for the Calendar Primary Agent is:

```text
Primary Role: module_worker
Goal: satisfy the Calendar Slice task and nothing else.
Task contract: Docs/Production/Tasks/TASK-004-Calendar-Slice.md
Use $padma-task-runner. Read the canonical English time rule and PadmaCore module
document. Announce the exact write set before editing. Preserve the fixed calendar
hierarchy and every deferred item. Delegate read-only rule/architecture checks if
useful. Stop if the frozen public contract must change or another Goal owns a file.
Return tests, task evidence, bilingual updates, learning evidence, and a commit hash.
Do not merge or push.
```

## Phase 3: bounded subagent work

Before writing, the Calendar Primary Agent can run two independent read-only packages in parallel:

```text
Role: system_planner
Work package: Derive calendar invariants and boundary fixtures from the accepted Time rule.
Inputs: the task, Time rule, ProjectState, and relevant system-design section.
Output: invariant table, boundary cases, and unresolved items with exact references.
Authority: read-only. Do not invent deferred rules.
```

```text
Role: architect
Work package: Check the proposed Calendar ownership, dependencies, deterministic state,
and save/network seams against the architecture baseline.
Inputs: the task and named architecture/module documents.
Output: blockers, ADR needs, and a minimal contract recommendation.
Authority: read-only. Do not edit files or change game semantics.
```

The Primary Agent compares both results with repository evidence. If they agree with the task, implementation proceeds. If either exposes a required public-contract change, the Goal stops and returns the issue to the Integration Coordinator; the worker does not patch a shared header opportunistically.

The `learning_tutor` then scopes the lesson to at most two targets, for example:

- how a value-type calendar state differs from a concrete world Actor;
- why a seeded deterministic fixture makes rule behavior reproducible.

The user predicts one boundary transition, runs or observes the approved test in a later implementation turn, and explains the result. The task records user evidence separately from Agent-generated test output.

## Phase 4: branch verification and independent Review

The Primary Agent runs narrow tests, bilingual documentation audit when needed, diff-scope checks, and records the completion report. It commits only its task write set.

A separate read-only `reviewer` receives:

```text
Use $padma-review. Review TASK-004 and its branch against main.
Inspect the actual diff and test evidence. Prioritize calendar-rule fidelity,
dependency direction, determinism, save implications, missing boundary fixtures,
write-set violations, and bilingual drift. Report findings first and do not edit.
```

The branch may move from `Review` to `Verified` only after every blocking finding is fixed or explicitly accepted by the authorized owner.

## Phase 5: integration

The Calendar and deterministic-random branches finishing at the same time does not determine merge order. The coordinator follows dependency order:

1. confirm each reviewed commit hash and clean worktree;
2. merge deterministic random fixtures first if Calendar tests consume that contract;
3. run the shared validation lane;
4. merge Calendar;
5. run the shared validation lane again or the consolidated checks required by the integration task;
6. start Resource Ledger only when its write slot and dependencies are clear;
7. update bilingual task state and `Docs/ProjectState.md`;
8. commit the integration result;
9. show the user the diff, checks, risks, and learning evidence;
10. push only after user authorization.

If a merge reveals a semantic conflict, create a small integration-fix task or return the owning Goal to `In Progress`. Do not make an undocumented compromise on `main`.

## What is parallel and what remains serial

```text
Serial:      contract freeze
                 |
Parallel:   Calendar Goal <----> Random Fixtures Goal
                 |                       |
Serial:      independent Reviews + dependency-ordered integration
                 |
Next wave:   Resource Ledger Goal + another non-overlapping Goal
```

Read-heavy planning, architecture checking, test design, and Review are excellent subagent work. Independent implementation Goals use separate worktrees. Shared public schemas, maps, `.uasset` files, central configuration, migration, Editor sessions, and final integration remain serial.

## Expected user interaction

The user does not need to manually relay every chat message. The repository and commits carry the durable facts. The user intervenes at the decisions that matter:

- approve or change task contracts and gameplay meaning;
- choose among true design alternatives;
- perform the planned hands-on learning exercise and teach-back;
- judge the playable/manual result;
- authorize integration push or milestone tag.
