# Documentation Rules

- Chinese companion for user reading: `Docs/AGENTS.zh-CN.md`

- Keep normative gameplay rules in `Docs/Rules/`.
- Treat `Docs/Design/EN/` and `Docs/Design/ZH/` as review drafts until the user accepts them; do not treat open items as canonical rules.
- Keep English design drafts and their Chinese companions synchronized by document ID and version.
- Every Markdown file must have an English source and a Chinese companion. Agents read English for task reasoning; Chinese is a user-facing mirror only.
- For normal documents use `Name.md` and `Name.zh-CN.md` in the same directory. Keep the existing `Docs/Design/EN/` and `Docs/Design/ZH/` convention for design drafts.
- Modify both members of a Markdown pair in the same task and commit. Run the documentation-pair audit after changes.
- Keep architecture decisions in `Docs/Architecture/` and `Docs/Decisions/`.
- Follow `Docs/Architecture/AGENTS.md` and `Docs/Architecture/Modules/AGENTS.md` for technical-document and module-Agent ownership.
- Keep one task per file under `Docs/Production/Tasks/`.
- Do not silently change a canonical rule while implementing code; update the rule document and add an ADR when the change is structural.
- Keep `Docs/ProjectState.md` short and current.
- Link new documents from `Docs/00_INDEX.md`.
