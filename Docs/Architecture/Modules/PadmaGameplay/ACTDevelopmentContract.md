# ACT Development Contract

- Document ID: ARCH-GAMEPLAY-ACT-001
- Version: 0.3
- Status: Native prototype exists; one-character complete ACT target accepted, implementation incomplete
- Chinese companion: [ACTDevelopmentContract.zh-CN.md](ACTDevelopmentContract.zh-CN.md)
- Owner: Gameplay Module Agent
- Source ownership: Source/DreamOfPadma/{Public,Private}/Gameplay/ACT; integration/write paths are scoped per TASK
- Authority: [ADR-0004](../../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), [ADR-0010](../../../Decisions/ADR-0010-Offline-Demo-Content-and-Map.md), [ChapterZero](../../../Rules/ChapterZero.md)

## Current implementation and reference boundary

GAS is already a Padma dependency. ACT/Runtime/PadmaACTAbility commits, delegates ResolvePendingAction and ends; ACT/Authoring supplies character/weapon/skill definitions and typed skill rows. The existing playable combat adapter is a rule-validation foundation, not complete third-person 3C, Sequence/Combo/Projectile/Summon/Execution gameplay.

Combat is a read-only behavior/asset reference, currently at E:/2026ue/Combat. The older conversation used E:/Combat. [Combat Fitting](codex://threads/01a0679e-0779-7d33-aae7-c74a149007f7) records the architecture direction; source paths and current behavior must be verified before migration. No GASCompanion, AuroraDevs_UGC or external project dependency is approved for Padma.

The old “no GAS/no ACT assets” and “ACT entirely deferred until Encounter is built” descriptions are obsolete. Reuse current Encounter and ACT foundations. The first ACT learning task is one new model with retargeted animation and keyboard/mouse 3C; it is smaller than the final P0 acceptance below.

## Final P0 versus first task

Q82/Q89/Q90 require one controllable new Padma character in one fixed, repeatable ACT scene entered from the strategy map. Actual operations cover third-person movement/camera, normal attack, combos/skills, targeting, interruption/being hit, projectile, summon and execution, then victory/defeat/exit/return with correct commit/rollback.

One representative of each required payload/action family must work; Combat's other characters/maps/content are P1/P2. User hand-feel convergence supplements this objective list. Keyboard/mouse is P0; specific reversible keys can use defaults. Gamepad polish, complex lock-on and multiplayer are later scope. Fixed enemy configuration/waves must be authored, not inferred from Combat.

[ACTMigrationMatrix](../../../Content/ACTMigrationMatrix.md) tracks required priority separately from verified promotion. Q81's first task contains model/retarget/3C only; it does not waive the final combat list. [DemoDeliveryPlan](../../../Production/DemoDeliveryPlan.md) maps subsequent learning units.

## Architecture and runtime authority

`Data-only Ability BP → native Ability lifecycle → immutable AbilityDefinition/Sequence → per-execution Runtime State → focused AbilityTasks/events → validated TargetData/GE → Cue/presentation`

| Layer | Owns | Must not own |
|---|---|---|
| Native Ability base | activation checks, grant/commit/cancel/end invariants, cleanup and typed extension points | Duplicated per-child lifecycle or UI-owned legality |
| AbilityDefinition | stable ID/version, class, activation/tags, cost/cooldown references, sequence, hit/target/warp/payload/Cue bindings | Current targets, timers, input buffer or live handles |
| ACTSequence | explicit nodes/edges/phases, input/required/blocked tags, branch priorities, window/animation/event actions | Mutable node/phase, hit lists or another independent copy of magnitudes |
| Sequence Runtime | current node/phase, selected edge/reason, windows, buffered input, targets and owned handles | Shared asset mutation or a giant universal AbilityTask |
| Focused AbilityTasks/components | montage/event waits, buffer/window handling, hit detection and payload lifetime | Mixed animation/AI/camera/authoritative damage inside one NotifyState |
| GE / rule service | approved attribute, cost/cooldown, damage/status settlement | A second direct-HP implementation hidden in presentation |
| GameplayCue / camera / Niagara | result-driven visual/audio feedback | Hit authority, rewards, costs or branch settlement |

Tags express categories/gates; explicit node state and edges remain necessary. Start with one execution path, then split tasks by responsibility. Use native ASC, GameplayAbility, GameplayEffect, AttributeSet, GameplayCue and existing native tasks rather than reimplementing GAS infrastructure.

## Data and authoring

Reuse existing ACT/Authoring types; do not create duplicate character/weapon/skill identity catalogs. Extend or adapt UPadmaACTSkillDefinition and FPadmaACTSkillRow with reviewed versioning when adding full definitions/sequences.

- Character-specific ACT catalogs/tables can share ACT row types; Encounter keeps separate schemas.
- Rows assemble ability/payload IDs and bindings. AbilityDefinition/Sequence carry complex references/topology. A scalar has one declared source, including any GE magnitude or explicit character override.
- Projectile/summon/ammo and optional area/target profiles use stable IDs. The older ambiguous “Scope” is not a frozen field or mandatory schema.
- Data-only Ability BP selects native implementation/configuration, not an Event Graph copy of execution logic.
- Infrastructure defaults may use typed DeveloperSettings/INI only where needed. Per-skill montage, damage, cost, cooldown duration, branches and payloads stay in content definitions, never duplicated config.
- Soft references need explicit load/cancel/cook behavior. Authoring validity is separate from live ability readiness and release-asset clearance.

The final phase vocabulary, commit boundary, cancellation matrix, buffer lifetime/replacement/priority, payload limits and numerical values require the scoped rule/content decision. Startup/Active/Recovery/ComboWindow/PreInput/End are explanatory vocabulary, not new universal timing constants.

## Hit, payload and cleanup contract

Input requests activation. Montage/Notify announces timing. Native hit/window code creates validated TargetData; GE/rules settle. Trace deduplication and target eligibility must be explicit. A visual impact or montage ending cannot prove damage or success.

Track temporary tags, tasks/delegates, effects, input/window state, projectiles, summons, target reservations and camera overrides. Cancel, interruption, owner death, target death, failed start, travel and battle exit each need a defined cleanup owner. Execution alignment must have an invalid-target/warp failure route. Do not copy unverified Combat pre-input or notify behavior as correct.

Exact per-payload persistence/damage/capacity and execution thresholds remain authored rules. No extraction or prototype sample approves final Padma values.

## Mode, save and future network seams

ACT and Encounter own separate character collections, ability schemas, clocks and runtime GAS state. ACT uses the validated character/weapon roster; map units supply site/context. Shared basic-skill IDs/slots resolve separately authored mode effects. Trait-only restrictions cannot become whole-character exclusions.

Keep current approved card/repository semantics in [Combat rules](../../../Rules/Combat.md); UI/key redesign does not silently change costs, permissions or clock behavior. New behavior expands commands/contracts rather than directly editing RunState.

Current P0 is offline local authority. Keep stable identities, serializable state, authority-confirmed results and UI-independent Ability/Effect boundaries. Replication, prediction, RPC, sessions and servers are not current implementation or acceptance. These seams reduce coupling but do not promise automatic future multiplayer.

Game owns full battle transaction/travel; every battle-mutated run field belongs in the approved snapshot. Save no ASC, ability/effect handle, camera/Sequence object or live spawned Actor pointer. Reconstruct transient objects; follow [SaveSchema](../../SaveSchema.md).

## Verification and execution policy

Relevant implementation evidence includes data validation, deterministic node/window/edge traces, normal/early/late input, cancel/death/interrupt cleanup, hit authority/deduplication, payload exit cleanup, retarget/montage observation and actual map-to-ACT-to-map results. New native code needs the Editor build; public/lifecycle/save changes need focused independent review.

[BuildMatrix](../../../Production/BuildMatrix.md) owns four-layer release checks, asset audit and generous measured performance baselines. Functional milestones may temporarily lack audio; RC must have production audio without requiring voices.

Use existing permissions and approved scope. Ask only when a missing decision changes gameplay meaning, scope, acceptance, architecture or a real risk. Inspect environment facts directly and use reasonable defaults for reversible details. Do not reopen answered options because an old TASK says Backlog. Source copying, plugin introduction, engine edits, merging/pushing and overlapping binary edits remain outside this contract's authorization.
