# Module Map

- Chinese companion for user reading: `Docs/Architecture/ModuleMap.zh-CN.md`

## Target dependency direction

```text
PadmaCore
   ↑
PadmaGameplay    PadmaWorld    PadmaUI
       \            |            /
              PadmaGame
```

`PadmaGame` is the composition root. `PadmaCore` contains rules and data contracts and must not depend on UI, maps, Niagara, or concrete gameplay Actors.

## Target responsibilities

### PadmaCore

Calendar, resources, card definitions, synthesis, seeded random streams, commands, events, result types, and save schemas.

### PadmaGameplay

Units, combat, abilities, GAS, buffs, targeting, AI policies, and battle orchestration.

### PadmaWorld

Hex map, node state, encounters, world mutations, World Partition, PCG, weather, and environment systems.

### PadmaUI

Widgets, view models, HUD, card interfaces, map interfaces, and input-to-command adapters.

### PadmaGame

GameMode, GameState, PlayerController, session subsystems, mode routing, and cross-module composition.

### PadmaEditor and PadmaTests

Editor-only asset/data tools and automated tests. They must not become runtime dependencies.

## Staged adoption

The current blank project has one generated module. Do not create empty modules for appearance only. Establish a boundary in folders and documentation first, then split into a UE module when a boundary has stable ownership or an independent test/build need.

## Program documentation map

- Program-wide ownership and dependency rules: ProgramArchitecture.md
- Data definitions, asset types, loading, validation, and versioning: DataDrivenArchitecture.md
- Architecture Agent rules: AGENTS.md
- Module Agent rules: Modules/AGENTS.md

Each logical module has a technical README:

- Modules/PadmaCore/README.md
- Modules/PadmaGame/README.md
- Modules/PadmaGameplay/README.md
- Modules/PadmaWorld/README.md
- Modules/PadmaUI/README.md
- Modules/PadmaEditor/README.md
- Modules/PadmaTests/README.md

The README files are implementation contracts and ownership records. They may describe a planned boundary before code exists. They must not be read as proof that a standalone UE module already exists.
