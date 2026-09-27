# Demo Delivery, Learning and Asset Plan

- Document ID: DEMO-DELIVERY-001
- Version: 1.0
- Status: Planning against current code; no new runtime implementation certified
- Chinese companion: [DemoDeliveryPlan.zh-CN.md](DemoDeliveryPlan.zh-CN.md)
- Authority: [ChapterZero](../Rules/ChapterZero.md), [ACT contract](../Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md)
- Source: grill Q46–Q90, reconciled in [TASK-053](ProjectCleanup.md)

## What is already reusable

Keep the current native calendar/resources/ABC/synthesis rules, typed catalog, frozen map/checkpoint values, separate Encounter/ACT GAS entry, battle transaction/save foundations and UI command boundary. Reuse requires the relevant final-content and regression checks; it does not certify all PDF rules or a release-ready Demo.

Current ACT's `UPadmaACTAbility` commits, calls ResolvePendingAction and ends. Existing authoring assets bind model/animation/skill IDs. They are useful seams, but are not a Sequence Runtime, new-character third-person 3C or a complete combo/projectile/summon/execution system. Current sample rendering is not final art. The painted tutorial is now a legacy rule-validation fixture; Chapter Zero targets a baked 3D map.

The user's prior Blueprint, DataAsset/DataTable, soft-reference, CommonUI/UMG and GAS study is a starting point. Do not require repeating all introductory courses.

## Work packages and learning units

These unit IDs are a proposed decomposition, not newly opened TASKs or authorization to implement them all. Each future learning task needs **one production result + one bounded learning unit + reproducible evidence**. User practice and understanding are separate from Agent-written code.

| Unit / dependency | Production result to implement | Learning keywords / evidence |
|---|---|---|
| U01 — foundation | Stable chapter/card/ability/map identities, definition/state distinction, existing UI command semantics and migration matrix | USTRUCT/UPROPERTY, typed rows, DataAsset, FName versus GameplayTag, versioning; validate a real binding and reject a missing reference |
| U02 — first ACT task | One new model retargeted in Padma with keyboard/mouse third-person movement, camera and controller | Skeletal Mesh, IK Rig/Retargeter, retarget pose/chains, root motion, AnimBP, BlendSpace, CharacterMovement, SpringArm, Enhanced Input; actual control/turn/camera collision evidence |
| U03 — U01/U02 | Native ACT ability lifecycle and data-driven grant/input routing | ASC owner/avatar, AbilitySpec, Commit/Cancel/End, tags, GE/AttributeSet, AbilityTask delegates; interrupt/end/re-entry cleanup |
| U04 — U03 | One combo Sequence with buffered input, legal branches and authoritative hits | Runtime node/phase, Montage Section, Notify events, input expiry, Combo/Hit windows, swept traces, TargetData, GE, Cue; early/late input, duplicate-hit and interruption cases |
| U05 — U04 | A player-triggered projectile ability in the fixed ACT scene | ProjectileMovement, spawn ownership, collision, lifetime, TargetData/GE, visual Cue; hit/miss/exit cleanup |
| U06 — U04 | A player-triggered summon with bounded lifetime and owned behavior | spawn/despawn, owner/faction, AI control, target query, ability grant/removal; death/cancel/battle exit cleanup |
| U07 — U04 | Execution ability with eligibility, aligned attacker/victim presentation and interruption handling | target validation, synchronized Montage/events, Motion Warping where needed, reservation/cancel cleanup; eligible/ineligible/death/exit cases |
| U08 — U01 | Fixed Chapter Zero 3D terrain + protected anchors/roads + cosmetic PCG baked with MapKey | axial/cube hex coordinates, noise/fBm, climate/height masks, graph reachability/locks, PCG attributes/keepouts, instancing, editor bake/cook; determinism and restore |
| U09 — U01; final values needed for settlement | Hand-authored ChapterDefinition/dialogue/tutorial/pools and final calendar/resource/ABC/synthesis content | DataTable/DA, TextId/StringTable, trigger-condition-effect, stage assignments, tutorial progress and receipts; real operations and save/reload behavior |
| U10 — U01; user UI interaction specification | New CommonUI pages, HUD, hand, target selection, settings and tutorial flow | ActivatableWidget/stack, focus, input config, Back/cancel, ViewModel/Intent, loading/disabled/error states, DPI/invalidation; user interaction checks |
| U11 — U05–U10 | Encounter then fixed ACT scene entered from the map; victories, defeats, exit and return; court ending | mode contexts, possession/travel, complete snapshot/commit/rollback, save compatibility; manually operated golden path |
| U12 — alongside U08/U10/U11 | Cohesive terrain/card/character/UI art, Chinese text and non-voice scenes; production audio before RC | composition/typography, material masks, UMG animation, Niagara, Sequencer/Cue; visual/hand-feel acceptance distinct from functional tests |
| U13 — begin diagnostics early, finish after U11/U12 | PIE, Development, Shipping, clean install/offline path; dependency audit and recorded performance baseline | Asset Registry/Reference Viewer, soft/hard/indirect dependencies, cook manifests, AutomationTool, Unreal Insights/stat GPU/memory; evidence per BuildMatrix |

