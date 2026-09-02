# Data-Driven Architecture

- Document ID: ARCH-DATA-001
- Version: 0.2
- Status: MVP data architecture baseline frozen; concrete schemas evolve under review
- Baseline accepted: 2026-09-02
- Canonical language: English for Agent consumption
- Chinese companion: DataDrivenArchitecture.zh-CN.md
- Owner: Lead Programmer / Architect Agent
- Related design: ../Design/EN/02_SystemPlanner.md and ../Design/EN/05_NumericalPlanner.md
- Related rules: ../Rules/Time.md, ../Rules/Synthesis.md, ../Rules/WorldState.md, ../Rules/Combat.md

## 1. Purpose

This document defines what should be data-driven, which kind of Unreal asset should carry it, how it enters runtime, and which module owns the result. It is a technical mapping document. The design documents remain responsible for the meaning, constraints, and acceptance criteria of the game rules.

The project should use many small, typed data domains rather than one universal table. A single catalog document can describe all domains, but the Unreal assets should remain separated by ownership and lifecycle.

## 2. Data layers

### 2.1 Design semantics

The design and rule documents define:

- Meaning of a field.
- Valid values and constraints.
- Timing and phase.
- Inputs and outputs.
- Player-facing consequence.
- Examples and acceptance cases.

They should not require a particular C++ class or asset path to explain the rule.

### 2.2 Authoring definitions

Authoring definitions are immutable content data. They use stable IDs and may reference other definitions by ID or a typed asset reference. Examples are card definitions, era profiles, node definitions, encounter definitions, and story events.

### 2.3 Runtime state

Runtime state is the current run, not a copy of an asset. It includes current resources, ownership, card locations, cooldowns, active mutations, encounter health, flags, and random stream positions. Runtime state is changed through commands and rules.

### 2.4 Events and persistence

An event explains a state change and can be consumed by UI, AI, story, audio, telemetry, replay, and save history. A save stores versioned state and stable IDs, not live object pointers.

## 3. Data-driven domains from the MVP

| Domain | Data that must be configurable | Primary consumer | Suggested asset |
|---|---|---|---|
| Calendar | Phase order, day/era/chapter structure, era theme and pool modifiers | PadmaCore | EraDefinition Primary Data Asset plus small modifier table |
| Card catalog | Card ID, family, attribute, tags, vectors, affinity, costs, stats, abilities, activation contexts, upright/reversed text | PadmaCore and PadmaGameplay | CardDefinition Primary Data Asset; flat balance table where useful |
| Card pools | Draw pools, era inclusion, weights, hand/deck rules, basic non-A battle-hand rules, card lifecycle outcome profiles, five-slot page rules, Tab repository, and mouse-wheel paging; total page count is deferred | PadmaCore | DataTable or pool Primary Data Asset |
| Synthesis | Candidate filters, vector score settings, Softmax temperature, success formula profile | PadmaCore | SynthesisProfile Primary Data Asset plus CurveTable |
| Failure side | Sigma dice specification, thresholds, non-reward failure consequences, and entropy effects; no failure reward | PadmaCore and PadmaGameplay | SigmaFailureTable DataTable |
| Resources | Flow, computation, entropy, faith, caps, debt, conversion costs, daily gains, and explicitly authored paid/risky loops | PadmaCore | ResourceBalance DataTable or BalanceProfile |
| Combat | Damage types, defense, true damage, F/D/R relations, statuses, range, costs, Encounter action timeline, and RealTimeAction timing/input hooks | PadmaGameplay | CombatProfile and AbilityDefinition assets plus curves |
| World graph | Stable nodes, edges, node type, owner, terrain affix, operation rules, conditional paths | PadmaWorld | WorldGraph Primary Data Asset plus node/edge tables |
| World mutation | Discoveries, captures, roads, facilities, weather, environmental flags | PadmaWorld | Mutation definitions and event effect data |
| Ruler AI | Patrol route, sight rule, reinforcement, counterattack, threat thresholds, utility weights | PadmaGameplay | RulerPolicy Primary Data Asset |
| Encounters | Encounter ID, legal entry, units, terrain, waves, boss phases, rewards | PadmaGameplay and PadmaWorld | EncounterDefinition Primary Data Asset |
| Outcomes | Victory/defeat condition type, source, comparator, threshold, timing, priority, result ID | PadmaGame and PadmaCore | OutcomeConditions DataTable |
| Story | Event ID, choices, prerequisites, state effects, next events, text IDs | PadmaWorld and PadmaGame | StoryEvent Primary Data Asset |
| Battle modes and input | Strategic sandbox, condition-driven Encounter turn-based route, ACT RealTimeAction MVP route, future FPS/other presentation, camera, possession, input adapter, ACT MVP `Tab` repository transition, background blur, 1/10 world-time scale, movement/attack input rejection, bullet-time, five-slot paging, and engagement-route/story effects | PadmaGame, PadmaGameplay, and PadmaUI | ControlModeRule DataTable or Primary Data Asset |
| Presentation references | Card art, icons, animation sets, VFX, SFX, BGM, camera and localization IDs | PadmaUI and presentation systems | Presentation Profile assets; never the rule source |

