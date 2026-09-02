---
name: padma-task-runner
description: Execute or close one Dream of Padma TASK contract with bounded scope, safe delegation, bilingual documentation, validation evidence, and an integration-ready handoff. Use when starting, implementing, resuming, verifying, or completing a TASK-xxx file in this repository.
---

# Padma Task Runner

Use one task contract as the unit of delivery. Mutable project facts remain in `Docs/`; this skill supplies only the repeatable procedure.

## 1. Establish the contract

1. Read `AGENTS.md`, `Docs/00_INDEX.md`, `Docs/ProjectState.md`, and the active English `TASK-xxx` file.
2. Read the English rule, architecture, module, and decision documents named by the task.
3. Confirm one observable Goal, one Primary Agent, allowed paths, non-goals, dependencies, acceptance criteria, verification, and one or two learning targets.
4. Inspect Git status and report unrelated or overlapping changes. Never erase or absorb them silently.
5. Do not implement a `Backlog` task. Move forward only when the task is `Ready` or `In Progress` and required user decisions are recorded.

## 2. Choose execution shape

- Keep the work in the current Goal when it has one acceptance boundary and one coherent write set.
- Ask the parent to create an independent Goal/worktree when a result can be reviewed and merged independently, needs several turns, and has its own write set.
- Delegate a subagent only for a bounded work package. Give it a Role, question or task slice, inputs, expected output, constraints, and read/write authority.
- Prefer read-only subagents for exploration, planning, architecture review, test design, and review.
- Do not let two writers edit the same file, UE asset, map, public schema, or central configuration.

## 3. Execute and teach

1. State the intended paths before changing them.
2. Make the smallest coherent change that satisfies the contract.
3. Preserve accepted rules and explicitly deferred details. Stop on a code/data/document conflict.
4. Keep every changed Markdown English/Chinese pair synchronized.
5. Use the task's learning targets: explain the concept, obtain a prediction, give the learner a small hands-on check, and record evidence.

## 4. Verify and hand off

1. Run the narrowest relevant tests or reproducible manual checks.
2. After Markdown changes, run `Scripts/AuditDocs.ps1`.
3. Run `Scripts/ValidateProject.ps1 -Strict` when repository structure or shared configuration changes.
4. Review the diff against allowed paths and acceptance criteria.
5. Update the task completion report. Only the integration owner updates shared milestone state.
6. Report changed files, checks, evidence, remaining risks, unresolved questions, and learning evidence.

Never merge, force-push, create a release tag, or push to a remote unless the user explicitly authorizes that action.
