# TASK-000 Title

- Chinese companion for user reading: `Docs/Production/TaskTemplate.zh-CN.md`
- Status: `Backlog`
- Parent milestone or integration Goal: `TBD`
- Primary Role: `TBD`
- Primary Agent: assigned when work starts
- Branch/worktree: assigned when work starts; use `main` only for integration-owned work

## Goal

Describe one observable result. This task contract and its Goal must have the same acceptance boundary.

## Why now

Explain which milestone, dependency, risk, or learning prerequisite makes this the next useful unit.

## Read first

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- Relevant English rule, architecture, module, and decision documents

## Decision state

### Frozen inputs

- List accepted rules and public contracts this task must preserve.

### Deferred or open inputs

- List unresolved items this task must not invent.
- Identify the user or owner who can resolve each blocking decision.

## Scope

### Allowed paths and write set

- List exact files or the narrowest directories the Primary Agent may edit.

### Out of scope

- List adjacent features, refactors, assets, rules, and infrastructure excluded from this task.

## Dependencies and integration order

- Required predecessor tasks or commits:
- Consumers affected by this contract:
- Merge-before / merge-after constraints:
- Shared file, schema, map, asset, build, or Editor-session conflicts:

## Delegation plan

Leave empty when no subagent is useful. A subagent is a bounded work package inside this Goal, not a second owner of the Goal.

| Role | Work package | Authority | Expected output |
|---|---|---|---|
| TBD | Exact question or task slice | Read-only or exact allowed paths | Evidence, proposal, test design, or patch |

## Acceptance criteria

- [ ] The observable Goal is satisfied.
- [ ] No work outside the allowed paths or explicit scope is included.
- [ ] Applicable rule, architecture, test, documentation, and learning criteria below pass.

## Verification plan

### Automated checks

- Command or test and expected result

### Manual checks

- Reproducible user or Editor action and expected observation

### Required evidence

- Logs, test output, screenshot, trace, saved fixture, or review findings

## Learning contract

Choose no more than two primary targets.

| Target | Evidence required | Status |
|---|---|---|
| TBD | Prediction plus user-produced observation and teach-back | `Not started` |

- Learner's hands-on exercise:
- Transfer question:

Allowed learning status values are `Not started`, `Introduced`, `Practiced`, and `Demonstrated`. Agent-produced output alone cannot earn `Demonstrated`.

## Risks and recovery

- Main implementation or integration risks:
- Safe rollback or recovery method:
- Conditions that require stopping for user or architecture review:

## Completion report

- Final status:
- Primary Agent and Role:
- Changed files:
- Acceptance evidence:
- Checks run and results:
- Checks not run and reason:
- Review findings resolved or accepted:
- Remaining risks and open questions:
- English/Chinese documentation updated:
- Agent-produced learning evidence:
- User-produced learning evidence:
- Integration commit or handoff reference:
