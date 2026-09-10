# MVP Design Baseline 01 - Main Planner

- Document ID: `DESIGN-MVP-MAIN-001`
- Status: High-level MVP baseline accepted; explicit deferred items remain open
- Baseline accepted: 2026-09-02
- Canonical language: English for Agent consumption
- Chinese companion: `Docs/Design/ZH/01_MainPlanner.md`
- Owner: Chief Planner
- Source material: `mygame.pdf`, `mycard.pdf`, and the latest clarifications in the project discussion

## 1. Baseline contract

The user accepted this document's high-level interpretation of the playable MVP on 2026-09-02. It is the implementation baseline, not a claim that every numerical value, card, map detail, or priority rule is final.

Any item marked `Open`, `Proposed`, or `To confirm` must not be silently treated as a canonical rule. A disagreement between this baseline and the source material must be reported and resolved in the review record.

## 2. Product definition

Dream of Padma is an independent game whose primary purpose is to express a complete original worldview and a coherent system encyclopedia. It is not optimized around becoming a mass-market blockbuster.

The MVP should prove that the project can connect:

1. A data-driven card and synthesis system.
2. A shared sandbox whose state changes as a result of player operations.
3. A time structure of phases, eras, and chapters.
4. Asymmetric conflict against a stronger ruler faction.
5. A strategic sandbox layer with a rule-driven turn-based Encounter battle.
6. A real-time ACT battle route in the MVP, with room for FPS and additional mini-game modes while sharing deterministic rule contracts.
7. A production workflow that can later expand the number of battle modes without making every mode a first implementation dependency.

The intended player experience is to make a meaningful operation, observe its systemic consequence, and then plan around the changed world rather than simply accumulate power.

## 3. MVP success statement

The MVP is successful when a player can complete one short, repeatable sandbox-to-battle run and understand why the world changed.

The accepted MVP baseline contains:

- One seven-day Era inside a structurally correct seven-Era Chapter.
- One small shared sandbox map with approximately eight nodes.
- One player core and one ruler core.
- One ruler patrol policy, one reinforcement/counterattack response, and one boss encounter.
- A small card catalogue containing basic cards, at least two A cards, two B cards, two C cards, and one non-reward failure-side consequence definition.
- The full A+B candidate-selection and success/failure pipeline, including preview parity and seeded reproduction.
- One node capture, one node mutation, one anecdote event, and one safe-house or recovery node.
- A playable strategic/RTS-like sandbox and one complete Encounter with its condition-driven action timeline.
- One complete playable ACT RealTimeAction battle loop, from entry through card use and battle result. FPS and other RealTimeAction variants remain extension routes.
- A pre-battle autosave and a save/load round trip for the current run.

The exact starting values, card list, map layout, and boss identity remain implementation/playtest decisions and are not fixed by this baseline.

## 4. Player loop

```text
Prepare hand and resources
        -> perform strategic sandbox operations
        -> optionally activate an ABC-card active ability outside battle
        -> choose a legal local battle route
           -> Encounter: resolve the condition-driven action timeline, priority,
              reactions, interrupts, extra actions, and legal basic non-A card plays
           -> RealTimeAction/ACT: click `Tab` to blur the background, reveal five cards,
              and enter 1/10-speed bullet time; press a numeric card slot and resolve
              the card effect in real time
        -> resolve the battle result and return to the sandbox
        -> update entropy, faith, threat, and war balance
        -> save at a safe boundary and continue the next day
```

The sandbox is the strategic layer. It uses phase-bound operations and may be presented with RTS-like selection and commands, but it is not itself a real-time battle.

An Encounter is created when an ABC-card-controlled character or unit moves onto a point where an enemy is staying. That point becomes the local battle site, presented as an Onmyoji-like turn-based battle. Its order is not a fixed player-then-ruler alternation: initiative and action-bar rotation are determined by battle conditions such as terrain and card abilities, and the battle supports priority, reactions, interrupts, extra actions, and non-turn ability windows. The high-level behavior is fixed, but the exact action-bar formula and precedence table are intentionally left blank until the implementation reaches that rule.

A basic non-A hand card remains a battle skill: in Encounter, the player may play at most one during each eligible player action turn. The one-card-per-turn cap does not apply outside the turn-based Encounter route, although card lifecycle, costs, cooldowns, and other data-defined restrictions still apply. The exact lifecycle probabilities are intentionally deferred until the relevant card implementation needs them.

The first RealTimeAction mode is ACT. In the MVP, clicking `Tab` opens the card repository panel, blurs the background, reveals five cards for the current page, and enters bullet time at 1/10 of normal world speed. While this state is active, movement and attack input are rejected; only basic non-A card-related input is accepted. The battle scene keeps its inertia instead of becoming a hard pause. A quick numeric-key press plays the card in the corresponding slot and applies its active effect. The visible numeric card slots are 1-5 per page; the mouse wheel requests the next page. Whether the total page count is fixed or unbounded, and its wrap/termination behavior, are intentionally left open until the card UI is implemented.

