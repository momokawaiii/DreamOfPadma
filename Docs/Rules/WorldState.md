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

## Confirmed MVP map (2026-09-08)

Ordinary authored/generated maps follow authored nodes and preset connections. Hex-shaped ground and a Civilization VI-like oblique camera are presentation; touching decorative hexes do not automatically connect nodes. The TASK-051 tutorial extension below explicitly opts into generated hex adjacency.

The user accepted layered storage: a static chapter map definition, .umap presentation and SaveGame run changes. Stable NodeId/EdgeId values connect these layers. Occupancy, ownership, facilities, discoveries and unlocked roads live in the world state service; UI sends a command, legality/payment is checked, then the state change emits an event. An invalid move changes neither location nor cost.

See [ADR-0005](../Decisions/ADR-0005-World-Map-Storage.md). Exact nodes/edges, costs and conditions still need their recorded D12/D03 answers; this decision does not make movement free.

## Map inspection requirement

On 2026-09-10 the user changed node inspection to single-click pinning. Hover updates only the map highlight, never details or action targets; the clicked target remains while crossing other cells to reach sidebar controls. A separate Move Here command moves selected card instances. Inspection itself does not move the Pawn, pay or initiate battle; modals and held-card gestures gate background input. First-MVP enemy information remains public and technical travel is not a battle transaction. See [screen design](../Design/EN/06_MapAndPreparation.md).

Fresh tutorial generator v3 retains 55 cells and seven landmarks; fire starts as ruler territory with one guard and an open direct link from home. Deploy a living C card at home, select it and move to fire to use the existing Encounter route. Old v1/v2 saves do not gain this guard; start a fresh tutorial to test the shortcut.

## Tutorial placement and war display (TASK-052)

Select an A or B card in the hand, then explicitly click a target cell. A opens a deployment confirmation; B lists each colocated world A instance, with illegal choices disabled and their reasons shown. A cell without A cards shows an explanatory modal. Cancel spends nothing. The confirmation freezes card and node IDs, and the existing Core Deploy command revalidates card zone, phase, terrain, ownership, attachment eligibility and costs at commitment. Hover changes only the map highlight and cannot switch details, arm, redirect or settle a pending confirmation. Drag/drop and sidebar deployment use the same confirmation path. Long lists scroll vertically.

The UI presents neutral War=0 as player 50 / ruler 50. Player share is clamp(0.5 + 0.5 × War / warGoal, 0, 1); ruler share is its complement. The liquid animation does not change War, its authored initial value, or existing victory conditions.

## Card movement

Each card definition has Movable/Immovable/Conditional plus an authored flag; unconfigured is not default permission. Conditional movement must reference an implemented condition evaluator. A and attached B form one group; validate every card before atomic location/payment changes. Any failure leaves the group and ledger unchanged. UI cannot infer world movement from a model or ACT roster; per-card policies, conditions, costs and payment timing remain authored inputs.

## Fixed painting tutorial extension (TASK-051)

The latest user decision authorizes a fixed 2.5D tutorial painting with generated hex wilderness and occupiable habitable terrain. `tutorial-painted-v1` opts into explicit generated neighbor edges; ordinary map semantics remain unchanged. The seven existing key IDs keep their authored rules and special routes, with tutorial-specific painting coordinates. Terrain, elevation, axial coordinates and ownership are data; shader animation is presentation only. Impassable water/mountain cells cannot be deployed onto, moved into or occupied. Wilderness edges preserve authored lock availability and cannot open a bypass around a required road flag.

Player occupation uses the normal paid noon movement command, including atomic A/B movement. Entering empty neutral or hostile habitable wilderness transfers ownership without inventing a garrison or battle. Occupying wilderness grants no war-balance, core-damage or strategic clearing reward. At dusk, the ruler's bounded frontier pass claims at most one neutral, empty, habitable adjacent wilderness cell using deterministic ordering without consuming the shared gameplay random stream. This demo pass does not attack occupied player cells, create troops or model an AI economy. Save/rollback freezes the generated layout and ownership; restore validates each map family against trusted configuration. See [ADR-0009](../Decisions/ADR-0009-Painted-Tutorial-Hex-Map.md).

## Anchored generation and story checkpoints (TASK-048)

FixedNPC and MainStory nodes keep authored identity and coordinates across seeds. The former consistently places its NPC; the latter supplies character encounters/completion for later story. Detours are allowed. A configured checkpoint evaluates Seen/Completed once on arrival and records a matched/missed branch; detouring before it is not an immediate missed event. There is no universal all-anchors movement/victory condition. Road flags remain explicitly authored. Arrival outside noon records the encounter/checkpoint without opening dialogue; node dialogue resumes at noon. The run freezes generated nodes, directed edges and checkpoint definitions in its snapshot. See [ADR-0007](../Decisions/ADR-0007-Anchored-Map-Generation.md).