This table is a catalog of responsibility, not a commitment to create every asset before the first playable slice.

## 4. Asset selection rules

### Use a Primary Data Asset when

- The object has a stable identity and is referenced by many systems.
- It contains nested references or a coherent content package.
- It should be loaded and discovered through an Asset Manager.
- It represents a card, era, encounter, story event, ruler policy, or world profile.

### Use a DataTable when

- Rows have a common flat schema.
- Designers need spreadsheet-style comparison.
- The data is a collection of thresholds, costs, modifiers, rewards, or outcome definitions.
- A stable row name can serve as a content ID.

### Use a CurveTable or curve when

- The rule is a tunable continuous relationship.
- Balance needs interpolation or a visible curve.
- The curve has a named input domain and output unit.

### Use Gameplay Tags when

- The value is a category, label, event topic, faction, status, or permission.
- Multiple modules need to query the same taxonomy.
- The tag should remain stable across localization and asset renames.

Do not use a string containing C++ or Blueprint expressions as a substitute for a typed rule. A data row selects a known evaluator; it does not execute arbitrary code.

## 5. Stable identity and references

Every definition needs a stable ID separate from:

- Display name.
- Localized text.
- Asset path.
- Actor class.
- Map coordinate.
- Visual mesh or icon.

Recommended ID families:

| ID family | Example shape |
|---|---|
| Card | Card.A.Root.Mark or Card.C.Creator.Yog |
| Node | Node.Player.Home or Node.Ruler.Core |
| Encounter | Encounter.Frontier.Counterattack |
| Story event | Story.Anecdote.FirstChoice |
| Outcome | Outcome.Win.RulerCoreDestroyed |
| Data profile | Profile.Synthesis.MVP |

The exact naming vocabulary belongs to Content/Naming.md. IDs must not be recycled after release of a save or replay.

## 6. Runtime conversion

The preferred data path is:

    Asset files
       -> editor validation
       -> asset registry / Asset Manager
       -> loaded immutable definitions
       -> typed runtime view
       -> rule services

Runtime code should not repeatedly parse raw text or reach into arbitrary assets during every command. Load and validate definitions at an explicit boundary, then pass typed views to calculations.

Definitions must not contain mutable per-run values. For example:

- CardDefinition contains a card's base tags, costs, and ability references.
- CardRuntimeState contains location, orientation, cooldown, damage, and current attachments.
- WorldNodeDefinition contains node type and initial rules.
- WorldNodeRuntimeState contains owner, discovered state, mutations, and garrison.
- OutcomeConditionDefinition contains a condition.
- RunOutcomeState contains whether and when that condition resolved.

## 7. Core data contracts

### CardDefinition

Required conceptual fields:

