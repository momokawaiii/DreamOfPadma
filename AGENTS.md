# Dream of Padma Agent Contract

- Chinese companion for user reading: `AGENTS.zh-CN.md`

## Project purpose

This repository is the clean C++ UE5 foundation for Dream of Padma. The previous prototype at `E:\2026ue\padma` is reference material only. Do not modify or bulk-copy it unless a task explicitly says so.

## Read order

Before changing files, read:

1. `Docs/00_INDEX.md`
2. `Docs/ProjectState.md`
3. The task file under `Docs/Production/Tasks/` or the user request
4. The relevant module and directory instructions

## Source of truth

- Rules: `Docs/Rules/`
- Architecture: `Docs/Architecture/`
- Production state: `Docs/ProjectState.md` and `Docs/Production/`
- Learning evidence: `Docs/Learning/`
- Runtime implementation: `Source/`
- UE assets: `Content/Padma/`
- External assets: `Content/ThirdParty/`

## Documentation language contract

- Every tracked Markdown document must have an English source and a Chinese companion.
- English is the canonical source for Agents and implementation decisions.
- Agents read the relevant English documents for a task. Chinese companions are for the user's reading convenience and are not an independent source of truth.
- A normal Markdown file uses the same-directory pair `Name.md` and `Name.zh-CN.md`.
- The design drafts use the established pair `Docs/Design/EN/Name.md` and `Docs/Design/ZH/Name.md`.
- Any Markdown change must update both members of its pair in the same task and commit. This includes rule changes, status changes, links, and corrections.
- If a user requests a semantic change through a Chinese companion, first reflect it in the English source, then synchronize the Chinese text.
- A Markdown pair must keep the same document ID, version, status, and meaning where those fields exist.

If code, data, and a design document disagree, stop and report the conflict. Do not silently invent a rule.

## Boundary rules

- `PadmaCore` must not depend on UMG, Niagara, concrete Actors, maps, or `UGameplayStatics`.
- UI sends commands; it does not directly mutate world or gameplay state.
- World and gameplay state changes go through a service/command and produce an event.
- Use stable IDs for cards, nodes, encounters, quests, and save records. Do not use Actor pointers as save data.
- Use a seeded random stream for rule outcomes that need to be reproduced.
- Do not put new global state into a universal Manager.
- Do not edit `Content/ThirdParty/` in place. Create project wrappers under `Content/Padma/`.
- Do not modify generated folders: `Binaries/`, `DerivedDataCache/`, `Intermediate/`, `Saved/`, or `.vscode/`.

## Agent change protocol

1. State the intended scope and affected files.
2. Make the smallest coherent change.
3. Run the narrowest relevant validation script or test.
4. Update the relevant documentation and `Docs/ProjectState.md`.
5. Report changed files, checks run, remaining risks, and learning targets.

When a Markdown file changes, its paired English or Chinese document must be changed in the same operation and both paths must be reported.
Run `Scripts/AuditDocs.ps1` after Markdown changes to verify that no companion is missing.

Do not combine unrelated refactors with a feature task. Do not rename or move large groups of UE assets without an explicit migration task and redirector check.

## Definition of done

- Acceptance criteria are met.
- Relevant tests or a reproducible manual check pass.
- No new compiler, reflection, asset-loading, or packaging errors are known.
- Documentation and task status are updated.
- Every changed Markdown file has a synchronized language companion.
- Generated output and local-only files are not committed.
