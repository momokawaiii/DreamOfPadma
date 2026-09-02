# Save Schema

- Chinese companion for user reading: `Docs/Architecture/SaveSchema.zh-CN.md`

## Required principles

- Save stable IDs, not Actor or UObject pointers.
- Include a schema version and migration path.
- Keep transient presentation state out of the save.
- Persist the random seed or stream state whenever outcomes must be reproducible.
- Save at safe state boundaries before supporting mid-animation saves.

## Save sections

```text
Metadata
  SchemaVersion, BuildVersion, SaveTime, RunId
Random
  MasterSeed, stream/event position
Calendar
  Chapter, Era, Day, Phase
Global
  SoulFlow, Entropy, Faith, BattleScale, flags
World
  Node states, mutations, ownership, discoveries, garrisons, event state
Player
  Position, cards, deck, warehouse, upgrades, unlocks
Encounter
  Encounter ID, engagement route, battle mode, units, stability, buffs, cooldowns,
  action timeline, initiative, reaction/interrupt windows, extra-action state,
  Encounter active action or an explicitly supported ACT RealTimeAction snapshot
BattleTransaction
  PreBattleSnapshotId, entry world hash, commit/rollback state,
  complete versioned copy of every battle-mutated run section
Story
  Choices, quests, character relations, unlocked events
EventHistory
  Ordered world-changing events or a compact history reference
```

## MVP save scope

The first save implementation should support calendar state, global resources, current node, card collection, completed local-battle state, story flags, and a seed. Entering a local battle creates a complete versioned pre-battle snapshot of every run state that the battle could mutate, including world mutations, ownership, resources, card locations/lifecycle, entropy, faith, threat, war balance, story flags, event history, and deterministic random-stream positions. Success commits the battle result; defeat or player exit restores the complete snapshot exactly, with no partial battle cost or state left behind. Mid-battle save can be added after the action-timeline and ACT snapshot formats are stable.

The save must not serialize transient held-key state, bullet-time input state, camera state, or animation progress unless a separate deterministic ACT snapshot contract is approved. The safe-boundary MVP should restore before battle or after a completed battle.

## Migration

New fields should be appended with defaults. Breaking changes require a migration function and a test that loads at least one prior fixture.
