# MVP Design Baseline 05 - Numerical Planner

- Document ID: `DESIGN-MVP-NUMERICAL-001`
- Status: High-level numerical baseline accepted; coefficients, clamps, and thresholds remain playtest data
- Baseline accepted: 2026-09-02
- Canonical language: English for Agent consumption
- Chinese companion: `Docs/Design/ZH/05_NumericalPlanner.md`
- Owner: Numerical Planner
- Related contracts: `Docs/Rules/Synthesis.md`, `Docs/Rules/Combat.md`, `Docs/Architecture/SaveSchema.md`

## 1. Numerical objective

The numerical MVP must make the game explainable and reproducible before it attempts to make it large. Every visible outcome should be traceable to inputs, a formula or table, a seeded random roll, and a named source.

The numerical planner owns coefficients, tables, fixtures, simulations, and balance reports. The planner does not change the meaning of a rule without updating the system or combat design document.

## 2. Numerical principles

1. Put coefficients in data, not in gameplay code.
2. Keep candidate selection probability separate from synthesis success probability.
3. Keep flow and computation as separate ledgers. Permit only explicitly authored conversion loops with a real cost, timing, and risk; luck may compensate a cost in some designs.
4. Make global entropy a risk/strategy variable rather than a single punishment meter.
5. Use stable test fixtures and seeded random streams.
6. Test boundary values before tuning average values.
7. Expose a calculation trace in DebugGame, Development, and Test builds.
8. Treat activation context as a rule gate, while keeping card costs, cooldowns, and input timing values data-driven.

## 3. Variable glossary

| Variable | Meaning | MVP use |
|---|---|---|
| `Flow` | Card and ability resource | Draw, play, summon, abilities. |
| `FlowCap` | Flow limit | Hand/resource pressure and debt boundary. |
| `Computation` | Sandbox operation resource | Movement, construction, node operations, selected effects. |
| `ComputationCap` | Computation limit | Debt and recovery boundary. |
| `GlobalEntropy` | Shared instability | Synthesis modifier, disaster pressure, world consequences. |
| `Faith` | Summoner/core belief value | Synthesis modifier, card effects, possible victory/defeat. |
| `CoreLife` | Core life value | Ruler-core life reaching zero is a confirmed MVP victory condition; the same value can support future defeat conditions. |
| `Stability` | A/card or construction durability | Destruction and board-state effects. |
| `WarBalance` | Player/ruler tug-of-war | Reaching the player boundary is a confirmed MVP victory condition. |
| `Threat` | Ruler suppression/escalation value | Patrol, counterattack, and card penalty. |

### 3.1 Activation-related numerical inputs

The activation channel is a rule gate; its tunable values belong in data:

- A basic non-A card pays its configured cost only after a legal local-battle context is established.
- An ABC-card active ability may pay its configured cost from the sandbox without entering a battle.
- Encounter action-bar values, priority, reaction/interrupt windows, extra-action limits, and the one-basic-card-per-eligible-player-action-turn cap are separate from the global day phase and must be named explicitly.
- The ACT MVP interaction contract is fixed at clicking `Tab`, opening a blurred five-card panel, setting world time to 1/10, rejecting movement and attack input, accepting only basic non-A card input, preserving inertia, and using 1-5 visible card slots per page. Duration/toggle behavior, cooldown/resource cost, page-boundary behavior, total page count, and lifecycle probabilities remain data/review items.
- Card lifecycle transitions (discard, return, cooldown, or other authored result) must have an explicit cost or condition; they must not be inferred from the UI.

## 4. Synthesis formulas

### 4.1 Vector candidate selection

For A and B sparse vectors:

```text
V = AVector + BVector
S_i = dot(V, CandidateVector_i)
Softmax_i = exp(S_i / T) / sum(exp(S_j / T))
```

The candidate set is filtered by permitted shared tags. `T` is a configured temperature. The candidate selection roll is separate from the success roll.

If B is used without the A accumulation path, the A weights are treated as `1` and B mutation properties are not transferred to the generated C. This remains a proposed system rule and requires a focused implementation/playtest decision before canonical use.

### 4.2 Success probability

```text
D = abs(BFinalFlow - CTargetFlow)
  + abs(BFinalEntropy - CTargetEntropy)

P_success = (100 - D
             + M_affinity
             + M_global_entropy
             + M_faith) %
```

Working affinity modifiers:

```text
Universal receptor      +10%
Intermediate receptor  -10%
Alien receptor          -20%
```

Working global entropy modifier:

```text
M_global_entropy = -(CurrentGlobalEntropy / 10) ^ 1.5
```

Working faith modifier:

| Faith x | Modifier |
|---|---|
| 95 <= x <= 100 | `10 + 1.0 * (x - 95)` |
| 80 <= x < 95 | `4 + 0.4 * (x - 80)` |
| 40 <= x < 80 | `-4 + 0.175 * (x - 40)` |
| 20 <= x < 40 | `-15 + 0.55 * (x - 20)` |
| 0 <= x < 20 | `-33 + 0.9 * x` |

The clamp to 0%-100%, behavior for negative or very high entropy, and modifier units are not frozen.

### 4.3 Worked example

The following example demonstrates interpretation only:

