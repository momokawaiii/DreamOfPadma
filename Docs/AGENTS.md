# Documentation Rules

- English owns detailed rules/contracts; Chinese companions explain the main points in short, everyday language. They need not match paragraph by paragraph or repeat logs/tables. Keep IDs, versions, important decisions, status and open limits consistent; link the English detail.
- Existing Chinese files are retained as summaries. New user-facing long-term rules, architecture, authoring guides and entry pages need a Chinese summary. New internal TASKs, history and evidence may be English-only. A Chinese file must always have its English source.
- Put gameplay meaning in Rules, structural decisions in Decisions, reusable procedures in the relevant guide, current priorities in ProjectState, and raw evidence in ignored Artifacts. Keep each fact in one owning place.
- Follow Agent/Workflow.md for execution and documentation. Small fixes need no new TASK. Update only documents whose useful facts changed; no automatic changelog, index or README updates.
- Keep active TASKs short and current. Preserve useful superseded evidence in History and link it; old reports are not startup instructions. Add index links only for useful navigation.
- Run Scripts/AuditDocs.ps1 after Markdown changes. It checks the language-summary policy, not translation quality or factual correctness.
