# ADR-0004: Separate Encounter and ACT GAS Systems

- Chinese companion: ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md
- Document ID: ADR-0004
- Version: 0.1
- Status: Accepted direction; concrete implementation contracts remain gated
- Date: 2026-09-08
- Owner: Lead Programmer / Architect, with user-owned gameplay decisions
- Task: [TASK-011](../Production/Tasks/TASK-011-Full-MVP-Task-Cluster.md)
- Decisions: [MVP register](../Production/MVPDecisionRegister.md)

## Context and user decisions

The user requested the complete seven-day MVP task cluster and clarified that Encounter and ACT both use GAS but are different mode systems with different tables. Turn-based character cards are deployed in the sandbox. ACT has a separate character-card collection and a battle-settings roster containing characters, weapons and an approved equipment subset. Terrain can restrict a trait of an ACT character card.

Basic non-A battle skill cards remain a separate concept: their identity and visible repository slot mapping are shared, while each mode configures its own effects. This does not identify an ACT character card with the turn-based card that triggered a battle.

Placeholder assets will be supplied by the user after implementation starts. Final animation/rendering polish is later work. Unaccepted numbers, including test-profile values, require individual user confirmation.

## Decision

1. Use Unreal's built-in GAS in both Encounter and ACT. TASK-013 introduces the dependency and mode-scoped infrastructure; TASK-017 owns the playable Encounter implementation; TASK-030 owns ACT. Delivery remains Encounter-first.
2. Keep separate configuration domains, typed mode catalogs/rows, ability/effect definitions, activation policies, runtime state, scheduling, clocks, targeting and validators. Mode-specific AttributeSets, lifecycle bases and sequences may differ. A shared GAS utility must not impose a universal mode schema.
3. Encounter action/window rules grant action eligibility before its own GAS execution. ACT uses its own input, real-time ability tasks, sequence/hit and cancellation path. ACTSequence and shared per-character ACT row structures apply within ACT only.
4. Share approved stable identities, command/result/event vocabulary, participant/snapshot infrastructure and pure calculations only where rule meaning is explicitly shared. Sharing does not require equal numeric values or effects; each mode's authored field has one declared source.
5. Distinguish deployed turn-based character-card identity from ACT character-card/loadout identity. TASK-035 owns ACT battle settings and roster state; TASK-027 carries roster and terrain/context to mode entry. Do not assume an automatic one-to-one character mapping or invent a restriction based on that assumption.
6. A basic battle card/slot resolves an explicit Encounter ability binding or ACT ability binding. Wrong-mode or missing bindings fail visibly. There is no silent fallback to the other mode's table or live ability instance.
7. Context restrictions name their target (character, weapon, ability or trait) and rule. A terrain restriction on a trait does not automatically ban the entire character. Exact scopes, validation/update timing, roster capacity and result projection remain D20 decisions.
8. Game composition owns mode/session lifecycle and the outer battle transaction. Each mode owns its own transient GAS objects; Core and saves use stable IDs/versions, not ASC pointers, AbilitySpec/effect handles, held keys or animation progress.
9. Success commits once; local defeat/exit restores the complete pre-battle state, including battle-mutated run-owned ACT roster/loadout/inventory state and random positions. Keep Encounter logical clocks, ACT time domains and paused sandbox calendar separate. Same seed alone does not promise arbitrary ACT physics replay.
10. Stay in the existing DreamOfPadma UE module until a later reviewed split is justified. External gameplay plugins and engine source changes remain separate decisions. UI, animation and Gameplay Cues cannot own authoritative damage, payment or status changes.

## Explicit partial supersession

ADR-0002's ACT-only justification for enabling GAS is replaced by this dual-mode decision. Its Encounter-first order, native/data-driven ACT direction, ACT-specific catalog/AbilityDefinition/ACTSequence split, cosmetic-only cues and external-plugin controls remain applicable.

TASK-004 and TASK-006 through TASK-010 remain historical or bounded fixture contracts; their no-GAS implementation scope is not widened. Statements requiring the same combat/card-effect resolver now mean shared contracts/calculations only where semantics are expressly shared, with distinct mode execution and data.

## Verification required of descendants

- Wrong-mode definitions are rejected; modifying one mode's definition does not change the other mode's fixture output.
- Shared basic-card identity/slot mapping resolves distinct mode bindings; character-card collections and ACT loadout are distinguishable.
- Terrain-restricted traits are validated independently from whole-character eligibility.
- Action eligibility, input gates and clock advancement cannot leak between modes.
- Interruption/cancellation ends only its execution; individual death cleans only that entity's grants/effects/callbacks while other eligible units continue. Only an approved battle-ending result or session exit clears the whole battle and restores global time/input. Loading/travel follows that explicit session boundary.
- Commit-once and exact rollback cover every mutating participant, including roster/weapon state when applicable.
- Trace mode, run/battle ID, definition versions, command order, logical clock and named random stream positions.

Global battle-settings presets participate in rollback only if D20 explicitly classifies those fields as run-owned battle-mutated state; a global preset is not silently rolled back.

## Deferred contracts

D04/D05/D06/D09/D14/D15/D16/D17/D20 retain exact schemas, ASC ownership, mode rules, loadout policies, cross-level projection and save compatibility decisions. No new gameplay values are accepted by naming a type or table in this ADR. D03 controls all unaccepted numerical inputs.

## Consequences

Mode content and behavior can evolve independently while using native GAS. This requires explicit mode binding/validation and two sets of mode content tests. The shared run/battle boundary carries durable identity and results without becoming a universal gameplay manager. This ADR authorizes the architecture direction, not an unreviewed runtime implementation.

## Early ACT authoring slice (2026-09-08)

The user's D23 answer authorizes TASK-036 static character/weapon/skill definitions and model/animation/ACT-table soft references before content is designed. Its ACT/Authoring types own static identity and validation; TASK-030 consumes them and owns executable GAS definitions/registry, while TASK-035 owns collection/roster state and projections. No GAS dependency, roster policy or numerical default is introduced by TASK-036. Table row types, ID bindings, missing references and skeleton identity are validated; trait/terrain selectors use a supplied read-only catalog and do not execute eligibility. Empty templates remain invalid drafts.