First practical ACT unit: **U02**, not bulk-importing the Combat project. U01 data/UI contracts may be prepared while final card values and the full UI specification are supplied. Prove one hit/sequence path before adding payload families. Package a baseline early; do not wait until the end to discover missing soft-referenced content.

## Additional knowledge keywords by domain

- **C++/data/lifetime:** UObject ownership, GC, weak/soft references, delegates/timers, reflection, modules/Build.cs, Data Validation, catalog resolve, asynchronous load completion/cancellation, value snapshots, schema compatibility, transaction idempotency.
- **ACT/animation:** ASC Owner/Avatar, AbilitySpec/AbilitySet, gameplay events, GameplayEffect specs and contexts, attributes, cost/cooldown ownership, AbilityTask OnDestroy, runtime phase state, input buffer expiry/replacement, hit deduplication, interrupt priority, skeletal sockets, AnimInstance, state machines, BlendSpace, Montage/Section, NotifyState, root motion, IK, Motion Warping and synchronized execution.
- **Map algorithms/PCG:** axial/cube conversion, neighbor lookup, region/biome masks, seed/stream partition, Perlin/simplex/fBm candidate techniques, slope/height/water classification, river/shore masks, explicit graph validation, lock partition checks, Poisson disk or Mitchell best-candidate spacing for decoration, PCG points/attributes/density/filtering/spawners, HISM/ISM, bounds, LOD, baking. Noise shapes fields; spacing distributes props; neither replaces path legality.
- **UI/technical art:** information hierarchy, spacing/grid, typography, color/value contrast, nine-slice brushes, texture atlas, alpha/sRGB/compression, UI Material domain, UV/panning/noise, SDF/masks, reveal/scan/glow, layered parallax, material instances, UMG animation/easing/stagger, CommonUI activation/input routing, Slate hit testing/focus/capture and invalidation. Holographic/parallax cards are targeted material exercises, not a requirement to put a unique shader on every button.
- **VFX/cinematics/audio:** Niagara systems/emitters/modules, sprite/ribbon/mesh renderers, user parameters, flipbooks, event timing, bounds/overdraw; Sequencer bindings/camera cuts/skip cleanup; later SFX/ambience/music, attenuation/mixing and Cue references. AE/Fusion is optional for motion design and pre-rendered sequences; interactive state must still be rebuilt in UE.
- **Save/build/performance:** stable IDs, immutable manifests, content versus schema versions, trusted restore, reward receipts, full battle snapshots, interrupted writes, cook inclusion and packaged reference checks; Unreal Insights, CPU/GPU frame time, load time, memory, instance counts and leak detection. Record a generous development-machine baseline now; values are not fixed to 1080p/60.
- **Deferred:** network replication/RPC, prediction keys/reconciliation, GAS network policy, sessions/server hosting; custom Slate story graph/compiler; complex lock-on, gamepad polish, FPS and large content tooling. Preserve seams now; no claim that networking is automatic later.

