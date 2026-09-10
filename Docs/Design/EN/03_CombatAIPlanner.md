# MVP Design Baseline 03 - Combat and AI Planner

- Document ID: `DESIGN-MVP-COMBAT-AI-001`
- Status: High-level MVP baseline accepted; detailed timing and priority rules remain deferred
- Baseline accepted: 2026-09-02
- Canonical language: English for Agent consumption
- Chinese companion: `Docs/Design/ZH/03_CombatAIPlanner.md`
- Owner: Combat / AI Planner
- Related contracts: `Docs/Rules/Combat.md`, `Docs/Rules/WorldState.md`, `Docs/Architecture/RuntimeFlow.md`

## 1. Combat objective

Combat is the local expression of the larger sandbox conflict. It must make card composition, positioning, resource pressure, entropy, faith, and war balance matter at the same time.

The MVP should prove one complete Encounter and one complete ACT RealTimeAction battle loop rather than implement every status, card, AI behavior, or control mode. The rules service must remain independent from animation, Niagara, camera, and actor lifetime.

## 2. Battle mode model

The game has one strategic sandbox layer and two local battle families:

1. Encounter is turn-based with a rule-driven action timeline. It is not a fixed player-then-ruler alternation: terrain, card abilities, initiative, priority, reactions, interrupts, extra actions, and non-turn ability windows may change which side acts next.
2. RealTimeAction is a real-time local battle route. ACT is the first MVP mode; FPS and other mini-game modes are extension routes. In the ACT MVP, clicking `Tab` opens the card repository panel, blurs the background, reveals five cards for the current page, and slows world time to 1/10. Movement and attack input are rejected in that state; only basic non-A card-related input is accepted, the battle scene keeps its inertia, and a quick numeric-key press plays the card in the corresponding slot.

The sandbox is not a local battle. It hosts movement, placement, attachment, synthesis, construction, story choices, and ABC-card active abilities that are allowed by their data. ABC-card active abilities are allowed from the sandbox by default and can be activated outside a battle without entering Encounter or RealTimeAction; exceptional restrictions are configured per ability. Whether an ABC ability is also legal during a local battle remains a per-ability or global-rule review item.

An Encounter is triggered when an ABC-card-controlled character or unit moves onto a point where an enemy is staying. That point becomes the local battle site and is presented as an Onmyoji-like turn-based battle. The resolved engagement route can select Encounter, ACT, or a future RealTimeAction presentation, and can trigger story consequences.

Basic non-A cards are battle skill cards, not general sandbox actions:

- Encounter: the active player must be the player, and at most one basic non-A card may be played during each eligible player action turn. Whether reaction, interrupt, or extra-action windows provide a separate card allowance remains blank with the exact Encounter rules.
- RealTimeAction: the local ACT mode must be active, the ACT MVP `Tab` repository/bullet-time state must be active, and the numeric slot must identify the card. Movement and attack input are rejected in this state. The visible slots are fixed at 1-5 per page; the mouse wheel requests the next page. The total page count and page-boundary behavior are deferred until the card UI needs them.
- Sandbox or any other non-battle context: reject the play request.

The source material's four Morning action categories (play A, play B, play C, and use C) describe legal sandbox/card operations. They do not turn an Encounter into a real-time battle or remove the turn gate for basic non-A cards. The underlying command and rule services remain deterministic and can be driven by player input, GM commands, tests, or future network messages.

## 3. Calendar and local battle resolution

| Boundary | Strategic/combat responsibility |
|---|---|
| Dawn | Apply start-of-day effects, scheduled arrivals, resource gains, and recovery that is explicitly daily. |
| Morning | Resolve sandbox movement, placement, card reactions, synthesis, construction, story actions, and eligible ABC active abilities. A legal local battle may be entered from the current world state in any calendar phase; entering it pauses sandbox time. |
| Dusk | Resolve phase-bound damage, field and time effects, entropy, threat, hand-pile transitions, and end-of-day consequences. |

