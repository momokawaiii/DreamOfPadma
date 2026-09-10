# MVP Decision Register

- Chinese companion: MVPDecisionRegister.zh-CN.md
- Document ID: MVP-DECISIONS-001
- Version: 0.4
- Status: Active
- Owner: Integration Coordinator
- Task: [TASK-011](Tasks/TASK-011-Full-MVP-Task-Cluster.md)
- Updated: 2026-09-10

## Use

This register records decisions, requested answers and dependent tasks. It is not a default-value list that answers for the user. Record each answer in the applicable canonical English source, synchronize Chinese and affected tasks, then update this register. Technical owners may propose exact contracts; choices that change gameplay meaning need the user.

An accepted policy does not settle production values, asset paths or schemas. D31 originally authorized editable temporary HTML numbers; the 2026-09-09 TASK-046 authorization extends them to explicitly labelled UE Demo tables. Missing/conflicting final production values still need individual confirmation. An open decision gates dependent implementation, not unrelated ready work.

## Current Demo scope override — 2026-09-10

The grill decisions are owned by [ChapterZero](../Rules/ChapterZero.md) and [ADR-0010](../Decisions/ADR-0010-Offline-Demo-Content-and-Map.md). The next delivery is an installable offline Demo, with both Development and Shipping tested. D31/TASK-046 temporary values remain prototype permissions; the final golden path requires the user's production card/rule values.

Fixed chapter maps are editor-generated/baked with versioned MapKey; phase story assignments and tutorial run/reward identity are target contracts. The Slate story editor is deferred. These scoped decisions take precedence over old blanket backlog/packaging statements without marking every full-MVP D-item solved. Q39–Q45 remain unanswered proposals.

## Decision matrix

