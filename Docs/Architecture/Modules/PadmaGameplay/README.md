# PadmaGameplay Module Program Document

- Document ID: ARCH-MODULE-GAMEPLAY-001
- Version: 0.3
- Status: Planned logical boundary; no standalone UE module yet
- Canonical language: English for Agent consumption
- Chinese companion: README.zh-CN.md
- Owner: Gameplay Module Agent
- Current implementation: Source/DreamOfPadma/ is the temporary shared runtime module
- Parent architecture: ../../ProgramArchitecture.md and ../../DataDrivenArchitecture.md
- Future ACT development contract: ACTDevelopmentContract.md

## TASK-046 current native implementation

The native slice adds Gameplay/Combat transient ASC actors/attributes, mode-specific clocks, receipts and cleanup; Encounter/PadmaEncounterAbility and ACT/Runtime/PadmaACTAbility are separate GAS activation classes using different typed effect tables. Shared basic effects run on a separate card/environment ASC. Current combat harness supports the HTML actions; importing Combat's full combo system is not claimed. [Native playable guide](../../../Content/NativePlayableDemo.md).

## 1. Purpose

PadmaGameplay implements local battle families. Encounter is a turn-based battle with a condition-driven action timeline; RealTimeAction is the real-time route, with ACT as the first MVP mode and FPS/other modes as extensions. The module turns Core commands and data definitions into combat, ability, status, target, encounter, and ruler-policy behavior.

## 2. Responsibilities

- Combat command validation, Encounter action-timeline construction, initiative/priority scheduling, active-action orchestration, reactions, interrupts, extra actions, and non-turn windows.
- ACT RealTimeAction battle state, the ACT MVP `Tab` repository transition, background blur, 1/10 world-time scaling, movement/attack input rejection, inertia-preserving scene behavior, and five-visible-slot-per-page card dispatch. Total page count remains deferred.
- Attack damage, true damage, defense, status, death, and target-resolution stages.
- C character units, constructions, cores, and combat-facing runtime state.
- Separate Encounter and ACT ability/effect definitions, activation policies, clocks and native GAS execution; only approved infrastructure/calculations without implicit mode rules are shared.
- F/D/R attribute relationships and era modifiers.
- Deterministic ruler patrol, detection, reinforcement, counterattack, and threat policy.
- Combat logs, decision traces, and presentation-neutral results.
- Deployment of successful C results and application of explicitly configured non-reward failure-side consequences into legal encounter state.

## 3. Non-responsibilities

PadmaGameplay must not own:

- World graph layout, node coordinates, or persistent node ownership.
- UI widgets, input devices, animation graphs, or Niagara systems.
- The card catalog or synthesis formula semantics owned by Core.
- Save-file serialization of live Actors.

Gameplay may request World operations through commands/events and may consume a read-only world query.

## 4. Public contracts

- Encounter start, action timeline, active action, priority, reaction, interrupt, extra action, end-turn, result, complete, and retreat contracts. The exact timeline precedence remains intentionally unimplemented until confirmed.
- ACT RealTimeAction enter/exit, `Tab` repository/bullet-time input, background blur, movement/attack rejection, page/slot selection, and card request/result contracts.
- Damage and status calculation inputs/results.
- Ability use and target validation results.
- Ruler decision candidates, selected action, and decision events.
- Threat and war-balance event proposals routed through the authoritative state service.
- Combat log entries with stable entity IDs.

The runtime command path must work for player input, GM commands, automation, replay verification, and future network messages.

## 5. Data and runtime ownership

Gameplay consumes CardDefinition, AbilityDefinition, CombatProfile, EncounterDefinition, RulerPolicy, SigmaFailureTable, and relevant era profiles. It owns encounter runtime state, not the immutable card catalog.

All tunable damage, status, AI, and threat values belong in typed data. The code selects evaluators and enforces constraints.

## 6. Dependencies and integration