Global day phases do not replace local battle timing:

- Encounter uses a condition-driven action timeline. The minimum MVP contract is battle start, initiative/action-bar construction, active action resolution, priority, reaction, interrupt, extra-action and non-turn windows, legal basic non-A card play, and typed battle-result events.
- RealTimeAction uses an ACT action/tick loop and its own GAS execution and tables. The mode owns world-time scaling, the bullet-time window, card-slot dispatch and effect resolution. Encounter owns a separate resolver; only explicitly identical rule meanings may reuse pure calculations and typed events.
- A battle returns a typed result to the sandbox. It must not mutate the world by bypassing the normal command/event path; defeat or exit restores the pre-battle snapshot, while success commits the result.

The working contract no longer treats encounter and distance combat as two timing variants. Encounter is turn-based; RealTimeAction is a separate real-time route. If a source term such as distance battle is retained, it must be mapped to one of these routes before implementation.

Initiative means the rule-defined ordering of the first and subsequent Encounter actions. Terrain, card abilities, and other conditions can modify the action bar. Any random tie-break or initiative roll must use a named seeded stream, and the precedence table must be data-driven.

## 4. Combat entities

The first encounter uses:

- A player core with life, faith, flow, computation, and war-balance contribution.
- A ruler core with the same fundamental defeat-facing values and asymmetric advantages.
- A small number of A cards that act as roots or board anchors.
- C character units and one C construction.
- One explicitly configured non-reward failure-side consequence, if the sample encounter needs one.
- One basic non-A battle skill card for direct damage and one for protection or recovery.

Every entity has a stable gameplay ID. An Actor, Pawn, or UObject pointer is never the identity stored in a save or event history.

## 5. Damage and status vocabulary

The source distinguishes attack damage from true damage:

- Attack damage is reduced or evaluated by defense.
- True damage bypasses ordinary defense.
- `Hit` means that life or stability was successfully reduced.

The source status vocabulary is retained, but several definitions need a precise review before full implementation:

| Status | Working interpretation |
|---|---|
| Strong | Attack +1; the used layers are consumed after use. |
| Shape | A layered effect involving true damage according to the source; the phrase that it is also a true shield conflicts with the earlier description and must be clarified. |
| Counterforce | Each layer blocks one instance of damage. |
| Taunt | Higher value receives priority; default value is zero. |
| Returning Ruin | Each layer blocks one point/instance of damage according to the source; its distinction from Counterforce is open. |
| Internal Injury | Stackable poison-like effect; each trigger deals one damage and consumes one layer. |
| Empty Gate | Each layer blocks one abnormal-status application. |

For the MVP, use only the minimum status subset required by the sample encounter. Do not silently implement both `Shape` and `Returning Ruin` as the same shield.

## 6. Attribute counter rules

```text
D counters F
F counters R
R counters D
```

The normal relationship doubles counter damage and halves weak-match damage. Punk removes the relationship, and Next-Generation doubles the relationship according to the source proposal.

The damage pipeline should expose the following stages for debugging and numerical tests:

```text
base action
 -> attack/true damage classification
 -> attribute relationship
 -> defense and shield handling
 -> status reactions
 -> life/stability change
 -> hit event
```

Default range from the source is skill range `1` and ordinary attack range `0` unless a card says otherwise. This is a provisional default, not a reason to hard-code range into every ability.

## 7. War balance and high-impact events

War balance starts at a perfect 50/50. It is a visible tug-of-war between the player and ruler.

The balance is moved by high-impact events rather than a constant linear motion. MVP events include:

- Destruction or removal of an A card.
- Construction or destruction of a facility.
- A serious threat to a core's life or faith.
- An explicit item, ability, story choice, or victory action.
- A configured consequence of an entropy disaster.

When the balance reaches the player's victory boundary, the player wins. This is one of the two confirmed MVP victories; the other is ruler-core life reaching zero. The exact numeric boundary and the effect of competing simultaneous events require numerical review. The outcome system remains extensible for the other documented victory and defeat methods.