| ID | Status | Decision owner | Tasks affected before implementation | Accepted meaning / remaining answer |
|---|---|---|---|---|
| D01 | Accepted | User | [013](Tasks/TASK-013-GAS-Infrastructure.md), [017](Tasks/TASK-017-Encounter-GAS-Playable-Loop.md), [020](Tasks/TASK-020-Card-Catalog-and-Lifecycle.md), [030](Tasks/TASK-030-ACT-GAS-Playable-Loop.md), [031](Tasks/TASK-031-ACT-Repository-and-Bullet-Time.md), [035](Tasks/TASK-035-ACT-Character-Cards-and-Roster.md) | Separate character-card collections; turn-based cards deploy on the map, ACT cards form the battle-settings roster. Basic non-A skill cards retain shared identity/slots and separately configured mode effects. |
| D02 | Accepted policy; asset inputs pending | User / Character Presentation | [014](Tasks/TASK-014-Character-Presentation.md), [033](Tasks/TASK-033-First-Era-Content-Pack.md), [035](Tasks/TASK-035-ACT-Character-Cards-and-Roster.md) | The user will import placeholder assets after work starts. Record exact paths, skeleton/animation compatibility and project wrappers before using them. Final animation detail and rendering polish are later work. |
| D03 | Accepted source policy; missing/conflicting values open | User | [015](Tasks/TASK-015-Combat-Calculation-Contracts.md), [017](Tasks/TASK-017-Encounter-GAS-Playable-Loop.md), [019](Tasks/TASK-019-Resource-and-Global-Ledgers.md), [020](Tasks/TASK-020-Card-Catalog-and-Lifecycle.md), [021](Tasks/TASK-021-ABC-Sandbox-Operations.md), [022](Tasks/TASK-022-Synthesis-Resolution.md), [026](Tasks/TASK-026-Ruler-World-AI.md), [030](Tasks/TASK-030-ACT-GAS-Playable-Loop.md), [032](Tasks/TASK-032-Run-Outcomes-and-Restart.md), [033](Tasks/TASK-033-First-Era-Content-Pack.md), [035](Tasks/TASK-035-ACT-Character-Cards-and-Roster.md) | 2026-09-09: explicit unambiguous mycard.pdf values may be used unless later accepted rules conflict; record page/field/unit/mode. Missing, ambiguous or conflicting values still require confirmation; do not invent ACT, economy or movement values. |
| D04 | Open technical contract | Architect; user for gameplay implications | [012](Tasks/TASK-012-Run-State-and-Contracts.md), [013](Tasks/TASK-013-GAS-Infrastructure.md), [028](Tasks/TASK-028-Battle-Transactions.md), [029](Tasks/TASK-029-Safe-Boundary-Save-Load.md), [030](Tasks/TASK-030-ACT-GAS-Playable-Loop.md) | Record stable type domains, participant/version rules, definition loading, ASC owner/avatar/grant/remove/rebinding, native dependencies and local-authority baseline. Keep mode runtime handles out of Core/save IDs. |
| D05 | Open | User / Combat Planner | [016](Tasks/TASK-016-Encounter-Timeline.md), [017](Tasks/TASK-017-Encounter-GAS-Playable-Loop.md) | Encounter initiative/action-bar formula, ties, priority, reaction/interrupt/extra-action/non-turn windows, end-turn processing, window card allowance and logical status/cooldown clocks. |
| D06 | Open | User / Combat and Numerical Planners | [015](Tasks/TASK-015-Combat-Calculation-Contracts.md), [016](Tasks/TASK-016-Encounter-Timeline.md), [017](Tasks/TASK-017-Encounter-GAS-Playable-Loop.md), [030](Tasks/TASK-030-ACT-GAS-Playable-Loop.md) | Selected damage/status subset, attribute pools and per-mode values, stacking/consumption/death ordering, modifier precedence and any unresolved era multiplier meaning. Do not equate Shape and Returning Ruin. |
| D07 | Open | User / System Planner | [018](Tasks/TASK-018-Calendar-and-Phase-Scheduler.md) | Zero/one-based calendar convention and the exact scheduled-effect ordering needed by this slice. Fixed Dawn/Morning/Dusk, seven days and seven Eras are already accepted. |
| D08 | Open | User / System and Numerical Planners | [019](Tasks/TASK-019-Resource-and-Global-Ledgers.md), [021](Tasks/TASK-021-ABC-Sandbox-Operations.md), [022](Tasks/TASK-022-Synthesis-Resolution.md) | Initial/cap/debt values and permissions, gains, operation payment timing, loop costs/risks and compensation parameters. Confirm each field/value rather than adopting prototype examples. Entropy thresholds/calamity triggers, probabilities, causing-side attribution and consequence magnitudes are individually confirmed; TASK-019 owns the value contract. |
| D09 | Open | User / System Planner | [020](Tasks/TASK-020-Card-Catalog-and-Lifecycle.md), [031](Tasks/TASK-031-ACT-Repository-and-Bullet-Time.md) | Hand/deck/lifecycle subset, limits/depletion, discard/return/burn/cooldown and random probabilities; per-mode effect cost/lifecycle ownership and explicit clocks. |
| D10 | Open | User / System Planner | [021](Tasks/TASK-021-ABC-Sandbox-Operations.md) | A placement, B attachment/direct-use/field timing and costs, C deployment and the selected sandbox ability. Attached movement follows accepted D29. ABC ability legality in local battle remains undecided. |
| D11 | Open | User / System and Numerical Planners | [022](Tasks/TASK-022-Synthesis-Resolution.md), [033](Tasks/TASK-033-First-Era-Content-Pack.md) | Candidate filtering, no-shared-tag case, Softmax units/normalization, formula coefficients/clamps, B-field inputs, whether Sigma remains and the explicit non-reward consequence. Failure never grants a reward. |
| D12 | Open | User / Level and System Planners | [024](Tasks/TASK-024-Sandbox-Graph-and-Movement.md), [025](Tasks/TASK-025-Node-Operations-and-Story.md), [033](Tasks/TASK-033-First-Era-Content-Pack.md) | First node layout/count/IDs, core-node interpretation, movement/category costs, hidden-path and terrain conditions, node operations and story consequence meanings. |
| D13 | Open | User / Combat and Numerical Planners | [026](Tasks/TASK-026-Ruler-World-AI.md), [033](Tasks/TASK-033-First-Era-Content-Pack.md) | Patrol/detection/stealth conditions, threat event/daily values, tie priorities and reinforcement/counterattack triggers and penalties. |
| D14 | Open | User / Combat and Level Planners | [027](Tasks/TASK-027-Battle-Routing-and-Travel.md) | How both sides, terrain and card effects constrain routes; which legal route selects Encounter/ACT and its story consequence; missing/ineligible destination behavior. |
| D15 | Open technical and state policy | Architect / System Planner; user for state meaning | [028](Tasks/TASK-028-Battle-Transactions.md), [029](Tasks/TASK-029-Safe-Boundary-Save-Load.md) | Complete participant set, version/compatibility/failure recovery, safe save boundaries and state commit/restore rules; explicitly include ACT roster/loadout and any battle-mutated inventory/weapon state. |
| D16 | Open | User / Combat Planner; Architect for schema | [030](Tasks/TASK-030-ACT-GAS-Playable-Loop.md) | First ACT action, target/hit rules, phases/commit/cancel/interrupt/buffer and clock semantics; selected character/weapon features only. A combo or Motion Warping is required only if that chosen action needs it. |
| D17 | Open | User / Combat and UI Planners | [031](Tasks/TASK-031-ACT-Repository-and-Bullet-Time.md) | Tab dismissal/re-entry, selection and closing behavior, page count/boundaries, duration and effect/cooldown clocks. Tab, blur, 1/10 world time, input rejection, inertia and visible slots 1–5 remain fixed. |
| D18 | Open | User / System and Numerical Planners | [032](Tasks/TASK-032-Run-Outcomes-and-Restart.md), [033](Tasks/TASK-033-First-Era-Content-Pack.md) | Explicit campaign defeat, simultaneous result priority and evaluation timing, victory boundary/event magnitudes and restart defaults. Local defeat/exit still rolls back; day seven does not imply a forced loss. |
| D19 | Open | User / Level and Numerical Planners | [025](Tasks/TASK-025-Node-Operations-and-Story.md), [033](Tasks/TASK-033-First-Era-Content-Pack.md) | Named first-Era cards/characters/weapons/boss, node/story content, exact user-approved numerical profiles and acceptance routes; each unspecified value is confirmed under D03. |
| D20 | Partially accepted; details open | User / System and ACT Planners | [027](Tasks/TASK-027-Battle-Routing-and-Travel.md), [030](Tasks/TASK-030-ACT-GAS-Playable-Loop.md), [035](Tasks/TASK-035-ACT-Character-Cards-and-Roster.md) | Accepted: battle settings selects ACT characters/weapons; terrain may disable a character-card trait. Still confirm roster capacity, possession/switching, availability/costs, preset versus run-save scope, any trigger-unit linkage beyond terrain, restriction target/timing/feedback and result projection. A trait restriction does not automatically imply the whole character is forbidden. |
| D21 | Accepted direction; exact layout open | User / World / Architect | [024](Tasks/TASK-024-Sandbox-Graph-and-Movement.md), [025](Tasks/TASK-025-Node-Operations-and-Story.md), [028](Tasks/TASK-028-Battle-Transactions.md), [029](Tasks/TASK-029-Safe-Boundary-Save-Load.md) | Oblique SLG camera; nodes and preset connections over hex-shaped presentation. User accepted static chapter definition asset + .umap visuals + SaveGame run changes (ADR-0005). Exact nodes/edges/costs remain D12/D03. |
| D22 | Partially resolved | User / System and Combat Planners | [015](Tasks/TASK-015-Combat-Calculation-Contracts.md), [018](Tasks/TASK-018-Calendar-and-Phase-Scheduler.md), [020](Tasks/TASK-020-Card-Catalog-and-Lifecycle.md), [022](Tasks/TASK-022-Synthesis-Resolution.md), [033](Tasks/TASK-033-First-Era-Content-Pack.md) | Source reconciliation: contract-card sandbox summon effects (mycard 6–7) excluded from the first version by user; later redesign. Preserve newer no-reward failure, phase and basic battle-card policies; resolve other conflicting PDF definitions before use. D22d is resolved by user: A→F, B→D, C→R; revise conflicting collection labels without inventing numbers/effects. See source extraction D22a–f. |
| D23 | Schema authorized; content open | User / ACT Planner | [030](Tasks/TASK-030-ACT-GAS-Playable-Loop.md), [031](Tasks/TASK-031-ACT-Repository-and-Bullet-Time.md), [033](Tasks/TASK-033-First-Era-Content-Pack.md), [035](Tasks/TASK-035-ACT-Character-Cards-and-Roster.md) | PDFs have no separate ACT character/weapon/action catalog or per-mode basic-skill effects. User has not designed ACT content and authorizes editable definitions with model and ACT skill-table soft references now. Runtime needs supplied content, supported GAS execution bindings and each confirmed number/cost/duration/target. Do not auto-convert C-person cards. |

