# TASK-043 Encounter Visual Feedback

- Chinese companion: TASK-043-Encounter-Visual-Feedback.zh-CN.md
- Document ID: TASK-043
- Version: 0.2
- Status: Review
- Primary: root; current Local checkout
- Authorization: user requests visible Encounter turns and source-to-target combat animation in the HTML prototype.

## Goal and decisions

Make each Encounter action readable: current actor, targets, impact, actual health/shield/block changes, and final defeat/victory before returning to the map. Preserve the next-ready timeline and every existing numerical rule. SVG/CSS presentation keeps the offline launch path without adding a rendering dependency. ACT and UE production are outside this task.

Include the directly related existing dead-actor stall: when Rain defeats its Encounter caster but both sides still have survivors, use the existing alive-unit next-ready selection. Preserve all damage values, defeat precedence, ACT behavior and the retained action of a surviving caster.

## Ownership and allowed paths

- Root: Prototypes/HTML/app.js, index.html, encounter-fx.js, encounter-fx.css, encounter-fx.test.cjs, README.md and README.zh-CN.md; this task pair; 00_INDEX, ProjectState and Changelog pairs.
- Bounded worker: Prototypes/HTML/engine.js and encounter-events.test.cjs only. Emit detached transient presentation receipts; preserve existing engine tests and outcomes.
- Independent review is read-only. Preserve all existing unrelated local edits. No C++, UE assets, configuration, staging, integration or remote operations.

## Acceptance and verification

1. Mark the current actor with text and a visible unit highlight; distinguish selected targets and upcoming timeline order.
2. Show attack travel, impact and applied HP loss; show absorption, block, healing, recoil and guard with suitable text and effects. Multi-target actions and rain waves remain intelligible.
3. Lock commands and enemy automation during playback. Cancel stale timers on exit/reset and retain the last hit before settlement. Failed commands show no fake hit.
4. Effects are derived from typed command receipts, never localized log parsing or saved animation state. ACT and exact battle rollback retain their behavior.
5. Run existing engine tests, new receipt/playback tests, JavaScript syntax checks, AuditDocs and ValidateProject -Strict. Independent review inspects actual files. Browser visual acceptance remains a manual check if local-file browser policy prevents automation; do not circumvent that boundary.

## Learning evidence and recovery

Agent evidence: demonstrate the separation between committed rule outcomes and a cancellable read-only presentation sequence. User exercise is optional: identify the next actor, follow a two-target attack, observe a blocked hit, then exit and verify rollback. Restore an older copied HTML folder only at the user's direction; do not discard the shared checkout.

## Report

User-reported follow-up: HTML 0.3 freezes at action windup with disabled controls. Root owns the existing UI/test/doc paths for the repair; no engine changes. Reproduce using the actual default Playback constructor and receiver-sensitive browser timer doubles, bind native timers to the global host, and release presentation on scheduling/painting exceptions without replaying a committed command. The previous 85 tests bypassed the default timer adapter and are insufficient evidence for this failure.

## 0.3 implementation and historical evidence

Implemented: typed transient receipts in engine.js; cancellable playback, predicted timeline, unit silhouettes/highlights, SVG trails and CSS impacts in encounter-fx.js/css; guarded app input/enemy timers and retained final settlement. HTML header is 0.3. No rendering library or network dependency was added. All changes stay in the allowed paths.

Evidence: 85 Node tests pass (47 existing engine, 20 receipt, 18 playback/app/paint tests). The latter use fake clocks and document doubles, including actual app input wiring and painter calls; they are not browser-render evidence. Tests cover source/target identity, clamped HP changes, shield/block/recoil, multi-target/rain, exact rollback, final defeat/victory, cancellation, rejected enemy-windup input, and persisted page restoration.

Independent review found and verified fixes for one P1 (rejected input invalidated the enemy timer), two P2 edges (persisted page restoration; earlier rain death presentation), and reduced-motion particle visibility. The additional dead-caster stall is repaired using the existing nextActor selection after a nonterminal skill; a surviving caster retains the action, and ended battles never advance. Receipt and actual-app harness regressions cover the handoff to a living ally. AuditDocs passed (208 Markdown files / 104 pairs); ValidateProject -Strict, JS syntax and scoped whitespace checks passed. Independent encounter_visual_review reran all 85 tests, verified the dead-caster repair and returned Conditional pass with no remaining P0–P2 findings. TASK remains Review. Browser visuals, resizing, native animation and input feel remain manual acceptance; the prior local-file policy denial was respected. See the README's six Encounter checks. No UE build/PIE, commit, merge or push occurred.


## 0.3.1 repair evidence

- Reproduction: loading the original default Playback with receiver-sensitive timers throws Illegal invocation after windup, leaving busy true and no future callbacks. This matches the screenshot's stage. Earlier injected-clock tests concealed this failure path.
- Fix: default scheduling/cancellation calls through the global host; scheduling, painting and completion exceptions clean up presentation. The app restores already committed state, shows a notice and logs the error, without duplicating commands, costs, skill discard or rollback. Engine code is unchanged.
- Tests: 92 pass (47 engine + 20 receipts + 25 playback/input/paint). Seven new tests cover default host calls/cancellation, phase exceptions, partial scheduling failure, completion failure, committed skill/enemy/final-settlement recovery, and the screenshot's Vitruvian/Dancer combination. Existing app tests now load the shipped default adapter; document doubles remain explicitly distinct from browser-render acceptance.
- Changed files: encounter-fx.js, app.js, encounter-fx.test.cjs, index.html; README, this TASK, ProjectState and Changelog language pairs. Page identifies HTML 0.3.1. No C++, UE, game-rule or save changes, commit or push.
- Independent encounter_freeze_review reproduced the old failure, verified the fix and returned Conditional pass with no remaining P0–P2 findings. Reviewer and root both passed 92 tests, JS syntax, AuditDocs (208 files / 104 pairs) and ValidateProject -Strict; root also passed scoped whitespace checks. User acceptance after updating the page remains pending; status stays Review.
