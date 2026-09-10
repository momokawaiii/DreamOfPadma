# Chapter Zero Demo Rules

- Document ID: RULE-CH0-001
- Version: 1.0
- Status: Accepted scope and behavior; content values and implementation pending
- Chinese companion: [ChapterZero.zh-CN.md](ChapterZero.zh-CN.md)
- Source: user answers Q1–Q38 and subsequent deferral in [Skills Testing](codex://threads/01a06a4e-a758-72a2-abf6-985bd08d6ae1), 2026-09-10
- Provenance and unanswered proposals: [TASK-053](../Production/Tasks/TASK-053-Grill-Technical-Docs.md)

## Delivery and player path

The next delivery is an installable, offline Win64 Demo at release quality. It is not the full MVP and does not include actual Steam submission/integration, cloud saves, achievements or online services. Both Development and Shipping must be tested. The story editor is deferred; playable story/tutorial runtime remains necessary.

`Start → main menu → opening story → Chapter Zero map → movement → calendar/resources → ABC cards → synthesis → node event/dialogue → Encounter → ACT → occupy royal court → Demo ending`

Chapter Zero is the tutorial. The core steps require real player operation and state changes. Encounter and the single ACT node are distinct and experienced in sequence. No FPS node. Other systems are introduced when unlocked, not all inserted into this tutorial.

The final Demo path uses approved production values; the user will provide final card documentation. Existing labelled temporary tables remain development fixtures and do not satisfy this acceptance. Missing values are not permission to invent defaults. How alternative victory conditions interact with the royal-court ending, exact node assignments and detailed rewards still need content decisions.

## Map and story

- The strategic map should use the same visual language as future chapters, referencing Civilization VI's layout. Fixed Seed, fixed story anchors, explicit edges and locks govern Chapter Zero.
- Algorithms generate terrain; PCG supplies cosmetic decoration/decals. Gameplay data owns nodes, movement and locks. Future procedural freedom must preserve anchors, reachability, deterministic identity and save compatibility.
- Ordinary, camp/furnace and anchor node types are independent of main/side/NPC/random story tracks.
- Ordinary nodes may repeat an event where its content policy allows. Random story availability varies by calendar stage.
- Q33 settles the earlier Q30 ambiguity: at the first map load for a stage, determine node assignments and persist them. Reloading the same save does not redraw; a new stage reevaluates pools.
- A checkpoint locks its event/branch on first eligible entry. Later changes do not rewrite past decisions.
- Some special nodes affect the main story. Their exact new effect/dispatch mechanism was not settled in Q39; do not infer arbitrary cross-graph jumps.

## Skip and replay

Tutorial status distinguishes NotStarted, InProgress, Completed and Skipped. Skipping the whole tutorial marks Skipped, initializes its configured post-tutorial rewards and permits continuation. It must not masquerade as actual completion.

Settings can start a new tutorial run. Deliberate replay may earn rewards again; reloading or crash recovery within the same run cannot duplicate a grant. Skipping a presentation cue only fast-forwards presentation, not tutorial settlement. Exact rewards and inventory/profile behavior across replay remain pending.

## Technical mapping and remaining scope

[DataDrivenArchitecture](../Architecture/DataDrivenArchitecture.md) owns definitions; [RuntimeFlow](../Architecture/RuntimeFlow.md) owns execution; [SaveSchema](../Architecture/SaveSchema.md) owns persistence. These rules do not select final field layouts, registry type encoding or a durable transaction algorithm.

Private research assets may remain temporary references under the user's stated scope; assets in a public release candidate must be self-owned or appropriately licensed and the reverse-engineered placeholders replaced. This is the user's asset boundary, not an asset-import instruction.

The prior Rules/Time, Combat, Synthesis and WorldState remain in force outside the explicit scope refinements above. Q39–Q45 were not answered; postponing the editor does not approve their detailed implementation proposals.
