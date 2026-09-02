# Module Documentation Agent Contract

- Chinese companion for user reading: `AGENTS.zh-CN.md`
- Parent rules: `../AGENTS.md`

## Purpose

Each directory under `Modules/` is the technical home for one logical module Agent. A module directory may exist before the corresponding C++ module, Actors, or assets exist. In that case the README records the planned boundary and must say that implementation is not yet present.

## Ownership

- The Module Agent owns its module README, implementation notes, tests, and explicitly allowed source/content paths.
- The Architect Agent owns cross-module boundaries, dependency direction, shared contracts, and structural ADRs.
- The Review Agent checks that code, data, design rules, and module documentation agree.
- No Module Agent may silently move a rule into another module, add a universal manager, or make a new runtime module only for naming symmetry.

## Required module workflow

1. Read the parent architecture rules and the module README.
2. Read the relevant English design/rule documents.
3. State the task scope and affected paths.
4. Implement or document the smallest coherent change.
5. Update the English and Chinese module documents together.
6. Add tests or a reproducible manual check.
7. Request architecture review for any public contract, dependency, save, or network change.

## Module README sections

Keep the module README current with:

- Purpose and status.
- Owner and allowed paths.
- Responsibilities and non-responsibilities.
- Public data and runtime contracts.
- Commands, events, and integration points.
- Dependencies and forbidden dependencies.
- Data assets and validation.
- Tests, debug commands, save/network behavior.
- Implementation plan, risks, and learning targets.

The English README is the Agent-facing source. The Chinese README is a user-facing mirror and may not introduce independent semantics.
