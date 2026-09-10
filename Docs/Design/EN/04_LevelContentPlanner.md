# MVP Design Baseline 04 - Level and Content Planner

- Document ID: `DESIGN-MVP-LEVEL-CONTENT-001`
- Status: High-level MVP baseline accepted; exact content identities and layout remain playtest decisions
- Baseline accepted: 2026-09-02
- Canonical language: English for Agent consumption
- Chinese companion: `Docs/Design/ZH/04_LevelContentPlanner.md`
- Owner: Level / Content Planner
- Related contracts: `Docs/Rules/WorldState.md`, `Docs/Rules/Time.md`, `Docs/Architecture/SaveSchema.md`, `Docs/Content/Naming.md`

## 1. Level objective

The level is a shared sandbox, not a disconnected mission selector. Its purpose is to make movement, node ownership, card deployment, ruler response, jointly constrained engagement routes, route-selected battle presentations, story choices, and world mutation visible in one small graph.

The MVP uses a hand-authored graph with data-driven node state. PCG, World Partition, large-scale terrain, weather, water, and foliage are later production extensions. The MVP should preserve the data contracts that allow those systems to replace the hand-authored presentation later.

## 2. Proposed sandbox shape

Use a compact directed graph with approximately eight nodes. The following IDs are proposals for stable identity, not final names:

| ID | Node role | Initial owner | MVP purpose |
|---|---|---|---|
| `N_PLAYER_HOME` | Home / core node (`归处`) | Player | Player core, initial hand, safe starting state. |
| `N_SAFEHOUSE` | Bonfire / safe-house node (`薪火`) | Neutral | Recovery, tutorial checkpoint, and pre-battle save demonstration. |
| `N_FURNACE` | Furnace node (`熔炉`) | Neutral | One temporary card enhancement or data transformation, then becomes an ordinary node. |
| `N_ANECDOTE` | Anecdote node (`轶闻`) | Neutral | One story choice that changes threat, faith, or a route. |
| `N_FRONTIER_WEST` | Ordinary node | Ruler | First capture and patrol tutorial. |
| `N_FRONTIER_EAST` | Ordinary node with terrain affix | Ruler | Alternate route, stealth or detection test. |
| `N_RULER_OUTPOST` | Facility / defense node | Ruler | Reinforcement and counterattack test. |
| `N_RULER_CORE` | Home / core node (`归处`) | Ruler | Boss access and core victory test. |

At least one path is hidden, conditional, or visually incomplete until a requirement is met. The requirement must be an explicit data condition, not a hard-coded level-name check.

## 3. Sandbox ownership and movement

The player starts with less territory. The ruler has a larger controlled area and superior map familiarity. The player plans a route, captures nodes, deploys facilities or units, and eventually reaches the ruler core.

The source movement-cost examples are:

- A card movement: 2 computation.
- C character or configured failure-side object movement: 4 computation.
- Construction movement: 6 computation.
- Core movement: dependent on the number of cards on the field and potentially debt-enabled.

B cards follow an A card when the A card moves. The exact behavior of a detached B card, a moving attached pair, and a core carrying field cards is a review item.

## 4. Node operations

The MVP should expose a small command set:

- Move an eligible card or unit to an adjacent node.
- Place an A card on a node.
- Attach a B card to an A card or play it through the direct-B path.
- Execute synthesis at a legal location.
- Deploy a C character or construction, or apply an explicitly configured non-reward failure-side consequence.
- Activate an eligible ABC-card active ability without entering a battle.
- Move an ABC-card character or unit onto a point where an enemy is staying, resolve the jointly constrained engagement route, and enter the selected Encounter, ACT, or future RealTimeAction presentation.
- Capture a neutral or ruler node after a legal Encounter action-timeline battle or the complete ACT RealTimeAction battle route.
- Build or activate one facility.
- Enter an anecdote and choose one of two outcomes.
- Use a safe-house recovery action.

Every operation produces a visible state change and an event suitable for save history. Node coordinates are layout data; node IDs remain stable when the visual map is rebuilt.

## 5. Node rules for the MVP

### 5.1 Home / core node

Stores a core and its persistent values. Ruler-core life reaching zero or war balance reaching the player boundary ends the MVP with victory. The outcome system also supports configured future victory and defeat conditions.

### 5.2 Ordinary node

Supports movement, deployment, ownership, and a local battle route. An ordinary node may have a terrain affix that can constrain the engagement route or action timeline.

### 5.3 Anecdote node

Presents a story or world-state choice. One option should help the player but alter entropy, faith, or threat; the other should trade a short-term resource benefit for a different future consequence.

### 5.4 Furnace node

Allows one controlled strengthening or transformation operation. After the operation, the node becomes an ordinary node, as described by the source concept.

### 5.5 Bonfire / safe-house node

Provides recovery or a safe planning boundary. It is also the tutorial location for save/load and pre-battle autosave feedback.

## 6. Proposed seven-day Era content schedule

This is a pacing proposal, not canon story content:

| Day | Content beat | System proof |
|---:|---|---|
| 1 | Start at player home and reach the first frontier node. | Dawn resources, movement, node ownership. |
| 2 | Attach a B card to an A card and preview synthesis. | Tag vector, candidate list, probability display. |
| 3 | Execute one synthesis and deploy the result. | Success/failure, C deployment, resource accounting. |
| 4 | Visit the anecdote node. | Story flag, faith/entropy/threat change. |
| 5 | Use the furnace or safe-house. | Node-specific operation and save boundary. |
| 6 | Trigger the ruler patrol response or counterattack. | Detection, reinforcement, action-timeline Encounter or complete ACT RealTimeAction entry. |
| 7 | Reach the ruler outpost or core gate and resolve the Era end. | Boss access, victory/defeat, day/Era transition test. |

