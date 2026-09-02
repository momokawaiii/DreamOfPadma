# World State Rules

- Chinese companion for user reading: `Docs/Rules/WorldState.zh-CN.md`

World changes are represented by stable node IDs and explicit events. A coordinate is layout data, not the identity of a node.

Examples of persistent mutations:

- Node discovered
- Node cleared
- Garrison defeated
- Road opened or closed
- Faction ownership changed
- Weather or environmental state changed
- Story choice applied
- Resource or encounter table changed

Every mutation should be possible to describe as a command, validate as a rule, record as an event, and restore from a save.
