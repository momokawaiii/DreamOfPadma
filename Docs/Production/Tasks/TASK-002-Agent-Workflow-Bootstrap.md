# TASK-002 Agent Workflow Bootstrap

- Chinese companion for user reading: `TASK-002-Agent-Workflow-Bootstrap.zh-CN.md`
- Status: `Done`
- Parent milestone or integration Goal: M0 foundation and collaboration readiness
- Primary Role: Integration Coordinator
- Primary Agent: current documentation task
- Branch/worktree: `main`

## Goal

Make the approved Goal/worktree/Primary Agent/subagent/Role/Skill model executable through project-scoped Codex configuration, reusable Skills, bilingual operating documents, and a verifiable task contract.

## Why now

The frozen MVP baseline is ready to enter implementation, but parallel implementation must not begin until ownership, write isolation, review, learning evidence, and Git handoff are explicit.

## Read first

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Architecture/ModuleMap.md`

## Decision state

### Frozen inputs

- The repository is shared memory; hidden chat history is not authoritative.
- One independently mergeable write Goal has one Primary Agent and normally one task contract/worktree.
- Subagents receive a Role plus a bounded work package.
- Stable reusable procedures become Skills; mutable game truth remains in `Docs/`.
- Every tracked Markdown document has a synchronized English/Chinese pair.

### Deferred or open inputs

- Permanent human or Agent ownership of logical modules remains unassigned.
- Concurrency limits may be tuned after real integration evidence; the initial policy is conservative.
- No gameplay implementation decision is in scope.

## Scope

### Allowed paths and write set

- `.codex/`
- `.agents/skills/`
- `.gitattributes`
- `AGENTS.md` and `AGENTS.zh-CN.md`
- `README.md` and `README.zh-CN.md`
- `Docs/00_INDEX.md` and `Docs/00_INDEX.zh-CN.md`
- `Docs/Agent/`
- `Docs/Learning/`
- `Docs/Production/TaskTemplate*`
- This task pair and `Docs/ProjectState*`

### Out of scope

- UE C++, Blueprints, maps, `.uasset` files, runtime configuration, and gameplay data
- Changing the frozen MVP design or resolving deferred game rules
- Packaging or release work
- Creating the first implementation task

## Dependencies and integration order

- Depends on the accepted MVP design/architecture baseline and private Git remote baseline.
- This task must be integrated before parallel implementation Goals are created.
- No concurrent writer owns this documentation/configuration write set.

## Delegation plan

- Official Codex documentation defined the current configuration, Skill, worktree, and Goal contracts.
- The `skill-creator` procedure initialized all three project Skills.
- Two bounded read-only reviewer runs were attempted before commit. Neither returned findings within the bounded waits, so both were shut down and are not counted as acceptance evidence. The Integration Coordinator completed the actual diff review against official documentation; this bootstrap-only review exception must not be copied into implementation tasks.

## Acceptance criteria

- [x] Project-level multi-Agent settings exist under `.codex/`.
- [x] Five planner profiles plus architect, module worker, reviewer, and learning tutor profiles have explicit boundaries.
- [x] Task execution, review, and learning Skills exist under `.agents/skills/`.
- [x] Goal, task, worktree, Primary Agent, Role, subagent, Skill, review, integration, and Git responsibilities are documented bilingually.
- [x] The task template records write sets, delegation, verification, risks, and user learning evidence.
- [x] A concrete but non-active parallel-development example exists.
- [x] No UE source or asset file is modified.

## Verification plan

### Automated checks

- Invoke the `skill-creator` validator for all three Skills. If its runtime dependency is unavailable, record the limitation and run an equivalent frontmatter/name/description/TODO/directory structural check rather than claiming the validator passed.
- Parse all `.codex/*.toml` files.
- Run `Scripts/AuditDocs.ps1`.
- Run `Scripts/ValidateProject.ps1 -Strict`.
- Run `git diff --check` and inspect changed paths.

### Manual checks

- Confirm the English documents form a complete Agent path from project entry to task, execution, review, integration, learning, and push.
- Confirm every new user-facing explanation has a Chinese companion.

### Required evidence

- Passing command output and an explicit changed-path audit in the completion report.

## Learning contract

| Target | Evidence required | Status |
|---|---|---|
| Distinguish Role, Goal, task, worktree, subagent, and Skill | User can route one example feature into the correct units | `Introduced` |
| Use a safe Git upload cycle | User performs the first post-baseline status/stage/review/commit/push cycle | `Introduced` |

- Learner's hands-on exercise: run the documented upload cycle after reviewing this task's commit.
- Transfer question: decide whether a future card-UI investigation should be a read-only subagent package, an independent write Goal, or serial integration work.

## Risks and recovery

- Custom-Agent configuration may evolve with Codex; official documentation must be rechecked before changing its schema.
- Excessive parallelism can still cause semantic conflicts even with separate files; integration order remains mandatory.
- This change is recoverable through Git because it contains documentation and project-local configuration only.

## Completion report

- Final status: `Done`
- Primary Agent and Role: current Agent, Integration Coordinator
- Changed files: project Agent configuration, three Skills, bilingual Agent/Git/learning/task documents, and state/index contracts
- Acceptance evidence: generated structure and all acceptance items above
- Checks run and results: Markdown pair audit passed for 100 files/50 pairs; strict project validation passed; all 10 TOML files parsed and the 8-read-only/1-writer Role policy passed; three Skills passed equivalent structural checks; local Markdown links passed; `git diff --check` and the no-UE-path scope audit passed
- Checks not run and reason: the provided `quick_validate.py` was invoked but could not import `PyYAML` in the offline bundled Python, and network installation was blocked; UE compile, PIE, and packaging are unaffected and outside this documentation-only task
- Review findings resolved or accepted: coordinator review corrected the inaccurate claim that a Codex-managed worktree automatically owns a branch; official documentation confirms it starts in detached `HEAD`. Independent reviewer subagents produced no result and were shut down, so no independent-pass claim is made for this bootstrap task
- Remaining risks and open questions: the first implementation cycle must demonstrate a successful bounded independent Review; Role/concurrency tuning and subagent responsiveness require real evidence
- English/Chinese documentation updated: yes
- Agent-produced learning evidence: executable workflow, templates, and example
- User-produced learning evidence: pending the first real task and Git push exercise
- Integration commit or handoff reference: this task's documentation-baseline commit
