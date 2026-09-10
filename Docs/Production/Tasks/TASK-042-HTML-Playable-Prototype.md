# TASK-042 HTML Playable Rule Prototype

- Chinese companion: TASK-042-HTML-Playable-Prototype.zh-CN.md
- Document ID: TASK-042
- Version: 0.2
- Status: Review
- Primary: root, integration and UI owner; current Local branch. One bounded worker owns data.js, engine.js and engine.test.cjs; a read-only architect supplies UE guidance.
- Authorization: 2026-09-09 user correction: develop a playable HTML prototype to align rules before further UE production. Temporary, editable numerical values are authorized for HTML only.

## Goal

Deliver an offline browser game with a reproducible deployment, daily economy, synthesis, explicit node movement, Encounter/ACT, rollback and victory loop. Preserve existing UE candidates and suspend further UE work. Source card values retain page attribution; partial card implementation and prototype proposals must be visible.

## Scope and ownership

Allowed: `Prototypes/HTML/`; this task pair; `Docs/Production/MVPDecisionRegister` pair; `Docs/Rules/Synthesis`, `Time` and `Combat` pairs; `Docs/00_INDEX`, `ProjectState`, `Changelog` pairs; a link/coverage correction in `Docs/Production/CurrentBuildTestPlan` pair. Root owns integration and documentation; preserve pre-existing edits in shared documents. No Source, Content, Config, generated UE folders, Git staging or integration changes.

## Accepted decisions and gates

- Click inspects; explicit Move Here moves cards. A and attached B move atomically after checking every member. Enemy node contents are public.
- All participating A/B cards enter discard on synthesis success or failure. Failure grants no reward. A tendencies accumulate into attached B once when entering Morning each day. Success creates C directly at the A/B pair's current node, never in hand and without a second deployment fee (user correction).
- Encounter and ACT are separate execution modes. ACT roster and weapons are separate from deployed ABC cards; basic skills share identities and slots with mode-specific effects.
- Battle freezes the calendar, commits success, and restores the complete pre-entry run on defeat/exit.
- Missing HTML numbers may be temporary; formal UE numbers and unresolved gameplay meanings remain open. The user accepted a separate basic-skill discard pile, recycling on exhaustion into shared slots, and manual Encounter/ACT prototype entries. True damage ignores C-card resistance; physical damage uses it.

## Current HTML amendment

Double-clicking home opens a full preparation screen with Node Details, Protagonist Display and Other Modes. The home protagonist is a separate identity from recruited ACT/FPS followers. Outfit, weapon type and compatible pose previews are editable; FPS remains selection preview only. The existing local-node scene inspection remains separate. Forge and watchman operations open authored dialogue and choices; grants occur only on completion, once. Cancellation grants nothing. New illustration/dialogue content is expressly placeholder prototype content, not accepted plot or final card abilities. Supply a bilingual play guide and a proposed UE CommonUI/presentation/dialogue architecture without C++ or engine dependency changes.

## Acceptance and checks

- An actual browser launch point and a short default play path, not only a static screen sketch.
- Editable temporary profile and explicit source/implementation coverage.
- Pure command/state layer, stable IDs, seeded outcomes and safe-boundary save/continue.
- Node tests for calendar boundaries, synthesis/material lifecycle, movement atomicity, mode separation, timeline/card allowance, ACT bullet time and complete rollback.
- Browser checks of real deployment/synthesis/movement and both battle modes; no claim of UE/GAS execution.
- Bilingual documentation audit, strict project validation and scoped whitespace review.

## Evidence

The initial 34 engine tests passed. Browser automation rejected the local file URL; no alternate browser access was attempted. The user confirmed that index.html opens and enters the map. The current home/dialogue/amended synthesis evidence is recorded below. No learning assessment is required.

## Completion report — v0.2 candidate

- Implemented: offline game plus home double-click/three-tab presentation, independent protagonist outfit/weapon/pose previews and ACT/FPS selections, choice-driven forge/watchman dialogue, and direct world C synthesis with no extra deployment charge. [Play guide and UE proposal](../../../Prototypes/HTML/README.md).
- Code paths: `Prototypes/HTML/index.html`, `data.js`, `engine.js`, `engine.test.cjs`, `app.js`, `home-ui.js`, `style.css`, `home.css`, and the README pair. Root owns UI/docs; html_state_amendment owned the three state/data/test files. Existing unrelated working-tree content is preserved.
- Documentation pairs: this task, MVPDecisionRegister, Synthesis, Time, Combat, 00_INDEX, ProjectState, Changelog, CurrentBuildTestPlan, and Prototypes/HTML/README. Shared state explicitly records HTML-first and no additional UE development.
- State verification: 47/47 engine tests, including 13 new synthesis/dialogue/identity/migration cases. Independent reviewer reran the suite, checked 42 pure template combinations and JS syntax; no P0–P2 findings.
- Review: independent html_ui_review returned **Conditional pass**. Browser automation was denied by local file URL policy, so native double-click, current layout and browser gameplay remain manual acceptance conditions. The user's initial map-entry confirmation is recorded without extending it to the new screens. Pure template execution is not browser evidence.
- Template evidence: 6 home renders (3 tabs × 2 mode selections), 30 protagonist SVG renders (2 characters × 3 outfits × 5 compatible weapon/pose combinations), and 6 dialogue-line renders. These checked template execution/titles/undefined values/SVG structure, not layout.
- Repository validation: AuditDocs passed (206 Markdown files / 103 pairs); ValidateProject -Strict passed; scoped tracked diff and untracked HTML/task whitespace checks passed. JavaScript syntax checks passed. No new runtime build was required because UE files were not changed.
- Remaining: user visual/play acceptance, final art/story, and the explicitly listed unimplemented PDF systems. UE CommonUI/stage/dialogue recommendations are not an implemented or accepted ADR. No new C++, UE map/asset, engine dependency, commit, merge or push.
