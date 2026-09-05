# PadmaGameplay Module Program Document

- Document ID: ARCH-MODULE-GAMEPLAY-001
- Version: 0.1
- Status: Planned logical boundary; no standalone UE module yet
- Canonical language: English for Agent consumption
- Chinese companion: README.zh-CN.md
- Owner: Gameplay Module Agent
- Current implementation: Source/DreamOfPadma/ is the temporary shared runtime module
- Parent architecture: ../../ProgramArchitecture.md and ../../DataDrivenArchitecture.md
- Future ACT development contract: ACTDevelopmentContract.md

## 1. Purpose

PadmaGameplay implements local battle families. Encounter is a turn-based battle with a condition-driven action timeline; RealTimeAction is the real-time route, with ACT as the first MVP mode and FPS/other modes as extensions. The module turns Core commands and data definitions into combat, ability, status, target, encounter, and ruler-policy behavior.

## 2. Responsibilities

- Combat command validation, Encounter action-timeline construction, initiative/priority scheduling, active-action orchestration, reactions, interrupts, extra actions, and non-turn windows.
- ACT RealTimeAction battle state, the ACT MVP `Tab` repository transition, background blur, 1/10 world-time scaling, movement/attack input rejection, inertia-preserving scene behavior, and five-visible-slot-per-page card dispatch. Total page count remains deferred.
- Attack damage, true damage, defense, status, death, and target-resolution stages.
- C character units, constructions, cores, and combat-facing runtime state.
- Ability definitions, activation contexts, costs, timing, and future GAS integration seams.
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
6. Add built-in GAS only through an approved dependency change when the ACT vertical slice requires it; external GAS plugins require a separate ADR.

## 9. Learning targets and risks

Learning targets: Gameplay Framework boundaries, combat services, status effects, targeting, GAS preparation, AI utility policies, animation timing contracts, and deterministic debugging.

Main risk: embedding combat rules in Character, Ability Blueprint, or animation code so they cannot be replayed or tested independently.