Encounter and ACT use the same card repository interaction: pressing `Tab` opens the repository, and the Encounter hand and RealTimeAction numeric slots are one-to-one views of the same basic non-A card mapping. In ACT, this `Tab` action also performs the MVP background-blur and bullet-time transition described above.

Active abilities carried by ABC cards are a separate activation path. They are allowed from the sandbox by default, while exceptional restrictions are configured per ability. They can therefore be activated without entering an Encounter or RealTimeAction battle. Whether an ABC ability is also legal during a battle remains an explicit per-ability or global-rule decision.

## 5. Calendar and phase model

The calendar is fixed and hierarchical:

```text
One day     = Dawn -> Morning -> Dusk
Seven days  = one Era
Seven Eras  = one Chapter
```

The game must store `ChapterIndex`, `EraIndex`, `DayIndex`, and `Phase` explicitly. The MVP plays one seven-day Era, but it must be able to display and test the surrounding boundaries through development commands.

The seven Era themes from the source material are retained as data:

1. Totem Era: F-E cards are strengthened and their pool is favored.
2. Obscurant Era: F-L cards are strengthened and their pool is favored.
3. Enlightenment Era: D cards are strengthened; the source suggests more ways to extend this Era.
4. Vengeful Era: R-E cards are strengthened and their pool is favored.
5. Modern Era: R-L cards are strengthened and their pool is favored.
6. Punk Era: counter relationships are removed.
7. Next-Generation Era: counter relationships are doubled.

The meanings and exact taxonomy of the `E` and `L` sublabels are not defined in this MVP baseline and must not be invented by implementation.

## 6. MVP phase behavior

| Phase | Baseline player-facing behavior |
|---|---|
| Dawn | Apply start-of-day effects, resolve scheduled arrivals, draw or refresh cards, and synchronously grant configured resources. |
| Morning | Issue sandbox movement, placement, attachment, synthesis, construction, and eligible ABC active-ability commands. Enter a legal local battle from any calendar phase when its world conditions are met; the local battle then follows its own action timeline or ACT loop. |
| Dusk | Resolve non-immediate damage, field effects, entropy and time effects, discard/return rules, threat changes, and end-of-day checks. |

Encounter initiative and action-bar rotation are driven by explicit conditions, including terrain and card abilities. The implementation must represent priority, reaction, interrupt, extra-action, and non-turn windows as typed rules and use a seeded stream wherever a roll is involved. The exact precedence table and action-bar calculation are deliberately deferred and must remain blank rather than being invented by an Agent.

A local battle is a separate mode transaction. Entering it pauses sandbox time and records a complete versioned snapshot of all battle-relevant run state. A successful result commits the battle changes; defeat or player exit restores the complete pre-battle snapshot with no partial battle state left behind before returning to the sandbox.

## 7. Core systemic pillars

### 7.1 Cards

- A cards represent roots, bases, or egos. Its attribute is F, confirmed by the user for D22d on 2026-09-08; conflicting collection labels require revision.
- B cards represent gospels, media, or catalysts. Its attribute is D, confirmed by the user for D22d on 2026-09-08; conflicting collection labels require revision.
- C cards represent terminals, creations, or selves. Its attribute is R, confirmed by the user for D22d on 2026-09-08; conflicting collection labels require revision.
- C cards can function as constructions, characters, events, or items.
- Basic non-A cards are battle skill cards, not general sandbox actions. They can be played only while a local battle is active: at most one during each eligible player action turn in Encounter, or through the ACT MVP `Tab` bullet-time plus numeric-slot input route in RealTimeAction. Outside the turn-based route there is no per-turn count cap; lifecycle and other costs remain data-defined and their probabilities are deferred.
- Active abilities carried by ABC cards are allowed from the sandbox by default, with exceptional restrictions configured per ability. This is ability activation, not the same operation as playing a basic non-A hand card.
- Basic non-A cards and an extra/deferred card pool are separate content categories.
- Cards can have upright and reversed states.
- Stable gameplay IDs are independent from display names and asset paths.

### 7.2 Resources

- Flow is primarily used for card draw, summoning, and card abilities.
- Computation is primarily used for sandbox movement and world operations.
- Global entropy is a risk and a resource-control pressure, not only a punishment.
- Faith affects synthesis and can also be a victory or defeat condition.
- Core life, stability, threat, and war balance are separate values.
- Resource conversion loops are not universally legal or universally forbidden. A loop must be an explicitly authored effect with a real cost, timing, and risk; luck may compensate that cost in some designs, but the loop must not be free. The MVP must make the price and stochastic compensation observable rather than banning the possibility in architecture.

### 7.3 Shared sandbox

The player and ruler occupy the same map. The player begins with less territory and must capture a route toward the ruler core. Node ownership, facilities, patrols, discoveries, weather or terrain modifiers, and story choices can change the future map state. The engagement route of a local battle is jointly constrained by both sides; terrain, location choices, and card abilities may restrict the available distance or route. The resolved battle route can trigger Encounter, ACT, or a future RealTimeAction presentation, and can also change story progression.

