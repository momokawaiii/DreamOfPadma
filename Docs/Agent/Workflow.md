# Agent Workflow

- Chinese companion: Workflow.zh-CN.md
- Current procedure: TASK-045; replaces generic process defaults in older examples/tasks. Specific game decisions and technical acceptance remain.

## Default loop

Read the current state and active TASK -> inspect affected sources -> implement -> run relevant checks -> update the TASK -> user playtest where applicable.

One Primary can coordinate and implement. Keep serial work in the current Local checkout; use a worktree only when actual concurrent independent writes need isolation or the user requests it. New chats are user-requested, not an automatic consequence of task length. No planner/reviewer/tutor relay for routine changes.

## Context budget

- AGENTS contains stable constraints; ProjectState contains current facts; TASK contains this change; roles contain specialties; Skills contain procedures.
- Use the index to find a source, not to load the library. Read the relevant English sections only. Verify current affected files; reuse unchanged source content already read.
- A TASK input list should name the decision or section needed. Older long lists are lookup aids. Do not reread all predecessor reports or both languages at startup.
- Resume from outcome, current files, checks and next action. Query actual state if it can have changed.
- Tool output should return matching sections or compact results. Do not repeatedly print whole files, unchanged status, or full successful logs.

## Delegate when it pays

Use a child for an independent bounded question or disjoint patch when saved time, expertise or context isolation justifies the extra model work. Keep dependent steps local. Start with one useful child; the configured capacity is a ceiling, not a target.

Where supported, prefer fork_turns=none with a self-contained package; otherwise supply only relevant history. Do not omit constraints merely to shorten the prompt. The package includes: result, source paths/sections and relevant decisions, authority, checks, and a short return format. Writers are told that others share the codebase and must preserve/adapt to their edits.

Children read applicable AGENTS and assigned sources, not the whole onboarding bundle. They return paths, conclusion/change, evidence and unresolved risk. Reuse a suitable existing child for follow-up; do not rediscover its completed work without a concrete discrepancy. The Primary reconciles interfaces and remains responsible for delivery.

## Verification and review

| Change | Minimum useful evidence | Independent review |
|---|---|---|
| Text, layout, reversible visual configuration | Relevant syntax/asset validation and a reproducible user check | On request or a concrete additional risk |
| Bounded gameplay logic | Tests for changed rules/state transitions; Editor compile for changed native code | When the risk below applies |
| Save/rollback, determinism, public contracts, GAS ownership/lifecycle, module/build dependencies, shared binary integration | Targeted regression cases plus applicable build/runtime evidence | One focused read-only pass before integration, unless the user explicitly chooses another acceptance route |

- User playtesting covers interaction/layout; it does not imply C++ compilation or save/rollback correctness.
- Reuse checks that still apply to unchanged code/environment. Rerun affected checks after changes or failures; do not repeat a full suite just because another role joined.
- AuditDocs after Markdown changes. ValidateProject -Strict for repository structure/shared configuration changes. UE build/Editor/PIE stays serial.
- Review examines the actual assigned diff and evidence. Recheck fixes/affected boundaries, not another full project review. A user waiver changes review, not required technical checks.
- Report unknowns accurately. No screenshots, videos, teach-back or extra approval solely to satisfy a generic checklist.

## TASK and state

A new delivery uses the compact TaskTemplate. Small fixes/clarifications within the current acceptance boundary amend the existing TASK. A read-only explanation needs no artificial implementation TASK.

Backlog = not authorized; Ready = relevant decisions and scope sufficient; In Progress = owned; Review = an actual reviewer or user check remains; Verified = applicable checks/required acceptance passed; Done = separately authorized integration completed. Review can be skipped when no separate gate applies. Never infer integration/push authorization from Verified.

A current user request can authorize a scoped Backlog task; record the applicable decisions and promote it before writing. Ask only for missing game meaning, a real scope/ownership conflict or required authorization; an old status alone is not a reason to ask again.

Record outcome, paths, check results and remaining work in the TASK once. The Primary also owns shared state when acting as the serial coordinator. Update ProjectState for priority/status/decision changes; update rules/architecture only when their meaning changes; add one concise Changelog entry for delivered behavior or workflow. English/Chinese pairs stay synchronized.

Learning is optional during delivery. Explain when requested; use the full prediction/practice/teach-back loop only for a chosen lesson, not because an old TASK has a learning section. See Docs/Learning/Workflow.md when teaching.

## Reusable prompts

User:
~~~text
Continue TASK-xxx on the current Local checkout.
Outcome: <observable behavior or correction>.
Acceptance: <what I will see/test>.
Constraints: <new constraints only>.
Teach only <topic, if wanted>.
~~~

Child:
~~~text
Bounded package of TASK-xxx: <result>.
Inputs: <paths/sections + relevant accepted decisions>.
Authority: read-only OR write only <paths>; parent owns task/state closure.
Check: <necessary evidence>. Return: paths, result, check, unresolved risk.
You are not alone: preserve others' changes and adapt to them.
~~~

Role setup and official references: CodexSetup.md. Historical examples are optional learning material.
