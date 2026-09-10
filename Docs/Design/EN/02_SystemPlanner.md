# MVP Design Baseline 02 - System Planner

- Document ID: `DESIGN-MVP-SYSTEM-001`
- Status: High-level MVP baseline accepted; numerical coefficients remain data/playtest decisions
- Baseline accepted: 2026-09-02
- Canonical language: English for Agent consumption
- Chinese companion: `Docs/Design/ZH/02_SystemPlanner.md`
- Owner: System Planner
- Related contracts: `Docs/Rules/Time.md`, `Docs/Rules/Synthesis.md`, `Docs/Rules/WorldState.md`, `Docs/Architecture/SaveSchema.md`

## 1. System responsibility

This document turns the high-level product baseline into a system model that can be tested without depending on a map or a widget. It does not freeze numerical coefficients or prescribe Unreal classes.

The system layer owns identity, state transitions, resource accounting, card data contracts, synthesis resolution, and persistent world events. Presentation systems may display or animate these results but may not redefine them.

## 2. Card taxonomy

### 2.1 Card families

| Family | Conceptual role | Attribute source | MVP responsibility |
|---|---|---|---|
| A | Root, base, ego; the cause or foundation | F; user-confirmed D22d mapping | May occupy a node, provide a basic board effect, carry tendencies, and receive a B attachment. |
| B | Gospel, medium, catalyst; the transformation context | D; user-confirmed D22d mapping | Provides cost, field values, decoration, arrival, and mutation properties. |
| C | Terminal, creation, self; the generated result | R; user-confirmed D22d mapping | Becomes a construction, character, event, or item and carries target values, stats, and abilities. |
| Basic non-A | Basic battle skill hand card | Explicit per card | Can be played only inside a local battle: at most one during each eligible player action turn in Encounter, or through the ACT bullet-time/numeric-slot route in RealTimeAction. |
| Extra / derivative | Explicit helpers or non-reward failure-side content | No vector contribution by default | Exists outside the normal candidate-vector pool and is resolved by explicit effects; it is not an automatic synthesis reward. |

The source rule is `A_x + B_x -> C_x`. The collection's old neutral/cross-family attribute labels require revision to the user-confirmed A→F, B→D, C→R mapping; they do not create implicit exceptions. A B card may be played without an A card; the exact consequences are specified below.

### 2.2 Card data fields

An MVP card definition should be data-driven and should distinguish design identity from presentation assets.

Common fields:

- `CardId`
- `CardFamily`
- `Attribute`
- `CardType`
- `SpeciesOrIdentityTags`
- `VectorTags`
- `Affinity`
- `FlowCost`
- `StabilityOrLife`
- `FlowTendency`
- `EntropyTendency`
- `TargetFlow` and `TargetEntropy` for C cards
- Abilities with explicit activation context, timing, cost, target, and cooldown
- `UprightText` and `ReversedText`
- `EraPoolTags`
- `RecommendedInputs`
- `ActivationContexts`
- `ActivationInput`, `BattleSlotPage`, and an optional battle slot
- `FailureSideEffects`

Stable IDs must survive asset renames, localization changes, and display-name changes.

### 2.3 Card activation contexts

The distinction between basic non-A cards and ABC cards is an activation-channel rule, not a replacement for the A/B/C family taxonomy:

- A basic non-A hand card is a battle skill card. Its play operation is legal only while a local battle is active.
- In an Encounter, the player may play at most one basic non-A card during each eligible player action turn. Special reaction, interrupt, extra-action, and non-turn windows are separate timing rules; whether any of them provide a separate card allowance remains blank until the exact Encounter rules are needed.
- In the ACT MVP, clicking `Tab` opens the card repository panel, blurs the background, reveals five cards for the current page, and slows world time to 1/10. Movement and attack input are rejected while this state is active; only basic non-A card-related input is accepted, the battle scene keeps its inertia, and a quick numeric-key press plays the card in the corresponding slot.
- The visible numeric slots are fixed at 1-5 per page. The mouse wheel requests the next page of non-A cards. The total page-count policy (fixed or unbounded) and page-boundary behavior are intentionally deferred until the card UI needs the decision.
- An active ability carried by an ABC card is allowed from the sandbox by default. Exceptional restrictions are configured per ability, and the ability can be activated outside a local battle when its conditions and cost are satisfied.
- Playing an ABC card for placement, attachment, synthesis, or deployment is a different operation from activating one of its abilities.

