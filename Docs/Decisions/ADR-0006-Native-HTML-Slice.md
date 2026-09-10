# ADR-0006 Native HTML Slice, Content Shards and Presentation

- Chinese companion: ADR-0006-Native-HTML-Slice.zh-CN.md
- Document ID: ADR-0006
- Version: 0.1
- Status: Accepted implementation decision under user-authorized TASK-046
- Date: 2026-09-09

The user has accepted HTML 0.4 and authorized its native UE implementation, including its temporary numbers in labelled Demo content. This selects the implemented HTML behaviors where older descendants left exact first-slice rules open. The larger PDF scope and FPS gameplay remain deferred.

## Decisions

- Retain logical Core/Game/Gameplay/World/UI folders in the existing module. Add built-in GameplayAbilities/GameplayTags/GameplayTasks, CommonUI/UMG/Slate, Niagara and ProceduralMeshComponent dependencies for executable combat, native screens and hex geometry. External Combat remains read-only reference, with no GASCompanion or Aurora dependency.
- Resolve typed tables into checked value snapshots at the Demo's entry boundary; never load tables every tick. A catalog contains multiple soft table shards per domain. Duplicate IDs across shards fail rather than override by order. Encounter effects and ACT effects have different row types and independent data. Card/ACT-character/ACT-weapon/FPS-gun/display identities are separate. Asset bindings remain typed soft Data Asset references; owned instances/saves carry IDs only.
- The small fixture loads its content synchronously at a visible entry/loading boundary; this is not an open-world streaming architecture. Soft references alone do not guarantee asynchronous loading. Cook coverage includes the playable catalog/map roots; missing/wrong references produce actionable errors.
- Pure Core commands own calendar, card economy, movement, synthesis and immutable saveable state values. A GameInstance subsystem adapts this state to safe SaveGame boundaries. Each operation validates before publication; battle snapshot includes movement payment, RNG, card piles, flags and roster values.
- Gameplay owns transient GAS actors and mode execution. Encounter chooses timeline eligibility before abilities execute. ACT uses real-time input, movement, attack tasks and its separate bindings. Character ASCs die with their battle actors; an independent environment/card source owns basic-card effects. Presentation completion cannot charge, damage or commit again.
- Game composition starts battle from a pending world request, projects victory once, and restores the pre-entry run on defeat/exit/start failure. Sandbox time does not advance during combat. ACT repository uses 0.1 simulation speed and rejects new move/normal-attack input; cleanup restores ordinary speed.
- Use native CommonUI screen/widget composition with a reusable 3D presentation stage. Home tabs switch UI and staged appearance without full map travel per tab. Local combat may use a distinct authored level; GameInstance run state survives and pending requests bind only after actors are ready. Level presentation is separate from static graph definitions and mutable save state.
- Dialogue definitions currently carry text lines and choices; portraits, voice and dialogue sequences need a later presentation extension. Rule completion grants rewards once. Combat Sequencer/Niagara/Montage hooks are presentation only, never reward/damage authority. Missing art uses explicit placeholders, not missing content silently treated as authored.
- Current HTML exclusions remain explicit. A new rule type/ability behavior still requires code; a new row using supported behavior does not. Skeleton compatibility, sockets, animation graphs, Niagara parameters and new shaders still need authoring.

## Evidence

TASK-046 owns build, targeted rule/GAS/catalog/rollback checks and playable assets. No networked ACT replay or production art/asset-streaming guarantee is implied. Runtime/manual acceptance will be recorded separately from native compilation.