The game must preserve the fixed seven-day Era structure even if a development command accelerates days during testing.

## 7. Content examples

The example cards provide a coherent first content family:

- `地痕` plus `蓍` can demonstrate a recommended path toward `犹格索托斯`.
- `安提帕鸿门宴` plus `埃及艳后` can demonstrate a recommended path toward `希律的舞女`.
- `佛罗伦萨的曙光` plus `山樱一梦` can demonstrate a recommended path toward `维特鲁威人`.
- A configured failure-side consequence such as `一个归来的王` can demonstrate a visible non-reward consequence; a failed synthesis must not silently grant a reward.

These are sample content identities. Their exact balance, visual presentation, and narrative text are not frozen by this document.

## 8. Ruler response and world mutation

The ruler should respond to the map rather than wait as a passive encounter queue:

1. Patrol nearby nodes.
2. Detect or fail to detect the player's forward unit.
3. Reinforce a captured facility.
4. Counterattack a weakly held node.
5. Increase threat as days and choices advance.

World mutations recorded in the MVP include node discovered, node captured, facility activated or destroyed, path unlocked, story choice applied, engagement route/mode triggered, encounter completed, and a configured weather/terrain flag.

## 9. Story scope

The MVP has one short anecdote with two choices and one consequence visible on the map. It should demonstrate that a story choice is a gameplay command and a persistent state mutation, not only dialogue text.

The full chapter narrative, complete relationship network, multiple story branches, and thematic victory grading remain future content. The latest user request adds the minimum relationship/bonus capability and its global view through TASK-039/038; named content and exact effects remain D28. Story-character IDs remain distinct from combat-character IDs.

## 10. Level technical scope

For the first playable build:

- Use one small map or one small set of levels with a clear transition boundary.
- Use a hand-authored node graph and data-defined node states.
- Use simple placeholder geometry and lighting.
- Keep an explicit seam for later World Partition and PCG adoption.
- Make the map state serializable without saving Actor pointers.
- Keep project-owned assets under `Content/Padma/` and external assets under `Content/ThirdParty/`.

The first map is a rules testbed. It is not the final art direction or final chapter geography.

## 11. Level acceptance checklist

- [ ] The player and ruler share one visible graph.
- [ ] The map contains home, ordinary, anecdote, furnace, and safe-house node types.
- [ ] At least one path is conditionally unlocked.
- [ ] The player can capture a node and the ruler can respond.
- [ ] A terrain affix changes one visible rule.
- [ ] One story choice changes persistent world state.
- [ ] Node IDs remain stable independently of coordinates and level assets.
- [ ] The player can reach and challenge the ruler core.
- [ ] An ABC-card character reaching an enemy-occupied point visibly triggers the local battle route.
- [ ] The resolved battle route can select/trigger the action-timeline Encounter or the complete ACT RealTimeAction MVP loop, and future modes remain extensible.
- [ ] Terrain, side choices, or card effects can constrain the engagement route, and the selected route can expose its story consequence.
- [ ] A seven-day Era can end and the next boundary can be tested.
- [ ] The complete pre-battle snapshot, exact return-to-sandbox rollback on defeat/exit, and success commit are visible.

## 12. Open level decisions

- Exact node count and graph shape.
- Whether both cores live in separate `归处` nodes or use another shared-core interpretation.
- Whether overworld and encounter use one World Partition world or separate levels.
- Exact hidden-path requirements and terrain affixes.
- The first anecdote text and its two consequences.
- The exact data for which node, card, story choice, or encounter condition triggers each battle route/mode, and how both sides jointly constrain the engagement route and its story consequence.

## 13. Learning targets

This baseline is intended to teach level design, graph-based content modeling, node ownership, world-state mutation, encounter transitions, story flags, save boundaries, data-driven content, World Partition planning, PCG seams, and later environmental production.

## 14. ACT roster and terrain handoff (2026-09-08)

The deployed sandbox unit is a turn-based character-card representation. When a route selects ACT, carry node/terrain context separately from the ACT character/weapon roster selected in battle settings. The ACT character collection is distinct; a one-to-one conversion from the triggering unit is not assumed. Terrain may disable an authored trait; the exact constraint target, timing, linkage and fallback policy require D20/D14 decisions.

TASK-024/025 supply world and terrain views, TASK-035 supplies roster/eligibility, TASK-027 composes battle entry and TASK-030 enforces ACT execution restrictions. Placeholder assets are imported by the user after work starts. Existing maps/content are extended through task-owned fixtures and later the First Era pack, not bulk-copied from the reference prototype. See [TASK-011](../../Production/Tasks/TASK-011-Full-MVP-Task-Cluster.md).

The user confirmed preset node connections over a hex-shaped map and layered map-definition/.umap/SaveGame storage on 2026-09-08. Camera and hex layout do not imply hex-neighbor movement. See [ADR-0005](../../Decisions/ADR-0005-World-Map-Storage.md).

See [map and preparation design](06_MapAndPreparation.md) for the new hover/local-scene/global UI requirements and the open interaction decisions.
