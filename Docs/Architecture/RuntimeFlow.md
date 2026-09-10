# Runtime Flow

- Chinese companion: [RuntimeFlow.zh-CN.md](RuntimeFlow.zh-CN.md)
- Status: Current native routes plus accepted Chapter Zero target
- Owners: Game composition; Core settlement; Gameplay mode execution

## Current native flow

`Catalog load/validation → UPadmaRunSubsystem → world/UI composition → commands`

The current default is the painted tutorial. Legacy map options use the continuous terrain/PCG presenter. Map clicks pin details; hover only changes feedback. Selecting an A/B hand card then clicking a cell opens the appropriate confirmation/attachment target flow. CommonUI page/overlay stacks gate input independently of Core command legality.

Movement into a guarded node may create the full pre-entry run snapshot and pending battle. OpenLevel retains the GameInstance; the battle controller creates the mode's transient GAS objects. Success commits once; defeat, exit or startup/travel failure restores the snapshot. Returning to World reconstructs presentation. Home tabs and optional streamed node scenes are presentation, not separate rule authorities.

This is the implemented subset in [NativePlayableDemo](../Content/NativePlayableDemo.md), not proof that a main-menu-to-ending tutorial exists. Activation and teardown remain owned by [ADR-0008](../Decisions/ADR-0008-Activatable-Presentation-Layers.md) and TASK-052.

## Accepted Demo golden path

`Launch → main menu → opening → Chapter Zero → movement → calendar/resources → ABC cards → synthesis → node event/dialogue → Encounter → ACT → occupy royal court → ending`

Encounter and the single ACT encounter are on different nodes and both must be played. No FPS node. Each gameplay step waits for a successful real operation; clicking its button or finishing an animation alone cannot complete it. The precise event vocabulary and step-to-node/card bindings are still implementation/content work.

| Stage | Orchestrator / authority | Required observable result |
|---|---|---|
| Start / continue | Game | New run uses baked chapter content; continue validates and restores saved content/state |
| Opening | Game story runtime + Presentation | Playback can finish/skip without directly issuing duplicate rewards |
| Map / tutorial step | Core/Game | Legal action changes real state and advances the eligible step |
| Event / dialogue | Core/Game story runtime | Valid choice has explicit effects and stable continuation |
| Encounter then ACT | Game + separate Gameplay adapters | Each battle enters, settles/rolls back and returns correctly |
| Royal court / ending | Game outcome/chapter orchestration | Authored ending is reached and tutorial status is recorded |

Exact interaction between existing alternative victory conditions and the Demo ending needs content design; do not silently remove war-balance victory or make every anchor a mandatory visit.

## Target chapter and random-story lifecycle

1. Resolve ChapterDefinition and matching baked map; validate references before replacing the active run.
2. Restore existing story decisions/phase assignments when continuing. For a newly entered calendar stage, evaluate eligible pools and determine node assignments at the stage's first map load, before node inspection.
3. Save the chosen EventIds and stage identity. Loading the same save, reopening the UI or cancelling dialogue does not redraw assignments.
4. A new calendar stage reevaluates eligible content. The same event may recur when its repeat policy allows it; ordinary stories are not universally once-only.
5. On a trigger, check authoritative conditions and enter the selected event. Player choice/operation goes back through services; UI/Sequence only presents it.
6. When entering a main checkpoint, lock its event/branch once. Later NPC visits or changed flags do not rewrite that decision.

Q30 allowed same-stage variety, but Q33 subsequently accepted first-stage-load assignment persisted in the save. Q33 is the governing behavior. A calendar stage key must identify its chapter/era/day/phase occurrence, not just the label “Dawn”; exact encoding is pending.

Special nodes may affect the main story. The proposed mechanism “flags/unlocked edges consumed by future checkpoints, no direct graph jumps” remains unanswered Q39. Keep current checkpoint behavior; scope any new cross-event mechanism explicitly. Multi-event ordering, repeat cooldown clocks and pool conflict priority need authored policy.

## Target tutorial skip and replay

| State / operation | Required behavior |
|---|---|
| NotStarted → InProgress | Begin a tutorial run with its own identity/version |
| Successful taught actions → Completed | Record actual completion and the authored completion rewards |
| Skip whole tutorial → Skipped | Apply configured post-tutorial reward initialization and unlock continuation; display skipped status |
| Skip only a cue | Fast-forward presentation; do not mark the whole tutorial complete or reissue effects |
| Replay from settings | Start a new TutorialRunId; the user may earn rewards again |
| Load / crash retry | Restore the same run identity and receipts; do not duplicate that run's reward |

Concrete reward contents, preserved inventory when replaying, save-slot/profile ownership and battle-failure resume points are not finalized. The required uniqueness/restore contract is in [SaveSchema](SaveSchema.md). The tutorial runtime is required; a custom Slate story editor is not.

## Battle, input and clocks

Encounter and ACT retain separate validators, GAS objects, definitions and clocks under [ADR-0004](../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md). Shared basic-skill identities select mode-specific bindings. Encounter's eligible action-turn card allowance and ACT's Tab/blur/0.1-time/five-slot card input are governed by [Combat](../Rules/Combat.md), not duplicated here.

The sandbox calendar pauses during either battle. A scene or overlay cannot settle battle results or alter the pre-battle snapshot. Inspection is read-only: current Encounter enemy autoplay waits while inspection is active; ACT inspection retains its own clock/Tab policy. Closing an overlay restores appropriate focus/input, not a new gameplay action.

## Failure and verification

Reject missing content, incompatible maps or invalid choices before changing the active state. Do not silently regenerate a saved map or redraw a saved story. Reconstruct cameras, widgets and GAS objects from stable state.

Test new/continue, cancellation, first-stage assignment versus reload, checkpoint locking, tutorial completion/skip/replay rewards, battle commit/rollback and clean exit. These are target acceptance cases; current evidence is in [ProjectState](../ProjectState.md). Packaged end-to-end evidence is governed by [BuildMatrix](../Production/BuildMatrix.md).
