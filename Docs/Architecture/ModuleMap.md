# Module Map

- Chinese companion: [ModuleMap.zh-CN.md](ModuleMap.zh-CN.md)
- Status: Current folder map; offline Demo additions are targets

One UE runtime module, `DreamOfPadma`, contains these logical boundaries. Dependencies and settlement rules are owned by [ProgramArchitecture](ProgramArchitecture.md).

| Domain / owner | Current source roots under Source/DreamOfPadma/{Public,Private} | Current responsibility | Demo addition, not yet delivered |
|---|---|---|---|
| [Core](Modules/PadmaCore/README.md) | Core/Content, Core/Run, Core/WorldMap, Core/Cards | Value definitions, rules, frozen graphs/checkpoints, RNG and validation | Chapter/story/tutorial value contracts and persisted phase assignments |
| [Game](Modules/PadmaGame/README.md) | Game/Content, Game/Run, Game/Save, Game/Framework, Game/WorldMap, Game/Presentation | Catalog/session/save/travel composition and editor map bridge | Chapter orchestration, baked MapKey lookup and version checks |
| [Gameplay](Modules/PadmaGameplay/README.md) | Gameplay/Encounter, Gameplay/ACT | Separate native GAS modes, effects and combat adapters | Production content and ordered tutorial encounters |
| [World](Modules/PadmaWorld/README.md) | World/Map, World/PCG | Continuous terrain, cached roads/proxies and cosmetic PCG | Frozen baked presentation bound to authoritative map metadata |
| [UI](Modules/PadmaUI/README.md) / Presentation | UI, Presentation | CommonUI/Slate HUD, layered inspection and feedback | Menu/opening/tutorial/end flow and final visual treatment |
| [Editor](Modules/PadmaEditor/README.md) | Existing editor-gated adapters; Scripts/Editor outside Source | Python authoring, model/map previews and validators | Bake/validation tools; story graph editor deferred |
| [Tests](Modules/PadmaTests/README.md) | Private/Tests | Rule/catalog/map/UI regressions | Chapter state, migration and packaged golden-path coverage |

World Actors and cosmetic PCG consume state; current mutable world settlement is in Core/Run, coordinated by Game. The default painted tutorial uses a separate Slate presenter; it does not prove that the 3D map bake path is complete. See [WorldMapAuthoring](../Content/WorldMapAuthoring.md).

Editor and Tests are logical responsibilities, not seven standalone UE modules. A module split needs a demonstrated dependency/build/test reason. Runtime cannot depend on a future Slate/UEdGraph editor. Detailed class mapping is in [RuntimeNaming](RuntimeNaming.md); current evidence is in [ProjectState](../ProjectState.md).