- Stable CardId.
- CardFamily and Attribute.
- Species/identity tags and synthesis vector tags.
- Affinity.
- Costs, base life/stability, and tendencies.
- Ability definition IDs with activation context, timing, cost, target, and cooldown.
- Upright/reversed text IDs.
- Era pool tags.
- Recommended inputs and failure-side effects.
- Basic non-A battle-slot/page and input metadata when applicable: 1-5 visible slots per page, Tab repository, and mouse-wheel paging. Total page count and discard/return/cooldown probabilities are intentionally not defined here.

### AbilityDefinition and activation policy

An ability definition must identify how the ability can be activated independently from the card's placement or synthesis use:

- Stable AbilityId and owning CardId.
- Allowed activation contexts, including Sandbox, EncounterPlayerTurn, or RealTimeAction. ABC active abilities allow Sandbox by default; exceptional restrictions are explicit per ability.
- Input method, including direct activation or RealTimeAction numeric slot/page.
- Resource cost, cooldown, target rule, and effect sequence.
- Whether outside-battle activation is allowed, defaulting to true for ABC active abilities unless the ability declares a restriction.
- Any mode-specific restriction or required input profile.

Basic non-A card play is represented as a battle-only card operation and must not be modeled as a generic sandbox ability. In Encounter, the data must support the player's eligible action-turn gate and one-card-per-turn limit. In ACT RealTimeAction, it must support bullet-time-only input and five-slot pages. ABC-card active abilities include the Sandbox context by default; any exceptional restriction and any additional battle context must be explicit per ability.

### SynthesisProfile

Required conceptual fields:

- Candidate tag filter policy.
- Vector scoring policy.
- Softmax temperature.
- Resonance distance formula profile.
- Affinity modifier profile.
- Entropy modifier profile.
- Faith modifier profile.
- Probability clamp.
- Random stream names.
- Sigma failure/consequence table ID, if Sigma dice remain in the synthesis path.

### OutcomeConditionDefinition

Minimum MVP fields:

- OutcomeId.
- Outcome type: Victory or Defeat.
- Condition type: a typed evaluator such as CoreLifeThreshold or WarBalanceThreshold.
- Source scope and source ID.
- Comparator and threshold.
- Evaluation timing.
- Priority.
- Presentation/result ID.
- Optional reward, story branch, and save marker IDs.
- Enabled flag or profile membership.

The two proposed victories are data rows. The resolver is code that evaluates the known condition types.

## 8. Data validation

Editor or command-line validation must check:

- IDs are unique and use the approved namespace.
- References resolve.
- Required tags and families are valid.
- Basic non-A cards are legal in local battle contexts and are not legal in the Sandbox context.
- Encounter basic-card entries require the player's eligible action-turn gate and one-card-per-turn limit, while allowing configured reaction/interrupt/extra-action rules and their eventual card allowance to remain separate and unresolved.
- ACT RealTimeAction card entries have a valid bullet-time-only input profile, a 1-5 page slot mapping, Tab repository mapping, and mouse-wheel paging. The ACT MVP profile must specify background blur, 1/10 world-time scale, and rejection of movement/attack input; total page count and page-boundary behavior remain uncommitted.
- ABC active abilities expose the default outside-battle Sandbox context and any exceptional restriction explicitly, with no contradictory mode restriction.
- Resource conversion loops have an authored cost, timing, and risk; no free cycle is admitted by validation.
- Synthesis failure data cannot grant a reward; any retained Sigma result must be a non-reward failure consequence.
- Costs, caps, thresholds, and probabilities are within allowed ranges.
- Curve input/output units are declared.
- A synthesis candidate is compatible with its candidate filter.
- Outcome conditions have a valid source, comparator, and priority.
- Era and Chapter references form a valid hierarchy.
- World graph edges reference existing nodes.
- Story choices reference existing flags or typed effects.
- No content definition depends on a generated folder or an absolute local path.

Validation errors should name the asset ID, field, and reason. Warning-level issues are recorded separately from blocking errors.

