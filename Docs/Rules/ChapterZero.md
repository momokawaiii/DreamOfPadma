# Chapter Zero Demo Rules

- Document ID: RULE-CH0-001
- Version: 1.1
- Status: Accepted scope and behavior; content values and implementation pending
- Chinese companion: [ChapterZero.zh-CN.md](ChapterZero.zh-CN.md)
- Source: user answers Q1–Q38 and Q46–Q90, including later scope refinements in [Skills Testing](codex://threads/01a06a4e-a758-72a2-abf6-985bd08d6ae1), 2026-09-10
- Provenance and unanswered proposals: [TASK-053](../Production/ProjectCleanup.md)

## Delivery and player path

The next delivery is an installable, offline Win64 Demo at release quality. It is not the full MVP and does not include actual Steam submission/integration, cloud saves, achievements or online services. Both Development and Shipping must be tested. The story editor is deferred; playable story/tutorial runtime remains necessary.

`Start → main menu → opening story → Chapter Zero map → movement → calendar/resources → ABC cards → synthesis → node event/dialogue → Encounter → ACT → occupy royal court → Demo ending`

Chapter Zero is the tutorial. The core steps require real player operation and state changes. Encounter and the single ACT node are distinct and experienced in sequence. No FPS node. Other systems are introduced when unlocked, not all inserted into this tutorial.

The final Demo path uses approved production values; the user will provide final card documentation. Existing labelled temporary tables remain development fixtures and do not satisfy this acceptance. Missing values are not permission to invent defaults. How alternative victory conditions interact with the royal-court ending, exact node assignments and detailed rewards still need content decisions.

## Map and story

- The final strategic map is fixed-Seed, editor-baked **3D terrain plus cosmetic PCG**, referencing Civilization VI's visual composition. Keep fixed story anchors and explicit edges/locks. The current painting is a legacy validation fixture, not a second final-map option.
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

## Follow-up delivery constraints

- ACT P0 is one **new Padma player character** with keyboard/mouse third-person movement/camera, attacks/combos/skills, targeting/hits/interrupts, and at least one representative projectile, summon and execution. A fixed repeatable scene with enemy configuration/waves must enter from strategy and support victory, defeat, exit and correct return. Other Combat content is P1/P2. The first ACT learning unit is only model/retarget/operable 3C; it does not reduce final P0.
- Page hierarchy, layout, visuals and input gestures may be rebuilt from the user's forthcoming full UI interaction specification. Preserve domain commands, results and state meaning; extend contracts explicitly for new behavior. Existing page shapes and hotkeys are not a permanent compatibility requirement. Gameplay costs, permissions and clocks still follow rules unless separately changed.
- Establish stable IDs, typed data and UI contracts first. Hand-author Chapter Zero through existing DataTable/DataAsset workflows; the future story editor writes the same runtime contracts. Final card documentation gates final values and acceptance, not unrelated framework work.
- Use one local authority. No multiplayer implementation or certification is required; a multiplayer entry is hidden or clearly unavailable. Keep serialization/authority boundaries without implementing replication, prediction, RPCs or sessions.
- Functional development may use production Chinese text and non-voice presentation with no audio or audio placeholders. Retain Cue/audio bindings. **Release-candidate acceptance requires production audio; voice acting and full English localization are not required.**
- Record a generous baseline on the development machine (hardware/resolution, frame floor, load time, memory and PCG counts). Exact budgets remain to be measured/recorded; no 1080p/60 FPS gate is approved. Once baselines are agreed, exceeding one pauses content additions for diagnosis under [BuildMatrix](../Production/BuildMatrix.md). Investigate severe leaks, crashes, softlocks and runaway generation before adding content.
- Replace temporary art manually through normal asset properties/references; no dedicated replacement tool. Reject a public/Steam-target package if `civili` or known prohibited-source assets remain in its references, including renamed/transitive/cooked dependencies. Private learning packages have a separate scope; a Shipping configuration alone does not establish public clearance.

[DemoDeliveryPlan](../Production/DemoDeliveryPlan.md) owns the work/learning order; [ACTDevelopmentContract](../Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md) and [ACTMigrationMatrix](../Content/ACTMigrationMatrix.md) detail the ACT target. [BuildMatrix](../Production/BuildMatrix.md) owns evidence and release gates.
