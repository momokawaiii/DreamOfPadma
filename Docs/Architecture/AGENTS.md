# Architecture Documentation Agent Contract

- Chinese companion for user reading: `AGENTS.zh-CN.md`

## Purpose

This directory defines how the game design becomes a maintainable Unreal implementation. It is the technical source of truth for module boundaries, runtime ownership, data contracts, persistence, and integration. It must not silently redefine a gameplay rule owned by `Docs/Rules/`, contradict the accepted baseline in `Docs/Design/EN/`, or invent an explicitly unresolved item.

## Documentation ownership

| Document area | Primary owner | Required reviewers |
|---|---|---|
| `ProgramArchitecture.md` | Lead Programmer / Architect Agent | Review Programmer, affected module agents |
| `DataDrivenArchitecture.md` | Lead Programmer / Architect Agent | System, Numerical, and affected module agents |
| `ModuleMap.md`, `RuntimeFlow.md`, `SaveSchema.md`, `DesignPatterns.md` | Architect Agent | Review Programmer |
| `Modules/<Module>/README.md` | The named Module Agent | Architect Agent for boundary or contract changes |
| `Docs/Decisions/` | Decision owner named by the ADR | Architect Agent and affected design owner |

Module Agents maintain their own module documents. They do not directly rewrite the central architecture baseline or another module's contract.

## Required read order

Before changing architecture documentation or module code, read:

1. `../00_INDEX.md`
2. `../ProjectState.md`
3. `ProgramArchitecture.md`
4. `DataDrivenArchitecture.md` when data or assets are involved
5. `ModuleMap.md`, `RuntimeFlow.md`, and `SaveSchema.md` as relevant
6. The target module's `Modules/<Module>/README.md`
7. The relevant English design and rule documents

## Boundary rules

- Design documents define intent, rule meaning, constraints, and acceptance criteria.
- Architecture documents define data representation, runtime ownership, dependencies, and extension seams.
- Module documents define implementation responsibilities, public contracts, tests, and learning evidence.
- Static definitions must be separated from mutable run state.
- Commands enter the rules/application layer; presentation must not mutate authoritative state directly.
- Cross-module communication uses stable contracts, commands, results, or events rather than concrete object reach-through.
- A structural change requires an ADR before implementation or in the same change.
- A new UE module is created only when its boundary has stable ownership or an independent build/test need. Documentation folders are not a reason to create empty runtime modules.

## Change protocol

1. Identify the owning document and module.
2. State whether the change is a rule change, data-contract change, dependency change, or implementation detail.
3. Update the central architecture document only when the project-wide contract changes.
4. Update the affected module document and tests.
5. Add or update an ADR for structural decisions.
6. Update the English document and its Chinese companion in the same task.
7. Run `Scripts/AuditDocs.ps1` and the narrowest relevant project or test validation.

## Module document minimum

Every module document must state:

- Purpose and current implementation status.
- Owning Agent and allowed source/content paths.
- Responsibilities and explicit non-responsibilities.
- Public data, command, result, and event contracts.
- Dependencies and forbidden dependencies.
- Static data assets and runtime state ownership.
- Test, debug, save, and future network expectations.
- Learning targets and known risks.

The English file is canonical for Agent reasoning. The Chinese file is a synchronized user-facing mirror.
