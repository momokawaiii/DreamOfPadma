# Combat Rules

- Chinese companion for user reading: `Docs/Rules/Combat.zh-CN.md`

Combat rules are the normative contract for local battle behavior. Numerical values remain configurable data until the user accepts the balance.

## HTML clarification (2026-09-09)

The user accepted a separate discard pile for shared basic skills: played cards enter it, and exhaustion recycles the cards while preserving the shared slot identities. ABC discard remains separate. The HTML prototype may offer separate Encounter and ACT entry buttons until the joint route rules are designed. This does not turn FPS into a playable route.

The user clarified C-card resistance to basic cards: physical damage is affected, while true damage is not reduced by that resistance. Thus One Cut's card-face 3 true damage remains 3 before any separately defined shield/block effects. The exact broader production modifier precedence remains outside this clarification.

## Local battle modes

### Encounter targeting and basic-card origin (2026-09-09)

The user clarified that an ordinary attack starts by pressing the character's attack button, then selecting a living enemy to execute. Targeting offers a Cancel button that spends nothing. HTML defaults to one target and preserves the Dancer's authored two-target option: choose the count in the attack controls, then complete that many distinct enemy selections to execute. No target is implicitly selected. A basic skill is played from the hand, not cast by the active C card. The current eligible player action owns only the one-basic-card allowance; it is not the skill's damage source. Basic effects originate from the card/environment and do not inherit that C card's attributes or attack modifiers. Physical basic damage still respects C resistance and true damage remains unaffected by it. Rain is a sky-originating fishbone storm affecting both sides, not projectiles from a character.

The HTML Encounter interaction uses upward hand-card dragging with legal target/drop feedback and a click/keyboard targeting alternative. The user requires release-to-preview followed by an explicit confirmation click; only confirmation consumes and resolves the card. A cancelled or invalid preview consumes nothing. Future UE presentation should support card motion and material/shader effects with Sequencer; numerical resolution and one-time card consumption remain owned by gameplay commands, not animation playback. This does not authorize UE implementation or change the existing ACT input contract.

Long-pressing either friendly or enemy Encounter units opens their portrait, current attributes, skills and descriptions, level and available catalog details. The HTML shows placeholder portraits and explicitly unconfigured levels or missing abilities. Inspection never attacks or pays a cost. Enemy autoplay waits while the details are open; any already committed animation may finish. The user owns browser interaction acceptance for this iteration; further extensive code review is not requested.

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

## Character cards, battle settings and mode-specific GAS (2026-09-08)

Encounter and ACT both use native GAS through separate mode execution and configuration domains; see [ADR-0004](../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md). Turn-based character cards are deployed on the sandbox. ACT character cards form a separate collection; ACT characters, weapons and the approved equipment subset are selected in battle settings. Entering ACT consumes that selection plus the battle's terrain/context, not an assumed conversion of the deployed turn-based card into the same ACT character.

Basic non-A skill cards retain shared identity and repository slot mapping, with separately authored effects for Encounter and ACT. This sharing does not cover character-card identity, live abilities, AttributeSets, cooldown clocks or mode state. Each mode validates and applies its own effects; only explicitly shared rule meaning may reuse pure calculations and typed result/event contracts.

An ACT card can declare a context restriction such as an unavailable trait on a particular terrain. The restriction names its target and rule; a trait restriction must not silently become a whole-character ban. Exact roster capacity, any trigger-unit linkage, restriction validation timing and persistent outcome projection remain in [D20](../Production/MVPDecisionRegister.md). The authoritative path must enforce restrictions even when input bypasses the UI.

Ability interruption or an individual death does not itself end the Encounter; its approved timeline/result rules decide whether a local battle ends. Local defeat/exit rollback remains distinct from campaign defeat.