Gameplay depends on PadmaCore contracts. It communicates with PadmaWorld through legal command/query/event contracts and is composed by PadmaGame.

No gameplay rule may depend on a Widget event, level name, Actor pointer identity, or a hidden frame-timing side effect.

## 7. Tests and debug evidence

Required tests:

- Ordinary attack versus defense and true damage.
- At least one F/D/R counter and weak-match case.
- Status application, stacking, consumption, and death.
- One legal and one failed target selection.
- Basic non-A card legality outside battle, on the player's eligible Encounter action turn, the one-card-per-turn limit, and an ineligible action window.
- ABC active-ability activation from the sandbox without entering a battle.
- ACT RealTimeAction 1/10 world-time slowdown, `Tab` repository transition, background blur, movement/attack input rejection, card-only bullet-time input, inertia preservation, five-visible-slot paging, and complete battle-loop dispatch.
- Encounter terrain/card-condition changes to initiative, reactions, interrupts, and extra actions, after the exact action-timeline rules are confirmed.
- One patrol, detection, reinforcement, and counterattack.
- Threat thresholds and war-balance event deltas.
- Same seed reproduces combat and AI decisions.
- The complete pre-battle snapshot restores every battle-mutated run state after defeat or exit; success commits the result.

Debug output should show input entities, data IDs, damage stages, status layers, AI candidates, selected action, random stream position, and emitted events.

## 8. Implementation stages

1. Implement a pure damage/result service and fixtures.
2. Add Encounter turn state and one encounter with placeholder entities.
3. Add the ruler policy and one counterattack.
4. Add the minimum status subset used by the sample encounter.
5. Add the complete ACT RealTimeAction route only after the Encounter action-timeline path is stable and a bounded ACT task is approved, following `ACTDevelopmentContract.md` and the `Tab`/blur/1-10 input contract.
6. TASK-013 supplies the approved native GAS dependency/lifetime foundation for both modes under ADR-0004 before their playable GAS implementations; external plugins still require a separate ADR.

## 9. Learning targets and risks

Learning targets: Gameplay Framework boundaries, combat services, status effects, targeting, GAS preparation, AI utility policies, animation timing contracts, and deterministic debugging.

Main risk: embedding combat rules in Character, Ability Blueprint, or animation code so they cannot be replayed or tested independently.

## 10. Full-MVP mode ownership

[ADR-0004](../../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md) assigns TASK-015 to approved shared calculations, TASK-016/017 to Encounter timelines/catalog/abilities, TASK-030 to ACT catalog/sequence/abilities, and TASK-031 to its repository window. Mode definitions and runtime state stay distinct. TASK-035 owns ACT character availability/loadout; the turn-based card system is not its collection.

Required added tests: wrong-mode binding rejection; table/clock/input isolation; restricted traits rejected by direct commands; no stale effects/tasks/callbacks after exit; complete participant recovery including approved run-owned roster/weapon state. Basic-card services are injected by composition; neither mode duplicates the shared card lifecycle or assumes equal effect data.


## Retired TASK-008 and current TASK-036 authoring

Historical TASK-008 supplied fixed Demo definitions, query/summon code and an F8 scene fixture. TASK-048 retired that source, its maps/assets and tests. It is not the current battle entry or a model-preview dependency; historical evidence remains in TASK-008.

TASK-036 owns Gameplay/ACT/Authoring and ACTCharacterCards static content. Separate ACT character/weapon/skill assets, typed skill rows and an authoring catalog expose soft references and actionable validation; the catalog holds no roster/run state or GAS handles. See [ACT authoring guide](../../../Content/ACTAuthoring.md). Current native execution is Gameplay/Encounter and Gameplay/ACT/Runtime, with separate GAS classes/tables; TASK-013/030 remain the original ownership references. Required checks cover missing/wrong references, duplicate identities/bindings, skeleton mismatch and explicit trait selectors. Empty templates are intentionally invalid drafts; later cooking/loading and user content remain necessary.
