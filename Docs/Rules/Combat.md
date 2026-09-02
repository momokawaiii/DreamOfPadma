# Combat Rules

- Chinese companion for user reading: `Docs/Rules/Combat.zh-CN.md`

Combat rules are the normative contract for local battle behavior. Numerical values remain configurable data until the user accepts the balance.

## Local battle modes

The game separates the strategic sandbox from local battles:

- Encounter is a turn-based battle with a condition-driven action timeline, not a fixed player-then-ruler alternation. Terrain, card abilities, initiative, priority, reactions, interrupts, extra actions, and non-turn ability windows may change which side acts next.
- RealTimeAction is a real-time local battle route. ACT is the first MVP mode; FPS and other mini-game modes are future routes. In the ACT MVP, clicking `Tab` opens the card repository panel, blurs the background, reveals five cards for the current page, and slows world time to 1/10. Movement and attack input are rejected while this state is active; only basic non-A card-related input is accepted, the battle scene keeps its inertia, and a quick numeric-key press selects the corresponding card slot and applies its active effect. Slots are fixed at 1-5 per page, with mouse-wheel requests for the next page. The total page count and page-boundary behavior are deferred.
- The sandbox is not a local battle. It may issue strategic commands and activate ABC-card active abilities from the sandbox by default, subject to per-ability restrictions, without entering Encounter or RealTimeAction.

An Encounter is triggered when an ABC-card-controlled character or unit moves onto a point where an enemy is staying. The point becomes the local battle site and is presented as an Onmyoji-like turn-based battle. The resolved engagement route may select/trigger Encounter, ACT, or a future RealTimeAction presentation and may trigger story consequences.

## Card activation legality

The authoritative validator must distinguish these operations:

1. Playing a basic non-A battle skill card.
2. Activating an active ability carried by an ABC card.
3. Playing or deploying an ABC card for placement, attachment, synthesis, or another world operation.

Basic non-A card play is rejected in the sandbox, outside an eligible player action turn in Encounter, and in any battle mode without its required input context. Encounter accepts at most one basic non-A card during each eligible player action turn; the one-card-per-turn cap does not apply outside Encounter, although lifecycle, cost, cooldown, and other data-defined restrictions still apply. Whether reaction, interrupt, or extra-action windows provide a separate card allowance remains intentionally open with the exact Encounter rules. ABC-card active abilities have a sandbox path by default, while exceptional restrictions are configured per ability. Whether they are also legal during a local battle is still a per-ability or global-rule decision and must not be guessed by the validator.

`Tab` opens the shared card repository in Encounter and ACT. In the ACT MVP it also blurs the background, reveals five cards, and enters 1/10-speed bullet time; movement and attack input are rejected during that state. The Encounter hand and RealTimeAction slots are one-to-one views of the same basic non-A card mapping. Playing or deploying an ABC card for placement, attachment, synthesis, or another world operation remains distinct from activating an ability carried by that card. The total number of pages is not yet defined.

## Resolution separation

The combat service must separate:

- Action-timeline construction, active-action validation, priority, reactions, interrupts, extra actions, and non-turn windows.
- Battle-mode and active-side validation.
- Damage calculation.
- Resource cost.
- Target validation.
- Status application.
- Death and battle resolution.
- Animation, camera, audio, and VFX presentation.

Encounter turns and RealTimeAction ticks may have different timing adapters, but they must call the same authoritative effect, damage, status, and event contracts where the rule meaning is shared.

Indirect damage, true damage, F/D/R attributes, buffs, era modifiers, bullet-time behavior, and card-slot input need explicit examples and automated tests before they become production content.

## MVP verification

The first vertical slice must test:

- A basic non-A card rejected outside battle and outside an eligible player action turn.
- The same card accepted once on the player's Encounter action turn and rejected after the per-turn limit.
- The ACT RealTimeAction route completing one battle loop, including world-time slowdown, inertia-preserving bullet-time input, five-slot page mapping, card effect, and battle result.
- An ABC-card active ability activated from the sandbox without entering a battle, subject to its configured restriction.
- Defeat or exit restoring the complete pre-battle snapshot of every battle-mutated run state while success commits the battle result.
- Damage, status, death, and battle-result events remaining independent from presentation.

The two confirmed MVP victory conditions are ruler-core life reaching zero and war balance reaching the player victory boundary. The outcome contract must remain extensible for the other documented victory and defeat methods.
