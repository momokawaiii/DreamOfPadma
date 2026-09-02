# Design Baseline Rules

- Chinese companion for user reading: `Docs/Design/AGENTS.zh-CN.md`

- `Docs/Design/EN/` contains the Agent-facing English design baseline.
- `Docs/Design/ZH/` contains synchronized Chinese companions for user review.
- Agents read the English design baseline for task reasoning; Chinese files exist for the user's convenience.
- The high-level scope and rule interpretation in the five MVP documents were accepted on 2026-09-02 and are the implementation baseline.
- Keep the same document ID, version, status, and meaning in each language pair.
- Update English and Chinese companions in the same change whenever a rule or scope item changes.
- Do not implement an item marked `Open`, `Proposed`, or `To confirm` as a canonical rule.
- Resolve conflicts with `mygame.pdf`, `mycard.pdf`, or later user clarifications in the review record before changing normative rule documents.
- When the accepted baseline changes, update the relevant files under `Docs/Rules/`, `Docs/Architecture/`, and `Docs/ProjectState.md`; create an ADR for structural decisions.
- Link every new design document from `Docs/00_INDEX.md`.
