# ADR-0002: Encounter-First Delivery and Future ACT Boundary

- Chinese companion for user reading: `Docs/Decisions/ADR-0002-Encounter-First-ACT-Preparation.zh-CN.md`
- Status: Accepted; GAS adoption trigger partially superseded by ADR-0004
- Date: 2026-09-05
- Implementation status: Deferred; documentation only
- Owner: Lead Programmer / Architect Agent
- Task contract: ../Production/Tasks/TASK-004-ACT-Architecture-Decision.md

## Context

Dream of Padma needs both the condition-driven turn-based Encounter route and a later ACT RealTimeAction route. The external Combat prototype demonstrates GAS, combo, montage, Motion Warping, and Blueprint techniques, but it also contains duplicated ability lifecycles, over-broad animation notify responsibilities, a damaged pre-input route, and plugin dependencies that are not part of Padma.

The project needs a durable future ACT boundary without diverting the current implementation from Encounter. It also needs to decide where project settings, per-character catalogs, per-ability definitions, runtime state, C++, Blueprints, GAS, and presentation belong.

## Decision

1. Encounter remains the implementation priority. This ADR and the ACT development contract do not authorize ACT code, assets, GAS dependencies, or configuration changes.
2. Future ACT implementation will be C++-first and data-driven. Common ability lifecycle and sequence interpretation belong to native Gameplay Ability and Ability Task types; data-only Ability Blueprints are optional content adapters.
3. Each playable ACT character may have a `CharacterACTCatalog` Primary Data Asset referencing character-specific Ability, Projectile, Summon, Scope, and Ammo DataTables. All instances use shared typed row structures.
4. An Ability Table row is a roster/assembly record that references a cohesive `AbilityDefinition` Primary Data Asset. Complex skill timing and combo topology belong to an `ACTSequence` Primary Data Asset. This resolves the ACT-specific high-level asset split; exact schemas and the broader project import pipeline remain open.
5. Stable logical IDs cross table and asset boundaries. Runtime resolvers use the active character catalog. Mutable state never lives in DataTables, Data Assets, or INI.
6. A future typed `UDeveloperSettings` and custom configuration section may hold project-wide defaults, policy, validation, loading, debugging, and feature gates. It may require an ability archetype to provide Enhanced Input, cooldown, montage, or other typed content, but it does not own a particular ability's binding, duration, montage, sequence, effects, or audiovisual assets.
7. As updated by ADR-0004 on 2026-09-08, native GAS is intended for both Encounter and ACT through TASK-013, with separate mode systems/tables. Runtime dependencies remain unimplemented until that task runs. External gameplay plugins still require a separate ADR.
8. Authoritative rules, hit validation, damage, costs, cooldowns, and cleanup live in C++ rule/GAS paths. Animation notifies emit narrow timing markers; Gameplay Cues and UI own cosmetics only. If networking is approved later, prediction is limited to reversible responsiveness while authority revalidates gameplay outcomes.
9. Combo topology uses explicit nodes and edges plus tag gates. A normal combo should default to one ability activation with runtime node and buffered-input state, rather than one new ability activation per segment.
10. Exact ACT gameplay semantics remain governed by accepted rule documents and their open/deferred markers. Architectural names in the ACT contract do not silently decide gameplay values or timing rules.

## Consequences

- Fixture-only TASK-007 through TASK-010 remain independent from GAS. Subsequent Encounter combat consumes the approved native GAS foundation without requiring an ACT gameplay framework.
- Future ACT Agents have a clear ownership, data, authority, Blueprint, and plugin boundary.
- Per-character content can expand without one universal DataTable or duplicated C++ classes.
- INI remains useful for policy while typed assets remain the source for authored skill content.
- The architecture creates more small typed assets and validation work, but makes ownership, loading, testing, and reuse explicit.
- Any future change to dependencies, module boundaries, networking, persistence, or external plugins still requires a scoped task and architecture review.

## Rejected alternatives

- One giant DataTable containing every ability, combo, hit, warp, projectile, summon, presentation, and runtime field.
- One custom INI database containing every ability's gameplay values and asset paths.
- A Blueprint-first port of the external Combat project.
- One Gameplay Ability activation for every ordinary combo segment by default.
- Animation Notify Blueprints or Gameplay Cues as authoritative combat logic.

## Deferred decisions

The first ACT vertical slice, GAS ownership details, networking baseline, tag taxonomy, exact row schemas, phase/cancel/buffer semantics, `Scope` meaning, payload lifecycles, and all design-baseline items already marked open remain unresolved. They require explicit future task or owner decisions.

## 2026-09-08 clarification

[ADR-0004](ADR-0004-Separate-Encounter-and-ACT-GAS.md) replaces the ACT-only GAS trigger and clarifies separate character-card collections, ACT battle-settings loadout and mode-specific effects. The remaining ACT asset architecture and Encounter-first order stand. Read it before TASK-013/017/030/035; historical TASK-004 scope is unchanged.
