---
name: padma-review
description: Review a Dream of Padma task, branch, diff, rule change, architecture change, or documentation baseline for acceptance, regressions, rule fidelity, dependency safety, verification gaps, and bilingual consistency. Use before integration or when the user requests an audit or review.
---

# Padma Review

Review independently from the worker's narrative. Treat the task contract and canonical English documents as the evidence source.

## Review procedure

1. Read `AGENTS.md`, `Docs/00_INDEX.md`, `Docs/ProjectState.md`, the active task, and every canonical document named by it.
2. Inspect the actual diff and repository state. Do not assume claimed checks or files are complete.
3. Review only the assigned change, but report an adjacent issue when it directly invalidates acceptance or integration safety.
4. Check these lanes as applicable:
   - task scope and acceptance criteria;
   - fidelity to accepted game rules and preservation of deferred decisions;
   - ownership, dependency direction, data-driven boundaries, and ADR needs;
   - determinism, save migration, future network authority, and rollback behavior;
   - runtime errors, regressions, edge cases, test quality, and missing evidence;
   - UE asset/map ownership and generated-file hygiene;
   - synchronized English and Chinese Markdown meaning.
5. Run safe read-only checks or tests when available. State anything not verified.

## Findings format

Lead with findings, highest severity first:

- `P0`: data loss, repository damage, security issue, or unusable build.
- `P1`: acceptance failure, rule corruption, major regression, or unsafe public contract.
- `P2`: significant edge case, maintainability risk, or missing test likely to conceal a defect.
- `P3`: bounded improvement that does not block acceptance.

For each finding, provide the violated contract, exact file or section, impact, and a concrete verification or remediation direction. Avoid style-only findings.

Finish with:

- acceptance status: `Block`, `Conditional pass`, or `Pass`;
- checks run and checks not run;
- residual risks;
- required bilingual or task-state updates.

Remain read-only unless the user explicitly asks for a separate remediation task. Never merge or push as part of review.
