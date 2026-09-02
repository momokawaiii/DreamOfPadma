# ADR-0001: Project Structure and Agent Governance

- Chinese companion for user reading: `Docs/Decisions/ADR-0001-ProjectStructure.zh-CN.md`

- Status: Accepted
- Date: 2026-09-01

## Context

The previous prototype combined rules, world state, UI, battle flow, and persistence in a small number of global Actors. The new project must remain teachable while supporting content growth and multiple Agent conversations.

## Decision

Use a documentation-first repository with Git/LFS, root and nested Agent instructions, explicit task files, validation scripts, stable IDs, versioned saves, and staged UE module boundaries. The initial runtime boundary is Core, Gameplay, World, UI, and Game composition; actual modules may be introduced as each boundary becomes stable.

## Consequences

- Agents can work from repository state instead of chat history.
- Rules and data can be tested without loading a map.
- New content should avoid modifying central flow code.
- Binary UE assets require ownership and migration discipline.
- Some abstractions are intentionally delayed until their variation is proven.
