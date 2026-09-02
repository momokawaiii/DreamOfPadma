# Dream of Padma

- Chinese companion for user reading: `README.zh-CN.md`

Dream of Padma is a UE5 C++ learning and development project focused on a data-driven world simulation, card synthesis, tactical combat, and switchable RTS/ACT/FPS presentation modes.

## Current status

The repository contains the blank C++ UE project, the initial project-governance scaffold, and an accepted high-level MVP design and architecture baseline. Explicitly deferred parameters remain open. The next milestone is foundation verification followed by the Core rules slice: calendar, resources, deterministic random streams, card definitions, and synthesis tests.

## Repository rules

- Read `AGENTS.md` before working.
- Read `Docs/00_INDEX.md` to locate the current design and architecture documents.
- Use `Scripts/ValidateProject.ps1` for a quick repository check.
- Use `Scripts/AuditDocs.ps1` after Markdown changes to verify English/Chinese pairs.
- Use `Scripts/RunTests.ps1` and `Scripts/PackageDevelopment.ps1` only when an Unreal Engine root is configured.
- Keep third-party assets separate from project-owned assets.
- Every Markdown file has an English source and a synchronized Chinese companion; Agents read the English source.

## Initial validation

```powershell
Scripts\ValidateProject.ps1
```

The Unreal project is currently associated with the engine version recorded in `DreamOfPadma.uproject`. Engine upgrades require an ADR in `Docs/Decisions/`.
