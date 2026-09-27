# Dream of Padma Agent Contract

- Work in the current Local checkout; E:/2026ue/padma is read-only reference.
- Start from the user request, Git state and Docs/ProjectState.md. Use Docs/00_INDEX.md only to locate relevant rules/source. Read an existing TASK only when it owns the work; do not create one for routine fixes.
- User decisions override older prose. Rules live in Docs/Rules; architecture in Docs/Architecture and Docs/Decisions. Old task scope/status is historical, not a fresh authorization gate.
- One Primary owns delivery and chooses investigation, implementation and necessary refactoring. Follow Docs/Agent/Workflow.md for checks, documentation and handoff; do not repeat its process in directory rules.
- Preserve unrelated edits and explicit user limits. Assign disjoint writes when delegation is authorized; keep one UE build/Editor/PIE lane. New chats require the user's request.
- Do not merge, force-push, tag or push without explicit authorization. Do not edit generated Binaries, DerivedDataCache, Intermediate, Saved or .vscode. No unrelated destructive cleanup.
- Keep ThirdParty assets unchanged; project wrappers belong in Content/Padma. Follow Content/AGENTS.md for experimental assets and UE moves.
- Core has no UMG, Niagara, concrete Actor, map or UGameplayStatics dependency. UI sends commands; services validate and settle state. Use stable IDs in saves, seeded gameplay randomness and explicit ownership; no universal Manager. Record consequential structural changes in an ADR.
- English owns detailed contracts. Existing Chinese companions are short plain-language summaries, not literal translations. Preserve important decisions, status and unresolved limits; link English for detail. New internal TASK/history/evidence files need no Chinese companion. See Docs/AGENTS.md.
