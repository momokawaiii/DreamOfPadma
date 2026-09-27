# PadmaGameplay

- Document ID: ARCH-MODULE-GAMEPLAY-001
- Version: 0.4
- Status: Implemented logical boundary; complete ACT Demo target pending; no standalone UE module
- Chinese companion: [README.zh-CN.md](README.zh-CN.md)
- Owner: Gameplay
- Parent: [ProgramArchitecture](../../ProgramArchitecture.md)
- ACT owner: [ACTDevelopmentContract](ACTDevelopmentContract.md)

## Current implementation and scope

The single `DreamOfPadma` module contains Gameplay/Combat transient ASC actors/attributes and separate Encounter/PadmaEncounterAbility and ACT/Runtime/PadmaACTAbility classes, typed effects, clocks and cleanup. Shared basic-card effects use a separate card/environment ASC. The current ACT executor commits a pending action and ends; it does not establish a third-person combo combat loop.

Existing ACT authoring exposes character/weapon/skill soft references and validation. See [ACTAuthoring](../../../Content/ACTAuthoring.md) and [NativePlayableDemo](../../../Content/NativePlayableDemo.md). TASK-048 retired TASK-008's fixed battle fixture; do not restore it as the current entry. Historical TASK-013/017/030/031/035 describe original allocations, not a reason to redo working infrastructure.

## Responsibilities and contracts

| Responsibility | Boundary |
|---|---|
| Separate Encounter/ACT battle execution, targeting, hits, status/death and mode clocks | Share only approved value/calculation contracts; never assume equal rows, timings or effects. |
| Native GAS ownership and lifecycle, ability validation/commit/cancel, transient runtime | No live GAS handles or Actor identities in Core/saves. |
| Action requests, typed results, battle completion and failure reasons | Input adapters and widgets do not settle rules. |
| ACT roster/weapon/trait validation using authored definitions | Separate from turn-based map cards; a restricted trait is not an implicit whole-character ban. |
| Battle-local state and cleanup | Game orchestrates travel and complete run snapshot commit/restore. |

World topology, persistent node occupation, synthesis/card lifecycle and save-file serialization remain outside this owner. Gameplay consumes approved definitions and emits presentation requests; animation graphs, Niagara art and widgets do not own damage/rewards. Asset configuration does not install a missing executor.

## Accepted Demo target

[ChapterZero](../../../Rules/ChapterZero.md) requires Encounter and one separate ACT node. [ACTDevelopmentContract](ACTDevelopmentContract.md) owns the one-new-character P0, native GAS/sequence decomposition and cleanup rules; [ACTMigrationMatrix](../../../Content/ACTMigrationMatrix.md) tracks source behavior and evidence. [DemoDeliveryPlan](../../../Production/DemoDeliveryPlan.md) begins ACT learning with model/retarget/3C, then adds abilities, combos and representative payloads.

The newer UI specification may replace old page/gesture design. Preserve card permissions, costs and clocks under [Combat](../../../Rules/Combat.md); explicitly revise rules if gameplay semantics change. Keyboard/mouse and single local authority are the Demo target. Multiplayer transport/prediction and hand-controller polish are deferred.

## Verification

Scope checks to the changed behavior: wrong-mode/target/trait rejection; configured damage/status results; commit and cancel cost behavior; duplicate-hit prevention; input/buffer window boundaries; task/delegate/spawn cleanup on interrupt/death/exit/travel; and full battle snapshot recovery. Gameplay settlement must remain independently testable from input and art.

Final ACT evidence includes the complete P0 behavior checklist plus user feel iteration, not just a successful GAS activation. Both packaged configurations and release gates are owned by [BuildMatrix](../../../Production/BuildMatrix.md). Full-project unresolved numerical rules remain in [MVPDecisionRegister](../../../Production/MVPDecisionRegister.md); do not copy that backlog into this README.