Encounter and RealTimeAction use the same card repository interaction: `Tab` opens the repository, and the Encounter hand and RealTimeAction numeric slots are one-to-one views of the same basic non-A card mapping. In the ACT MVP, this action also triggers the background blur and 1/10-speed bullet-time state. Whether an ABC active ability is also legal during an Encounter or RealTimeAction battle must be specified per ability or by a later global rule; code must not guess.

## 3. Attribute and tag model

The primary attribute relationship is:

```text
D counters F
F counters R
R counters D
```

Normal counter damage is doubled and weak-match damage is halved. The Punk Era removes counter relationships and the Next-Generation Era doubles them according to the source proposal. Exact priority when multiple modifiers conflict is `To confirm`.

Vector tags are sparse key-value data, for example:

```text
{"Mystery": 80, "Forbidden": 20}
```

The design distinguishes vector tags from species/identity tags. A species tag can affect combat or content rules, but extra/dependency cards do not automatically contribute to A+B vector selection.

## 4. Calendar and era data

The calendar is explicit state, not an implicit modulo calculation:

```text
CalendarState
  ChapterIndex
  EraIndex
  DayIndex
  Phase = Dawn | Morning | Dusk
```

The hierarchy is fixed:

```text
One day = Dawn -> Morning -> Dusk
Seven days = one Era
Seven Eras = one Chapter
```

Every phase transition is an event. Day-end effects, Era changes, and Chapter changes are separate events so that systems can subscribe without deriving hidden state.

The seven Era pool modifiers are data entries, not branches hard-coded into card logic. `F-E`, `F-L`, `R-E`, and `R-L` are preserved as source labels; the taxonomy behind `E` and `L` needs later clarification.

## 5. Resource model

### 5.1 Flow

Flow is used for card draw, card play, summoning, and card abilities. A card may grant or consume flow, and some effects may convert flow to computation or computation to flow. Resource loops are neither universally allowed nor universally forbidden: each loop must be an explicit effect with a real cost, timing, and risk, while luck may compensate the cost in some authored designs.

The source material also proposes a default resource gain related to 50% of the cap and allows limited debt. These are not final MVP constants. The implementation must represent current value, cap, debt, and source of change separately so a balance change does not require a code rewrite.

### 5.2 Computation

Computation is the sandbox operation resource. It is spent on movement, construction, node operations, and selected card effects. It is not a second name for flow.

The source examples propose:

- Move an A card: 2 computation.
- Move a C character or a configured failure-side object: 4 computation.
- Move a construction: 6 computation.
- Move the core: a card-count-dependent cost, potentially allowing debt.

These values are provisional examples for the numerical review.

### 5.3 Global state values

The MVP state model distinguishes:

- `GlobalEntropy`
- `Faith`
- `CoreLife`
- `WarBalance`
- `Threat`
- `Flow` and `FlowCap`
- `Computation` and `ComputationCap`
- `Stability` for cards that use it

Every modification should have a source, a reason, and an event or command ID in debug builds.

## 6. Hand, deck, discard, and card state

The source material distinguishes ABC cards from basic non-A hand cards and states that ABC cards enter the discard pile. Basic non-A cards are battle-only hand skills, while ABC active abilities can be activated outside a local battle. These activation permissions must not be confused with deck location. The source also describes end-of-day return rules, top/bottom placement, burn/banish effects, hand limits, and a depletion penalty.

For MVP, the data model must support:

- Draw pile, hand, discard pile, and optional extra pile.
- Card movement to top, bottom, discard, burn, or banish.
- Card-specific return timing.
- A hand limit and a visible reason when a draw is refused.
- Deterministic reshuffle with a seed.

Basic non-A card lifecycle is intentionally card- or effect-dependent. After a play, a card may enter discard, return to a pile, or enter cooldown according to data. Encounter applies the one-card-per-eligible-player-turn cap; non-turn-based modes have no per-turn count cap, but still obey lifecycle, cost, cooldown, and other data-defined restrictions. The exact probabilities for discard, return, or cooldown, and the total number of card pages, are intentionally blank and must be confirmed when the relevant card implementation begins. The legal activation contexts above are already part of the clarified contract; implementation must not silently collapse them into one generic card-play path.

## 7. A+B synthesis pipeline

The MVP interpretation is a layered pipeline with exactly two primary outcomes: success or failure.

### 7.1 Resolution order

1. Validate the input card IDs, family, ownership, availability, and costs.
2. Resolve the A and B positions, attachment, upright/reversed state, and current field values.
3. Combine the A and B sparse vector tags into `V`.
4. Build the eligible C candidate set. A candidate is normally eligible when the vector shares a permitted tag with it.
5. Score each candidate using the vector dot product.
6. Apply Softmax with a configured temperature to select one candidate C.
7. Compute the success probability from resonance distance, A affinity, global entropy, and summoner faith.
8. Use a seeded random stream to resolve success or failure.
9. If the result is failure, grant no reward. If Sigma dice are retained, resolve only a separately documented non-reward failure consequence.
10. Emit a result and events containing enough information for preview, replay, save, and debug inspection.

Softmax chooses *which candidate is attempted*. It does not itself decide whether the selected C successfully appears.

### 7.2 B attachment and direct B play

When B is attached to A, the B field is the context in which the A flow and entropy tendencies accumulate until the synthesis is executed. Entering the probability stage consumes the configured computation cost.

When B is played without an A:

- The direct synthesis path may still enter vector calculation.
- The A weights are treated as `1` for this path.
- Mutation properties that normally travel from B into the generated C are not granted.

The exact timing of accumulation and whether a B can remain as an independent field object are part of the implementation acceptance test.

### 7.3 Candidate selection

For a candidate `i`, the working score is:

```text
S_i = dot(V, CandidateVector_i)

Softmax_i = exp(S_i / T) / sum(exp(S_j / T))
```

`T` is a configured temperature. The candidate selection random roll and stream position must be recorded in debug/test builds.

If no shared vector tag exists, the source proposes an equal-random C fallback. The fallback increases risk and may impose a daily entropy cost while the generated result remains on the field. Its exact MVP duration and removal cost are `To confirm`.

### 7.4 Success probability

The working source formula is:

```text
D = abs(BFinalFlow - CTargetFlow)
  + abs(BFinalEntropy - CTargetEntropy)

P_success = (100 - D
             + M_affinity
             + M_global_entropy
             + M_faith) %
```

Affinity modifiers from the examples are:

```text
Universal receptor      +10%
Intermediate receptor  -10%
Alien receptor          -20%
```

The global entropy modifier is described as:

```text
M_global_entropy = -(CurrentGlobalEntropy / 10) ^ 1.5
```

The current candidate faith function is:

| Faith x | `M_faith(x)` |
|---|---|
| 95 <= x <= 100 | `10 + 1.0 * (x - 95)` |
| 80 <= x < 95 | `4 + 0.4 * (x - 80)` |
| 40 <= x < 80 | `-4 + 0.175 * (x - 40)` |
| 20 <= x < 40 | `-15 + 0.55 * (x - 20)` |
| 0 <= x < 20 | `-33 + 0.9 * x` |

Probability clamping, out-of-range entropy behavior, and the unit interpretation of the modifiers must be confirmed before coding. The MVP must still expose a single success/failure result, never a hidden partial-success state.