## 8. Core life, faith, and entropy

Core life and faith are separate defeat-facing values. Faith also modifies synthesis and can be changed by cards, events, and daily effects.

Global entropy is not only a penalty. The encounter should contain at least one safe low-entropy line and one high-risk line that can produce a meaningful advantage or disaster. The disaster must be attributable to the side that caused it and must affect war balance or future state according to explicit data.

The MVP does not require the full disaster encyclopedia. It requires one deterministic low-risk path and one deterministic testable high-risk path.

## 9. Ruler AI model

The ruler is asymmetric: it begins with more territory and stronger resources, and it has better map familiarity.

### 9.1 Required MVP behaviors

1. Patrol a configured route with a `Lawkeeper` unit that does not spend computation on ordinary patrol.
2. Detect or fail to detect a player unit according to a simple sight rule.
3. Reinforce a nearby facility or node after it is captured.
4. Launch a counterattack when the captured node is under-protected.
5. Defend the ruler core and choose a legal target for the boss encounter.
6. Increase threat on the daily schedule and respond when a threshold is reached.

The source states that a player unit outside the Lawkeeper's sight can enter stealth and ignore the ruler threat penalty on C cards. The MVP should implement one visible stealth route and one visible route that triggers a response.

### 9.2 Threat escalation

Threat rises gradually each simulation day. It also rises sharply after a successful ruler counterattack or a player choice that benefits the ruler, and falls after a successful node capture or an explicitly liberating anecdote action.

At peak threat, the source proposes clearing threat and sending a powerful Angel to force a battle at a player facility. This Angel response is optional for the first build; the threshold and reset behavior must be represented in the data model even if the full encounter is deferred.

### 9.3 AI decision policy

MVP AI is a deterministic utility/priority policy, not an LLM-driven autonomous agent. Each decision should expose:

- Candidate actions.
- Preconditions.
- Score or priority reason.
- Chosen action.
- Seeded tie-break information.

This makes AI behavior debuggable and gives the project a clean future seam for more advanced policies.

## 10. Control and battle modes

The strategic sandbox uses an RTS-like presentation for selection, movement, placement, world operations, and ruler response. This is the strategic layer, not the definition of Encounter combat.

Encounter is the turn-based battle mode with a condition-driven action timeline. RealTimeAction is the real-time battle family; ACT is the first MVP mode and FPS or other mini-game presentations are future adapters. The combat command contract must not know whether input came from a top-down cursor, a third-person pawn, or a first-person weapon. The input adapter and mode profile translate presentation input into typed commands:

- Sandbox strategic commands.
- Encounter action-timeline, reaction, interrupt, extra-action, and end-turn requests.
- ACT RealTimeAction bullet-time entry and numeric-slot card-play requests.

The exact joint resolution data for the battle route, the constraints imposed by both sides, and the mapping from route conditions to story triggers remain open design decisions. It is confirmed that the route can select/trigger Encounter, ACT, or a future RealTimeAction presentation and can affect story progression. The first RealTimeAction variant is ACT, and the MVP requires one complete playable ACT battle loop.

## 11. MVP Encounter baseline

The first Encounter should contain:

- One player-controlled anchor A card.
- One player C character and one player construction.
- One ruler patrol and one ruler defensive unit.
- One legal synthesis route and one failed-synthesis test route.
- One node-local terrain modifier.
- One direct attack, one true-damage action, and one status effect.
- One basic non-A card that is rejected in the sandbox, accepted on the player's eligible action turn, and rejected outside that turn; the Encounter fixture must also demonstrate at most one such card per eligible player turn. Whether reaction, interrupt, or extra-action windows grant a separate card allowance remains deferred with the exact Encounter rules.
- One ABC-card active ability that can be activated from the sandbox without entering a battle.
- One high-impact war-balance event.
- Both confirmed MVP campaign victory conditions must be testable: ruler-core life reaching zero and war balance reaching the player victory boundary. The local battle must also expose a configured defeat result that restores the pre-battle snapshot.