TASK-034's ALL gate means all decisions its complete acceptance consumes are closed, or explicitly deferred without removing a required MVP feature. Do not defer a required implementation merely to pass final acceptance.

## Recommended question batches

1. Before TASK-012/013: D04 technical contracts; before TASK-014 request the paths of user-imported placeholder assets.
2. Before the first Encounter: D05/D06; list the specific formulas, clocks and numerical fields the user needs to choose.
3. Before calendar/resources/cards: D07/D08/D09; avoid asking for the entire system encyclopedia at once.
4. Before sandbox/synthesis/world AI: D10–D14 and relevant D19 content.
5. Before ACT roster/combat: D20/D16/D17; distinguish character cards from basic skill cards in every question.
6. Before persistence/outcomes/content acceptance: D15/D18/D19, confirming full state coverage and acceptance routes.

Additional source/map decisions are documented in [MVP source extraction](MVPSourceExtraction.md) and [ADR-0005](../Decisions/ADR-0005-World-Map-Storage.md). D21 settles topology direction and storage layers, not movement cost; D22 does not authorize rewriting accepted rules.

## Answer record

For each answer record: decision ID, user meaning, date, affected fields and exact values/units, scope (test profile or production rule), canonical English path, Chinese companion path, affected TASKs and remaining questions. Empty is not zero, unavailable is not free, and unanswered is not consent.

