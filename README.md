# Dream of Padma

- Chinese companion for user reading: `README.zh-CN.md`

Dream of Padma is a UE5 C++ learning and development project focused on a data-driven world simulation, card synthesis, tactical combat, and switchable RTS/ACT/FPS presentation modes.

## Current status

The retained runnable scope is the **turn-based L_PadmaWorld/L_PadmaBattle pair**, **L_ChenACT**, and the **PadmaNPR** plugin. Retired previews, the HTML application and old task records are outside the project; see [cleanup and recovery](Docs/Production/ProjectCleanup.md). Main-map source data is `Scripts/Data/PlayableData.cjs`; validate it with `node Scripts/Editor/ExportPlayableData.cjs --check`.

The repository contains the native UE project, an accepted high-level MVP design and architecture baseline, Role profiles, optional task handoffs, and Git handoff rules. The Primary chooses implementation and exploration methods within the authorized outcome; routine delivery, review and teaching need no project Skill wrapper. See [ProjectState](Docs/ProjectState.md) for current delivery priorities and remaining work. Explicitly deferred parameters remain open.

## Repository rules

- Read `AGENTS.md` before working.
- Read `Docs/00_INDEX.md` to locate the current design and architecture documents.
- Read `Docs/Agent/CodexSetup.md` and `Docs/Agent/Workflow.md` before coordinating multiple Agents or worktrees.
- Follow `Docs/Agent/GitWorkflow.md` for staging, committing, integrating, and GitHub backup.
- Use `Scripts/ValidateProject.ps1` for a quick repository check.
- Use `Scripts/AuditDocs.ps1` after Markdown changes to check the Chinese-summary policy.
- Use `Scripts/RunTests.ps1` and `Scripts/PackageDevelopment.ps1` only when an Unreal Engine root is configured.
- Keep third-party assets separate from project-owned assets.
- English owns detailed contracts. Chinese companions are brief plain-language summaries; new internal task/history/evidence files may be English-only. See Docs/AGENTS.md.

## Initial validation

```powershell
Scripts\ValidateProject.ps1
```

The Unreal project is currently associated with the engine version recorded in `DreamOfPadma.uproject`. Engine upgrades require an ADR in `Docs/Decisions/`.
