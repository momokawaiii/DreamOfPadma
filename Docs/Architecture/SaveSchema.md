# Save Schema

- Chinese companion: [SaveSchema.zh-CN.md](SaveSchema.zh-CN.md)
- Status: Format 1 currently implemented; Chapter Zero additions are an accepted target, not a format migration
- Owners: Core value/state validation; Game disk envelope and restore orchestration

## Current format and compatibility

[UPadmaRunSaveGame](../../Source/DreamOfPadma/Public/Game/Save/PadmaRunSaveGame.h) stores `AppId=padma-native-demo`, `FormatVersion=1` and reflected `FPadmaRunState`. The current slot is `PadmaDemo046`. It includes calendar/resources, RNG position, card partitions, preparation, node state, flags/history, frozen map layout, Seen/Completed NPC IDs and once-only checkpoint decisions.

Current saves reject combat/dialogue boundaries, incompatible envelopes and invalid state. They are not HTML imports or mid-battle snapshots. Battle rollback uses a complete in-memory run snapshot. Global battle-settings presets are not implicitly run state; current native preparation is run-owned.

[ADR-0007](../Decisions/ADR-0007-Anchored-Map-Generation.md) supersedes ADR-0005's static-topology-only save policy: frozen generated **value topology** is saved. Trusted configured anchors are retained independently of loaded save data.

[ADR-0009](../Decisions/ADR-0009-Painted-Tutorial-Hex-Map.md) documents tutorial v1/v2/v3 compatibility: missing hex fields retain v1's 162 cells; v2/v3 have 55; v3 adds the nearby guard only for fresh runs. Prior layouts retain ownership/garrisons. Map/layout/envelope identifiers and the v1 signature are unchanged. Do not repurpose these integer generator versions as the new manifest.

## Accepted target sections

Names below define information requirements, not current reflected members.

| Section | Persisted information | Restore rule |
|---|---|---|
| Envelope | Schema/build/content compatibility, run identity | Validate before applying any section |
| Map | MapKey, frozen gameplay layout and generator schema | Load compatible baked visuals; never regenerate topology from current algorithms |
| Run | Calendar, resources, card instances/piles, ownership/garrisons, facilities/unlocks, preparation and outcome | Restore values, then rebuild presentation |
| Random | Persisted gameplay/story stream state or equivalent deterministic draw position | UI and cosmetic operations never advance these streams |
| Story | Seen/Completed NPCs/events, flags, choices, locked checkpoint branches, node story assignments | Reuse resolved choices and assignments |
| Tutorial | TutorialVersion, TutorialRunId/index, NotStarted/InProgress/Completed/Skipped, step progress and reward receipts | Continue the same run; replay explicitly creates a new identity |
| Battle boundary | Complete versioned pre-entry values for every battle-mutated run section | Commit once on success; exact restore on failure/exit |

A node story assignment needs at least NodeId, the concrete calendar-stage occurrence, StoryEventId and the applicable pool/content version. A label such as “Dawn” alone collides across days. Serialization shape and stream split remain implementation work; do not silently change existing RNG sequences.

Save no Actor/UObject/ASC pointers, live ability/effect handles, camera handles, Sequencer players, editor graphs, widget focus/capture, held keys or animation timing. Rebuild these objects from stable definitions/state. Mid-cue/mid-battle saving requires a separate supported snapshot design.

## MapKey and version checking

`MapKey = ChapterId + Seed + GeneratorVersion`

GeneratorVersion identifies an immutable manifest covering terrain algorithm, PCG graph, theme/biome, map configuration and generated schema. The authored manifest and baked asset metadata are trusted references; saved metadata cannot redefine that trust.

1. Validate the save envelope and look up its MapKey in the installed baked-content catalog.
2. Require matching baked metadata and a compatible frozen value layout/anchor contract.
3. Validate referenced content/IDs and all state before replacing the active run.
4. On mismatch, preserve the active run/save and report incompatibility. An explicitly implemented migration or user-chosen new run may follow; never silently regenerate, discard unknown nodes or overwrite the old save.

For example, the same ChapterId/Seed with MapGenA and MapGenB identifies different map versions. If the installed package retains MapGenA it may load that matching asset; otherwise it needs supported migration or a new run. No requirement here forces retention of all historical art packages.

ChapterContentVersion is separate. Editing dialogue alone does not change map identity, but persisted EventIds/choices still need content compatibility checks. Map-compatible does not automatically mean story-compatible.

## Tutorial reward and recovery invariant

Each tutorial run distinguishes completion from skipping. Skipping initializes the configured post-tutorial rewards and permits continuation. A deliberate settings replay creates a new TutorialRunId and may award them again. Loading or retrying the same run must not duplicate them.

A conceptual receipt key is `TutorialRunId + RewardId`. State changes and receipts must recover consistently; a crash between them cannot produce a second grant or a lost grant. Transaction layout, durable write protocol, reward contents and replay inventory/profile ownership are not yet settled. Do not claim an implementation simply by adding an in-memory boolean.

Ordinary repeatable story rewards require their own occurrence policy; tutorial receipts must not accidentally make every random story once-only.

## Migration and acceptance

Adding these target fields requires a versioned implementation and explicit legacy behavior. A new field's default is safe only if its semantics are defined; do not infer “completed tutorial,” redraw missing stage assignments or grant rewards when opening an old save.

Required cases for that implementation:

- Same save restores map values, ownership, RNG and locked story branches without rerunning generation.
- Manifest/anchor/content mismatch rejects atomically; supported old painted layouts remain covered.
- Stage-first-load selects once; save/reload, cancelled dialogue and UI inspection preserve the result; a new stage can select again.
- Completed and Skipped remain distinguishable; same-run retry cannot regrant, new-run replay can.
- Interrupted reward/save recovery cannot partially apply state; failed battle restores every registered run field and random position.
- Development and Shipping restore without editor assets, source paths or live object references.

Current test evidence remains in TASK-046/047/048/052. These additions have not been coded or packaged by the documentation task.