## Map and preparation decisions

- D24 — Accepted (User, 2026-09-09; 024/027/037): click only inspects all node information, with no movement/payment/battle. A separate right-side Move Here button in the local view submits movement of selected ABC/other card instances. Scene availability remains authored content.
- D25 — Accepted full disclosure in first MVP (User, 2026-09-09; 024/026/037): expose all enemy node content, including identities, cards, counts, attachments and state. Hidden-intelligence strategy is future work; ruler AI detection rules are unchanged.
- D26 — Accepted (User, 2026-09-09; 033/034/038): FPS is outside the first playable MVP; ACT is the first playable action mode. Encounter retains its existing MVP requirement. FPS configuration shows future capability only, never playable deployment.
- D27 — Open (User; 035/038/039): where/when preparation opens and edits, ongoing time, unsaved edits and global preset/run ownership. Browsing is not an equip/grant/advance command; this page does not inherit ACT Tab bullet time.
- D28 — Relationship-bonus capability requested, concrete rule open (User/System/Numerical; 025/033/034/039): named NPC/event/bonus, target including a possible B-card category, threshold/magnitude, grant/removal/stacking/timing and global/run persistence. Affection is not synthesis compatibility; bonuses are not all ACT effects.

D24–D26 were answered on 2026-09-09 and the screen design is synchronized. TASK-037/038/039 still require their other predecessors.

- D29 — Mobility policy accepted; per-card configuration open (User, 2026-09-09; 012/020/021/024/035/040): each card catalog has Movable/Immovable/Conditional and explicit unconfigured state. A and attached B move as a group with every card validated. Failure causes no partial movement/payment; check ownership, source, edges and authored costs. Conditions, per-card policies and costs are not inferred.
- D30 — Model binding authorized (User/Architect, 2026-09-09; 014/037/040): model-required content has Data Asset soft-reference sources. Scene slots bind instance and domain-qualified definition IDs; Game resolves the selected source and its models/skills. ACT retains its existing sole source. Static preview is not run occupancy or save state.

## HTML-first continuation (2026-09-09)

These updates supersede earlier unanswered descriptions in the matrix for their stated scope; they do not automatically authorize production UE implementation.

- D31 — Accepted (User; 042, amended 046): the user accepted the HTML 0.4 subset and now authorizes native UE implementation. On 2026-09-09 the user explicitly permits its temporary numbers in clearly labelled UE Demo configuration. This supersedes the earlier HTML-only delivery boundary. D03/D08 individual confirmation remains for final production values; see ADR-0006.
- D32 — Accepted (User; 021/022/042; normative Synthesis and Time): discard participating A/B after either synthesis outcome; accumulate A tendencies into attached B once at daily Morning entry; success creates C directly at the materials' current node, with no hand entry or second deployment/payment.
- D33 — Accepted (User; 015/020/027/031/042; Combat): shared basic skills enter a separate discard pile, recycle on exhaustion and preserve shared slots; HTML may manually choose Encounter/ACT. C resistance affects basic physical damage, not true damage.
- D34 — Direction accepted (User; 037/038/042): double-click home enters Node Details / Protagonist Display / Other Modes preparation. Single-click inspection and the original local scene preview remain separate. Resident protagonist, ACT followers and FPS followers have separate catalogs. Preview outfits, weapon types and compatible poses; FPS remains selection-only, not playable combat.
- D35 — Entry accepted, final story open (User; 025/039/042): forge/watchman clicks enter dialogue. This version uses original placeholder text/choices, final choice commits existing one-time effects, cancellation grants nothing. No affection or new reward policy is inferred.
- D36 — UE recommendation, not implementation/accepted ADR: CommonUI screen stack with a reusable 3D display stage; story state/commands separate from UI/Sequencer. See the [HTML guide](../../Prototypes/HTML/README.md). Permanent preset ownership, actual assets and final story remain future decisions.