```text
A = 地痕
  Vector = {奥秘: 80, 禁忌: 20}

B = 蓍
  Vector = {奥秘: 90, 知识: 10}
  Final field = [Flow 4, Entropy 8]

Candidate C = 犹格索托斯
  Vector = {奥秘: 80, 禁忌: 10, 知识: 10}
  Target Flow = 2
  Target Entropy = 10
  Affinity = Universal receptor
```

The combined vector is `{奥秘: 170, 禁忌: 20, 知识: 10}`. The illustrative dot-product score for this candidate is:

```text
170*80 + 20*10 + 10*10 = 13,900
```

The resonance distance is `abs(4-2) + abs(8-10) = 4`. If the current global entropy is `40` and faith is `50`:

```text
M_affinity       = +10
M_global_entropy = -(40 / 10)^1.5 = -8
M_faith          = -4 + 0.175*(50-40) = -2.25
P_success        = 100 - 4 + 10 - 8 - 2.25 = 95.75%
```

This is not a balance approval. It is a review fixture showing that candidate selection and success calculation are separate layers.

## 5. Sigma dice and failure consequences

The current project clarification is:

- The primary synthesis result is only success or failure.
- Sigma dice do not replace or secretly modify the primary success formula.
- On failure, no reward is granted.
- If Sigma dice are retained, they may only determine a separately documented non-reward failure consequence; they must not create a hidden success, partial success, or reward.
- A failure-side consequence is not a third form of successful C generation.

The following table is intentionally empty until a focused implementation decision is recorded:

| Dice input | Non-reward failure consequence | Resource/entropy effect |
|---|---|---|
| To define | To define | To define |

Whether Sigma dice remain in the synthesis path, the number of dice, sides, thresholds, rerolls, non-reward consequences, and whether the dice are shown to the player are open decisions.

## 6. Resource budgets

Source examples to validate during tuning:

| Operation | Source example |
|---|---:|
| Move A card | 2 computation |
| Move C character/configured failure-side object | 4 computation |
| Move construction | 6 computation |
| Enter synthesis probability stage | 3 computation in the sandbox rule example |
| Limited debt | Up to 30% of the current cap in the source note |
| Default resource gain | A source note suggests at least 50% of the cap when no special text exists |

These are not final starting values. The MVP needs named configuration profiles so that a test profile can be generous while a playtest profile can be tuned.

## 7. Combat numerical rules

The MVP numerical pipeline must support:

- Attack damage versus true damage.
- Defense before ordinary attack damage.
- F/D/R counter relationships.
- Status layers and consumption timing.
- Life, stability, and core values as separate pools.
- War-balance event deltas.
- Threat threshold effects.

The source relationship is double damage for a counter and half damage for a weak match. Punk removes the relationship. Next-Generation doubles the relationship, but whether that means double the multiplier or double the bonus is open.

The first numerical test fixtures should cover:

- Base attack against equal defense.
- True damage against high defense.
- D into F, F into R, and R into D.
- Punk and Next-Generation variants.
- Zero, one, and multiple status layers.
- Simultaneous war-balance events.

## 8. War balance and threat tuning

War balance starts at 50/50. It should move through named event deltas rather than an unexplained per-second drift.

Threat should have a daily increment and event-based changes. Thresholds at 25/50/75/100 are retained from the source proposal as candidate data points; the associated player-card penalties and peak response require playtest validation.

No threshold is final until a trace can explain:

```text
previous value + event list + daily tick = new value
```

## 9. Test fixtures and balance reports

The numerical test suite should include:

- Faith values `0, 19, 20, 39, 40, 79, 80, 94, 95, 100`.
- Global entropy `0`, a low value, a threshold value, and an extreme value.
- Resonance distance `0`, near match, and over-range match.
- No candidate, one candidate, and multiple candidates.
- Same seed repeated at least twice.
- Success and failure with no reward.
- Explicitly authored paid/risky resource loops with a bounded action count and a stochastic compensation case.

Each balance report should state the profile, seed, card set, number of trials, outcome distribution, and the conclusion. A screenshot or exported trace is preferred for learning evidence.

## 10. Open numerical decisions

- Initial values and caps for every resource and core value.
- Probability clamp and modifier units.
- Exact Softmax candidate filtering and normalization.
- Whether Sigma dice remain, their specification, and their non-reward failure-consequence table.
- Debt and resource-loop safeguards, including the cost and risk of each authored loop.
- Exact meaning of Next-Generation counter doubling.
- War-balance event magnitudes and victory threshold.
- Threat penalties and peak response.
- Hand limit, flow limit, and depletion penalty.
- Encounter action-bar precedence, initiative modifiers, reaction/interrupt priority, extra-action rules, non-turn windows, and end-turn resource effects.
- ACT MVP duration/toggle behavior, cooldown/resource cost, page-boundary behavior, and total page count; `Tab`, background blur, movement/attack input rejection, 1/10 world-time scale, and 1-5 visible slots per page are fixed.
- Basic non-A discard/return/cooldown probabilities remain blank until the relevant card implementation needs them.
- Whether ABC active-ability costs and cooldowns apply inside battle as well as outside battle.

## 11. Learning targets

This baseline is intended to teach numerical design, probability modeling, Softmax, seeded simulation, boundary testing, data tables, economic loops, combat balance, telemetry, and evidence-based iteration.