## 9. Deterministic calculation and random streams

Every random calculation must declare:

- Master seed or run seed.
- Named stream.
- Event or command position.
- Input definition IDs and versions.
- Roll value in debug/test builds.

Candidate selection and success/failure use separate random steps. Preview must use the same pure calculation service as execution but must not advance the live stream or mutate state.

The calculation trace must be sufficient to reproduce:

- Candidate scores and Softmax probabilities.
- Selected candidate.
- Success probability.
- Success/failure roll.
- Sigma result or non-reward failure consequence, if that path is enabled.
- State and event outputs.

## 10. Data and save/network boundaries

Assets are versioned content definitions. Saves are versioned runtime state. Neither should serialize a live Actor pointer as an identity.

When a definition changes:

- Preserve the stable ID.
- Record a definition or content version where the outcome depends on it.
- Add migration when a save shape changes.
- Add a fixture if old data must remain loadable.

For future network play:

- The server or authority loads the same definitions.
- Clients may receive read-only definitions and presentation references.
- Commands carry IDs and parameters, not UObject pointers.
- Results and events are the synchronization vocabulary.

## 11. MVP asset plan

The first data-driven slice should use the smallest useful set:

1. One MVP balance profile.
2. Two A definitions, two B definitions, two C definitions, and one failure-side definition.
3. A minimal basic non-A battle-hand set with five-visible-slot page metadata, including direct damage and protection/recovery examples; total page count and lifecycle probabilities remain deferred.
4. One synthesis profile and, only if retained, one Sigma failure/consequence table with no reward outcomes.
5. One resource profile.
6. One hand-authored world graph with stable node IDs.
7. One Encounter definition and one ruler policy.
8. One battle-mode profile describing the condition-driven Encounter route and the complete ACT RealTimeAction MVP route, including the `Tab` repository transition, background blur, 1/10 world-time scale, movement/attack input rejection, bullet-time, and five-slot paging.
9. Two victory rows and at least one explicit defeat row.
10. One anecdote event with two choices.
11. One presentation profile for placeholders.

The exact values remain numerical playtest data. The asset contract should be stable enough that changing a value does not require changing the rule service.

## 12. Ownership and documentation

The design owner defines field meaning. The Architect Agent defines the data contract and loading boundary. The Module Agent consumes the contract and owns validation/tests for its module. The Review Agent checks that no rule or balance value is duplicated in code.

When a field changes:

1. Update the English design/rule source if its meaning changes.
2. Update the English data contract if its shape or constraint changes.
3. Synchronize every Chinese companion in the same task.
4. Update the affected module README and fixtures.
5. Add an ADR if identity, dependency, save, network, or import behavior changes.

## 13. Open data decisions

- Final choice between CSV, JSON, DataTable, Primary Data Asset, or a mixed import pipeline.
- Exact asset granularity for cards and abilities.
- Whether formulas use curves, typed profiles, or both.
- The final E and L tag taxonomy.
- Final probability clamp and unit conventions.
- Whether presentation references live inside card definitions or in separate profiles.
- The schema for exceptional ABC active-ability restrictions; Sandbox permission is the default.
- Whether ABC active abilities are also legal during Encounter or RealTimeAction.
- Exact Encounter action-bar precedence, initiative modifiers, reaction/interrupt priority, extra-action rules, non-turn windows, end-turn effects, and the precise card allowance of those windows; this is intentionally blank until the implementation needs it.
- ACT MVP `Tab` trigger, background blur, movement/attack input rejection, 1/10 world-time scale, five visible slots per page, and Tab repository mapping are fixed. Duration/toggle behavior, cooldown/resource cost, page-boundary behavior, and total page count remain open.
- Basic non-A discard/return/cooldown probabilities remain blank until the relevant card implementation needs them.
- How both sides jointly constrain the engagement route, including terrain/card restrictions and story consequences.
- Whether Sigma dice remain and, if so, their non-reward failure consequence.
- Definition version policy for replays and saves.
