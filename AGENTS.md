# Dream of Padma Agent Contract

- Chinese companion: AGENTS.zh-CN.md

## Entry and authority

- Work in the current Local checkout by default. The old E:/2026ue/padma prototype is read-only reference material.
- Primary startup, at the first substantive work in a task: consult Docs/00_INDEX.md as a routing index, read Docs/ProjectState.md and the active TASK or user request, then relevant directory instructions and source sections. Reuse unchanged content already in context; after resuming, verify current Git state and affected files.
- User decisions take precedence over older project prose. Preserve accepted rules; report real code/data/rule contradictions and continue independent work while the affected decision is unresolved.
- Rules live in Docs/Rules; architecture in Docs/Architecture and Docs/Decisions; current state in Docs/ProjectState.md; per-task evidence in Docs/Production/Tasks. Source and UE assets remain in Source and Content/Padma.
- Routine execution uses Docs/Agent/Workflow.md. Read CodexSetup only to configure roles/tools; read learning material when teaching is requested. Legacy TASK read lists are routing indexes, not instructions to load every linked document. Their specific acceptance criteria, technical checks and unresolved decisions still apply.

## Execution boundaries

- One Primary owns a coherent delivery; the same agent may coordinate and implement serial work. Small follow-ups stay in that TASK. New top-level chats require the user's request.
- State the scope and exact allowed paths, inspect Git, and preserve unrelated edits. No overlapping writers to files, schemas, maps or assets. Keep one UE build/Editor/PIE lane.
- Delegate only a useful independent package. Give its outcome, source paths/sections, exact read/write authority and required evidence. Children use that package plus applicable directory rules; they do not repeat project-wide onboarding or close the parent TASK.
- Review and learning follow the conditional policy in Workflow.md. This supersedes generic mandatory reviewer/teaching/worktree boilerplate in older task templates, not task-specific technical acceptance or permission requirements.
- Do not merge, force-push, tag or push without explicit user authorization. No destructive cleanup of unrelated files. Do not edit generated Binaries, DerivedDataCache, Intermediate, Saved or .vscode folders.
- External assets in Content/ThirdParty stay unchanged; create project wrappers in Content/Padma.

## Game architecture invariants

- PadmaCore has no UMG, Niagara, concrete Actor, map or UGameplayStatics dependency.
- UI sends commands; services validate/mutate state and emit events. Presentation does not own gameplay settlement.
- Stable IDs identify cards, nodes, encounters, quests and saves; no Actor pointers in save data.
- Reproducible outcomes use seeded randomness. No universal Manager for new global state.
- Preserve approved mode/data boundaries. Structural changes require an ADR; routine implementation within an accepted boundary does not.

## Documentation and completion

- English is canonical. Every Markdown file has a synchronized Chinese companion: Name.md / Name.zh-CN.md, or the established Design/EN / Design/ZH pair. Keep ID, version, status and meaning aligned. Read English for decisions; inspect Chinese to update or check its translation.
- Put scope, acceptance and evidence once in the TASK. Update only documents whose facts changed; ProjectState is a short current-state summary, not a work log.
- Run the narrow checks selected by Workflow.md and AuditDocs.ps1 after Markdown edits. Completion requires the requested behavior and relevant checks; report unverified runtime behavior honestly.
- Final handoff: outcome, changed paths, checks, remaining user action/risk. No routine learning report when no lesson was requested.