### 7.4 Conflict and victory

The source material defines a war-balance tug-of-war beginning at 50/50. High-impact events, such as the death of an A card, construction or destruction of a facility, or a serious core threat, move the balance rather than causing a constant linear drift.

The source also describes victory through core destruction, faith collapse, and special thematic achievements such as miracles or human traces. The MVP primary victories are confirmed as:

1. Reduce the ruler core's life to zero.
2. Move war balance completely to the player side.

Faith collapse and themed victory grades remain future-compatible outcome definitions. The outcome system must support adding the other documented victory and defeat methods later without changing the MVP resolver.

## 8. Presentation and mode scope

The strategic sandbox uses RTS-like selection and command presentation: select cards or units, issue movement and world-operation commands, observe the shared map, and react to the ruler.

Encounter is the turn-based local battle mode with a condition-driven action timeline. RealTimeAction is the real-time local battle family; ACT is the first MVP mode, followed by possible FPS or other mini-game presentations. The architecture must expose shared mode and card-activation commands so these routes can use the same rule state while changing camera, possession, timing presentation, and input adapter. The jointly resolved engagement route is a gameplay trigger that can select the presentation mode and affect story progression; it must not be reduced to a cosmetic camera choice.

The MVP includes one complete playable ACT RealTimeAction battle loop. The exact data model for jointly constrained engagement routes and their story triggers remains a review item; FPS and other modes are future extension routes.

## 9. Explicit non-goals for this baseline

- Completing all 49 days of a full Chapter as content.
- Implementing all seven Era decks and the complete card encyclopedia.
- Finalizing every Arcana/Enlightenment card.
- Implementing the complete GAS-based card lifecycle and all upgrade rules.
- Online networking, SDK integration, hot update, and platform release support.
- Full World Partition, PCG, weather, water, foliage, and cinematic production.
- Production-quality Substrate Toon art direction; a rendering spike can be a separate technical task.

These are future expansion targets, not rejected features.

## 10. Baseline acceptance record

The user accepted the following high-level interpretation on 2026-09-02:

- [x] The sandbox is a strategic/RTS-like layer, Encounter is a turn-based battle with a condition-driven action timeline, and RealTimeAction is a separate ACT/FPS/other battle route.
- [x] The fixed calendar is Dawn/Morning/Dusk, seven days per Era, and seven Eras per Chapter.
- [x] The MVP may play one Era while retaining the complete Chapter hierarchy in data and test commands.
- [x] Flow and computation remain distinct, and only explicitly authored, paid/risky resource loops are allowed.
- [x] A+B synthesis first selects a candidate C through tag-vector scoring and Softmax, then resolves only success/failure through resonance, affinity, entropy, and faith.
- [x] Synthesis failure grants no reward; any remaining Sigma-dice role must not create a reward or partial success.
- [x] The shared sandbox and the ruler's asymmetric patrol/counterattack are necessary MVP content.
- [x] The two MVP victory conditions are confirmed: ruler core life reaches zero, or war balance reaches the player victory boundary. Additional documented outcome types remain extensible future data.
- [x] Basic non-A hand cards can be played only in a local battle; Encounter permits at most one during each eligible player action turn.
- [x] In the ACT MVP, clicking `Tab` opens the card repository, blurs the background, reveals five cards, changes world time to 1/10, rejects movement and attack input, preserves scene inertia, and accepts only basic non-A card input; a quick numeric-key press plays the matching slot.
- [x] `Tab` opens the shared card repository, and Encounter hand cards map one-to-one to RealTimeAction slots; total page count is intentionally deferred.
- [x] ABC-card active abilities are sandbox-available by default, subject to per-ability restrictions.
- [x] A local battle pauses sandbox time, commits on success, and restores the pre-battle snapshot on defeat or exit.
- [x] The first MVP contains one complete playable ACT RealTimeAction battle loop.

## 11. Learning targets

This baseline is intended to teach product scoping, core-loop definition, system decomposition, requirements writing, acceptance criteria, versioned design decisions, and communication between design and engineering.

## 12. Full-MVP delivery clarification (2026-09-08)

The complete seven-day MVP is now decomposed in [TASK-011](../../Production/Tasks/TASK-011-Full-MVP-Task-Cluster.md). Both battle modes use native GAS while retaining different configuration and execution systems. Turn-based character cards are deployed on the map; a separate ACT character-card collection supplies the characters/weapons selected in battle settings. Terrain may restrict an ACT character-card trait. Basic non-A battle skill cards continue to share identity/slots, with mode-specific effects.

The user will import placeholder assets once implementation starts; detailed animation and final rendering are later art work. Every previously unaccepted numerical value, including a test-profile value, requires individual user confirmation. These decisions preserve the full MVP scope while avoiding a mandatory final-art dependency. Exact roster and restriction policies remain in the [decision register](../../Production/MVPDecisionRegister.md).