Random rule outcomes in Encounter and ACT must reproduce with the same controlled inputs, definition versions, logical timing and seed, and expose a readable battle log. This is not an arbitrary ACT physics replay guarantee. The ACT slice demonstrates slowdown, inertia-preserving bullet-time input, five-slot mapping, card dispatch and a complete result through its own mode-specific effect resolver; basic skill identity/slots remain shared.

## 12. Combat acceptance checklist

- [ ] Encounter has an explicit condition-driven action timeline with initiative, priority, reactions, interrupts, extra actions, and non-turn windows.
- [ ] Attack damage and true damage take different resolution paths.
- [ ] At least one F/D/R relationship is visible and testable.
- [ ] Movement, placement, attack, and node capture work through commands.
- [ ] A basic non-A card is rejected outside battle and off the player's eligible Encounter action turn; no more than one is accepted in that turn.
- [ ] An ABC-card active ability can be activated from the sandbox by default, subject to its configured restrictions.
- [ ] The ACT MVP `Tab` action opens the repository, blurs the background, reveals five cards, slows world time to 1/10, rejects movement and attack input, preserves scene inertia, accepts only basic non-A card input, and maps quick numeric-key presses to the matching slot.
- [ ] `Tab` opens the shared card repository, and the Encounter hand maps one-to-one to the ACT slots.
- [ ] War balance starts at 50/50 and moves only through configured events.
- [ ] One ruler patrol, one reinforcement response, and one counterattack can be reproduced.
- [ ] Stealth and detection are visible to the player.
- [ ] A failed synthesis does not become a hidden partial success.
- [ ] The same seed produces the same combat and AI result.
- [ ] A complete pre-battle snapshot restores every battle-mutated run state after defeat or exit, while success commits the result.

## 13. Open combat decisions

- Exact Encounter action-bar precedence, initiative modifiers, response/interrupt priority, extra-action rules, non-turn windows, end-turn effects, and the precise card allowance of those windows. These rules are intentionally blank until the implementation reaches them.
- ACT battle content details beyond the first complete MVP loop.
- ACT MVP trigger, background blur, movement/attack input rejection, and time scale are fixed as `Tab` and 1/10. Duration/toggle behavior, cooldown/resource cost, page-boundary behavior, and total page count remain open.
- Whether ABC active abilities are also legal during an Encounter or RealTimeAction battle.
- Final semantics and distinction of Shape, Counterforce, and Returning Ruin.
- Future outcome extensions beyond the two confirmed MVP victories, including faith collapse and thematic achievements; documented defeat methods must remain data-defined and extensible.
- Exact threat thresholds, counterattack rules, and Angel behavior.

## 14. Learning targets

This baseline is intended to teach Gameplay Framework boundaries, command processing, combat resolution, status effects, GAS preparation, AI state/utility policies, animation timing contracts, physics-facing interactions, deterministic debugging, and strategic-sandbox/Encounter/RealTimeAction input abstraction.

## 15. Independent mode systems and roster (2026-09-08)

Both modes use native GAS under [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), with independent ability/effect tables, clocks, lifecycle and mode runtime. Turn-based character cards deploy on the sandbox. ACT uses a separate character-card collection and the character/weapon loadout selected in battle settings; basic non-A skill identity/slot mapping is shared with different mode effects.

Terrain/context can restrict an ACT card trait. Eligibility and trait restrictions are authoritative checks, including direct commands that bypass UI; they do not silently convert a trait restriction into character exclusion. TASK-035/027/030 own roster authoring, context handoff and execution respectively. Remaining roster/constraint rules and all unaccepted values are user decisions in D20/D03. A unit death or ability interrupt returns to the Encounter timeline unless approved local outcome rules end the battle.