Official learning entry points: [IK Rig retargeting](https://dev.epicgames.com/documentation/en-us/unreal-engine/ik-rig-animation-retargeting-in-unreal-engine), [CommonUI input guide](https://dev.epicgames.com/documentation/unreal-engine/commonui-input-technical-guide-for-unreal-engine), [PCG framework](https://dev.epicgames.com/documentation/unreal-engine/procedural-content-generation-framework-in-unreal-engine). These support retargeting, input and generation study; the broader unit ordering above is a project-specific recommendation.

## What to reverse-engineer or study

| Reference | Extract as a specification | Bring into Padma only after checking |
|---|---|---|
| Combat | input → grant/activation → phase/branch → hit/effect → cleanup; combo/PreInput timing, projectile/summon/execution lifecycle and camera behavior | Selected compatible animation/VFX/model data and approved behavior; rebuild in native GAS/C++, never import the plugin class hierarchy as a dependency |
| Civilization VI | camera scale, terrain-height silhouette, biome transitions, shore/river/road overlays, settlement spacing, foliage density, layered UI composition; generation stage ordering | Learning-only research inputs under the user's boundary; production replacements need their own provenance and reference audit |
| Arknights / Persona / similar UI references | a timing sheet of delays/easing/stagger, transitions, masks, focus and readable hierarchy | Your own layout/graphics/materials/animation. Frame observation does not require copying their UI assets or building a game-data extractor |

For Combat, start with `GAS_Xia`, `GA_ComboBase`, `GA_Combo_X/XX/XY`, `ANS_PreInput`, `ANS_Input_Window` and `GA_Execution`. Local asset paths were found, but their live behavior was not replayed in this documentation task. Record exact source chains and defects before choosing a migration; use [ACTMigrationMatrix](../Content/ACTMigrationMatrix.md).

## Asset production and acquisition list

Counts below are practical starter packs, except the explicitly accepted one player character and representative projectile/summon/execution coverage. Expand only for the authored content; no new mandatory card/NPC count is introduced.

| Pack | Find/create | UE deliverable / acceptance |
|---|---|---|
| Player/enemy | One chosen player model, compatible rig/textures, weapon, enemy model usable for hit/execution tests | SkeletalMesh/Skeleton/PhysicsAsset, sockets, AnimBP, retarget setup; check scale, feet, hand grip and root motion |
| Animation | idle/start/run/stop/turn; selected combo/skill; hit/stagger/death; projectile cast, summon cast, paired execution | Retargeted sequences/Montages and semantic timing events; no authoritative damage hidden in NotifyState |
| World kit | grass/soil/rock/sand/snow as needed; hills/cliffs/mountains, water/shore/river, roads/bridges; trees/bushes/rocks | Meshes or terrain material layers, normals/roughness/masks, controlled PCG palette and collision/LOD; 3D relief readable from the strategy camera |
| Seven landmarks | home, fire, story, plain, gate, forge and boss visual sets; landmark-specific NPC/props when needed | Stable NodeId presentation bindings; landmarks remain recognizable after art replacement |
| Cards/HUD | final-card illustration set, portraits/icons, paper/ink/metal surface textures, borders/corners, ornaments, masks and UI typeface | Atlas/brush/style assets, localized text kept separate, shared UI materials; states include hover/pressed/disabled/selected |
| ACT/VFX | hit spark, slash/ribbon, projectile trail/impact, summon reveal/despawn, execution cue, basic selection/telegraph | Niagara/material/flipbook profiles with clear spawn/end ownership and acceptable overdraw |
| Story/motion | opening/ending visual elements, dialogue portraits, camera/action scenes, UI transitions | LevelSequence/Cue and UMG animation; AE/Fusion storyboard or pre-render assets only where they suit fixed playback |
| RC audio | UI confirm/cancel, footsteps, weapon/hit, ability, ambience and music | Production audio/Cue bindings before release candidate; voice acting is not required |

Useful search terms: `stylized strategy environment`, `modular cliffs rocks foliage`, `terrain ground PBR`, `third person rigged character`, `root motion combat animation`, `paired execution animation`, `parchment grunge seamless`, `UI ornament alpha`, `Niagara slash ribbon projectile`. Select one coherent style; retargeting cannot repair incompatible motion intent, missing paired animation or bad skinning.

[Poly Haven](https://polyhaven.com/license) supplies CC0 HDRIs/textures/models, and [ambientCG](https://docs.ambientcg.com/license/) supplies CC0 material/model assets; they are suitable starting sources for surface/lighting studies. [Fab](https://www.fab.com/eula) is a source for character/animation/environment packs; check each listing's license and technical compatibility rather than treating every marketplace item as equivalent. No purchase, download, extraction or import was performed here.

## Manual replacement and release audit

No dedicated asset replacement editor. Use existing Content Browser/Details/soft-reference fields and replace actual uses. Maintain origin, original package, current package and replacement state; renaming an extracted asset does not make it replaced.

Public/Steam-target RC cook/package must fail on `civili` names/paths/packages or known reverse-engineered dependencies. The audit must trace hard/soft/indirect dependencies through Asset Registry, maps, Blueprint defaults, DataTable/DataAsset/catalogs, AnimBP/Montage, Material/Niagara/Sequence and the final cook manifest; a filename grep alone is insufficient. Audit implementation and clean-package proof remain work.

Private learning builds may retain documented research inputs under the user's scope. They must never be described as cleared public release candidates. Production audio is a later RC gate; full performance optimization is deferred, while crashes, softlocks, severe leaks, runaway spawning and loading failures still require correction.

## Inputs and next concrete step

The user supplies the new character/model choice, final card/reward document and complete UI interaction specification. Check the actual reference project/environment directly; use reasonable defaults for reversible key bindings and similar details, without inventing missing rule values.

Start with U01's minimal IDs/contracts and the migration inventory, then U02's new-model retarget/3C task. The first task's 3C-only boundary does not reduce final P0 ACT coverage in Q82/Q89/Q90.
