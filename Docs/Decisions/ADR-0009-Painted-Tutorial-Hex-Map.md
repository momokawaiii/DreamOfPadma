# ADR-0009 Painted tutorial hex map

- Chinese companion: ADR-0009-Painted-Tutorial-Hex-Map.zh-CN.md
- Status: Accepted
- Date: 2026-09-09
- Scope: TASK-051 and TASK-052; the user-authorized fixed tutorial painting, occupiable wilderness and click-pinned sidebar.

## Decision

The tutorial uses a fixed painting with a separately defined gameplay graph. `FPadmaTutorialMapGenerator` in Core produces the value-only `tutorial-painted-v1` layout with `bHexWilderness=true`. New generator-v3 runs retain v2's 55-cell grid and make fire, directly linked to home, a ruler outpost with one initial guard. Saved v1 (162 cells) and v2 (55 cells) layouts retain their prior garrisons and ownership. All three versions include the seven existing key nodes, whose IDs, story roles and authored directed links survive. Seeded wilderness supplies additional terrain and explicit adjacent links. This is an opt-in gameplay topology contract; ADR-0007's decorative PCG remains presentation for other layouts.

`FPadmaWorldNodeDefinitionRow` carries `HexQ`, `HexR`, `bWilderness`, `bHabitable`, `bTraversable` and `Elevation`. Painting coordinates use a 0–100 range, with the origin at the top left. The generator reserves a hex cell for each key node while keeping its displayed landmark position exact:

| Stable ID | X | Y |
|---|---:|---:|
| `home` | 10.5 | 48.0 |
| `fire` | 21.4 | 71.8 |
| `story` | 48.8 | 27.4 |
| `plain` | 48.2 | 51.5 |
| `boss` | 85.0 | 15.0 |
| `gate` | 77.5 | 41.5 |
| `forge` | 76.5 | 70.5 |

Plains, woodland, hills, wasteland, mountains and water are explicit data. The generator repairs necessary pass/ford corridors inside the same story-unlock partition and marks those corridors as habitable ground. Wilderness adjacency cannot cross partitions or bypass authored locked routes. Painting pixels, visible roads and GPU animation grant no movement rights. Decorative buildings in the painting do not create additional key nodes.

`FPadmaRunRules::PreviewWildernessOccupation` checks faction, terrain, ownership, an open directed edge and the absence of a defending garrison. Player occupation follows validated movement and its existing costs; an empty neutral or ruler wilderness cell can become player territory. `AdvanceWildernessFrontier` adds at most one ruler-owned cell at dusk, chosen from adjacent empty neutral habitable wilderness using a separate deterministic decision stream. This frontier feature adds no attacks on player territory, units or economic simulation. Wilderness occupation gives no war-balance, core-health or resource reward. Existing key-node settlement remains authoritative. Normative behavior belongs in [WorldState](../Rules/WorldState.md) and [Time](../Rules/Time.md).

The run freezes the full `FPadmaMapLayout`, including map ID/version/seed, hex fields, terrain and edges; runtime ownership remains in `FPadmaRunState::Nodes`. Save/load restores that data rather than regenerating it from an image or current seed. Existing layouts retain the default `bHexWilderness=false`. The controller explicitly chooses the tutorial for a fresh run unless `-PadmaLegacyMap` or the `encounter`, `act`, `roundtrip` capture scenarios request the legacy path. Continuing an existing run keeps its layout.

Startup and `StartTutorialMap` retain a copy of the original authored layout before tutorial generation. `UPadmaRunSubsystem::Initialize`/`StartNewMap` forward that reference to `FPadmaRunRules::Initialize(Content, AuthoredReference)`. Restore uses this trusted configuration even after active content changes between authored and tutorial layouts; tutorial compatibility is derived from the approved configuration rather than saved node definitions. Invalid anchors or unknown map identity cannot replace the active run. A tutorial-only initialization without an authored reference does not invent one. `AuthoredAndTutorialRestoreOrder` covers both restore orders and atomic rejection.

## Versioned grid configuration (TASK-052)

The saved layout carries `FPadmaHexGridConfig`: radius, vertical projection, origin, axial bounds and horizontal clipping range. Generator, border drawing and pointer containment use the same `CellCenter/CellCorner/ContainsPoint` functions. Versions 2 and 3 use radius 10, projection 0.65, origin (5,4), Q -5..6, R 0..9 and U 3..97. Legacy v1 uses radius 5.7, Q -9..10 and R 0..16; its other values match. Missing reflected configuration defaults to v1 so previous saves preserve their original size.

Map ID and layout Version remain tutorial-painted-v1 / 1; GeneratorVersion selects the trusted 55/162-cell preset. Validation rejects unknown versions or edited projection fields and derives compatibility from the independent authored reference. The v1 signature format remains unchanged; v2/v3 additionally sign configuration. Changing one editor radius alone is not an approved new map version. Fixed landmark display coordinates remain the table above; their containing axial cells may differ between versions. A first pan from exact fit adds 1.12× overscan before translating, keeping image and hit projection aligned without blank borders; Home restores exact fit.

## Presentation and input boundary

`SPadmaTutorialMap` draws background, territory, markers and movement previews, converting pointer positions to stable node IDs. `UpdateMapHover` updates only `HoverNode`, without changing `PreviewNode` or queuing details. On click, `SelectMapTarget` updates `SelectedNode`/`PreviewNode`; `MapDetailNode` supplies the pinned sidebar target. Crossing other cells cannot change that target. Actions carry explicit IDs through existing commands. Modal and drag gates remain, and selected A/B clicks still open confirmation.

The navy/gold HUD uses a proportionally scaled 1672×941 design canvas and the 1774×887 source painting. Material Time drives localized water, cloud/haze and forge-light effects without moving painted landmark UVs or requiring per-frame CPU parameter updates. Freeze/reduced-motion controls are documented in [StrategyPresentation](../Content/StrategyPresentation.md). This is a reference-guided reconstruction, not a pixel-perfect reproduction claim. Native rendering/input have been inspected; final atmosphere comparison and user visual acceptance remain tracked in TASK-051.

[ADR-0008](ADR-0008-Activatable-Presentation-Layers.md) continues to own CommonUI page/overlay activation, input routing and inspection timing. The painted map and clicked details add no new Blueprint activation hierarchy and do not decide the open D27 preparation-timing question.

## Consequences

Core owns generation, legality and saved topology; UI owns hit testing and drawing; assets own the painting and GPU atmosphere. Hex cycles are supported only by the explicit tutorial contract, while legacy map validation remains in effect for other maps. Deterministic topology, locked-route preservation, occupation costs/rewards, save restoration and click/hover input isolation require focused regression checks. Fixed visual geography limits this tutorial's reuse for arbitrary generated maps; future layouts need their own authored positions and presentation contract.
