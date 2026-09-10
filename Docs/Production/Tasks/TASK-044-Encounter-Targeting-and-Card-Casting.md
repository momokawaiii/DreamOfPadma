# TASK-044 Encounter Targeting and Card Casting

- Chinese companion: TASK-044-Encounter-Targeting-and-Card-Casting.zh-CN.md
- Document ID: TASK-044
- Version: 0.2
- Status: Review
- Primary: root; current Local checkout
- Authorization: user requests explicit enemy selection for non-area attacks, basic cards dragged upward from the hand, and effects without a C-unit caster. Rain falls from the sky onto both sides.

## Decisions and scope

Ordinary Encounter attacks retain their unit source and authored target limit. The player's latest amendment requires attack-first input: press Ordinary Attack, then click a living enemy to release immediately, or Cancel/Escape without spending. HTML defaults to one target; the Dancer can choose two in the controls and releases on the second distinct enemy click. Long-press either side for 500 ms to inspect a portrait, current attributes, skills/descriptions, level and available card details. Unconfigured level/portrait/abilities are labelled honestly. Basic skills use the current player action only to validate the one-card allowance. Their effect source is the played card/environment, not that C unit or its F/D/R/attack attributes. Physical C resistance, true damage, costs, target scope, shield/block, card discard and battle rollback remain.

This implementation is Encounter HTML only. ACT controls/bindings and UE production remain outside the patch. Record future Sequencer card motion and material/shader presentation as a user requirement, not an implemented UE system. User-confirmed interaction: drag a single-target card onto a legal unit and release to preview, then click Confirm Release to settle; drag Rain into the arena and confirm its all-living-noncore scope. Invalid drop, cancel or Escape consumes nothing. Click/number selects a card for a subsequent explicit target/arena click and confirmation as an accessible alternative, never instant default-target casting.

## Ownership and allowed paths

- Root: Prototypes/HTML/app.js, index.html, encounter-fx.js, encounter-fx.css, encounter-fx.test.cjs, optional card-input.js/card-input.test.cjs, README pair; this task pair; Combat rules, 00_INDEX, ProjectState and Changelog pairs.
- Bounded worker: Prototypes/HTML/engine.js, engine.test.cjs and encounter-events.test.cjs. Change only source identity/attribution and explicit Encounter target validation plus associated tests. No concurrent writes to root-owned files.
- Bounded documentation worker: README pair only; test-maintenance worker: encounter-fx.test.cjs after root releases that file. Earlier reviewers are read-only and stopped on user request. The user will test page interaction; do not add extensive review rounds. Preserve unrelated local changes. No C++, UE assets/configuration, dependency installation, save migration, staging, commit, merge or push.

## Acceptance and checks

1. Press attack before selecting targets; single-target input releases on the valid enemy click. Cancel/Escape does not pay. Dancer retains the explicit two-target option. Refresh selection on actor change. Long-press friendly, enemy or fallen units opens details without casting; movement cancels the hold and the resulting click is suppressed after inspection. Right-click or focused I is an alternative. Enemy autoplay waits while inspecting; an already committed animation may finish.
2. Dragging a basic card upward visibly lifts the card and highlights legal targets. A legal drop only stages a target/cost preview; explicit confirmation revalidates and submits once. Invalid drop/cancellation leaves gameplay unchanged. Click/keyboard alternatives also require targeting and confirmation.
3. Basic-card receipts separate action-window ownership from card/environment source. No C casting pose, C-to-target trails, C attribution or C-derived modifiers. Ordinary attacks retain unit provenance.
4. Rain produces three sky-to-ground fishbone waves, affecting allies and enemies according to actual receipts; other basic effects appear at chosen targets. Preserve death, final-hit retention, playback exception recovery and cancellation.
5. Preserve existing engine/receipt/input evidence and adapt the existing UI smoke cases to the amended controls; run narrow syntax and documentation checks. The user explicitly owns browser interaction/layout testing. Do not attempt to bypass the prior local-file browser policy denial or add extensive review rounds.

## Report and learning evidence

HTML 0.4 implementation is ready for user playtesting. App/FX/CSS add attack-first selection/cancellation and the character-detail layout. CardInput separates dragging/preview from confirmation; engine receipts separate action-window owner from card/environment source. Fishbone rain falls onto both sides; cards do not animate a C caster. Native-timer freeze recovery remains intact.

Checks: 47 engine, 26 receipt and 17 input cases passed during this task; the amended app/FX file passed its 36 cases once, including long-press smoke. These are Node state/event tests, not browser layout evidence. App, FX, card-input and engine syntax checks passed; AuditDocs passed 210 Markdown files / 105 pairs and scoped diff whitespace checks passed. Portraits are placeholders, levels are unconfigured, and unimplemented PDF abilities remain explicitly labelled. The user will test real browser interaction; no further independent review or learning submission is required for this iteration. No UE changes, staging, commit, merge or push.