### 7.5 Sigma dice and failure side

The current project clarification is authoritative for this baseline:

- Sigma dice do not replace the primary success formula.
- A failed synthesis remains a failure and grants no reward.
- If Sigma dice are retained, their role can only be a separately documented non-reward failure consequence; they must not create a hidden success or partial success.
- There is no third primary outcome called partial success.

Whether Sigma dice remain in the synthesis path, and the number of dice, sides, thresholds, and non-reward consequence table, are open numerical design items.

### 7.6 Preview parity

Preview and execution must call the same pure resolution service. A preview may expose candidate probabilities, success probability, expected failure-side categories, costs, and debug seed information. It must not consume cards, resources, random streams, or world state.

## 8. World-state mutation model

Every persistent change should be representable as:

```text
Command -> validation -> state change -> event -> history/save
```

Examples include node capture, road opening, facility destruction, story choice, weather change, encounter completion, and card deployment. Coordinates are layout data; stable node IDs are identity.

## 9. Proposed MVP data set

The following examples are for validating the model, not for freezing content balance:

- A: `地痕`, `安提帕鸿门宴`
- B: `蓍`, `埃及艳后`
- C: `犹格索托斯`, `希律的舞女`, `维特鲁威人`
- Failure-side consequence examples, only if explicitly non-reward: `一个归来的王`, `约瑟夫残影`
- Basic non-A battle skills: one direct attack, one true-damage action, one protection/recovery action

The recommended paths shown on the example cards, such as `地痕 + 蓍 -> 犹格索托斯`, are content hints and not a guarantee that the Softmax candidate is always forced.

## 10. Open rule decisions

- Exact initial values and caps for flow, computation, life, faith, entropy, and war balance.
- Whether all probabilities are clamped to 0%-100% and at which stage.
- Exact B-field accumulation timing and the computation cost boundary.
- Exact no-shared-tag fallback behavior.
- Basic non-A card lifecycle and whether ABC active abilities are also legal during battle.
- Whether Sigma dice remain, their format, and their non-reward failure consequence.
- Card upgrade/lifecycle timing remains open. Native GAS is planned for both battle modes under ADR-0004; this does not decide the full card lifecycle or upgrade rules.
- Exact Encounter action-bar precedence, initiative modifiers, reaction/interrupt priority, extra-action rules, non-turn windows, and end-turn effects.
- Remaining ACT UI details such as dismissal/re-entry behavior and page-boundary behavior; the MVP trigger (`Tab`), background blur, 1/10 world-time scale, movement/attack input rejection, five visible slots per page, and Tab/card-repository mapping are fixed. The total page count is intentionally deferred.

## 11. Learning targets

This baseline is intended to teach data contracts, `USTRUCT`-style rule data, deterministic random streams, pure services, event-driven state changes, resource accounting, formula testing, preview/execution parity, and versioned save design.

## 12. Card collections and ACT loadout (2026-09-08)

Separate three concepts: (1) the turn-based/ABC card system, including characters deployed on the sandbox for the Encounter route; (2) the separate ACT character-card collection used by battle-settings character/weapon selection; (3) basic non-A skill cards whose identity/slot mapping is shared but whose mode effects are authored separately.

TASK-020 owns the turn-based/ABC lifecycle and shared basic-skill contracts. TASK-035 owns ACT character availability and roster/loadout state. ACT character cards do not automatically participate in ABC synthesis, sandbox deployment or the turn-based pile lifecycle. Definition IDs, owned instances and battle entities are different identities; there is no automatic character-card conversion at mode entry.

An ACT terrain restriction explicitly targets a character, weapon, ability or trait. Exact restriction behavior and roster/preset-versus-run persistence remain D20; restricting a trait does not silently forbid the whole character. Every unaccepted numerical input is individually user-confirmed under D03. See [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md) and the [decision register](../../Production/MVPDecisionRegister.md).
