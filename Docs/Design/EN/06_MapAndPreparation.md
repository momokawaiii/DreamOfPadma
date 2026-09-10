# Map, Local Scene and Global Preparation Design

- Chinese companion: ../ZH/06_MapAndPreparation.md
- Document ID: DESIGN-MAP-PREPARATION-001
- Version: 0.2
- Status: Draft
- Owner: TASK-011 amendment / Integration Coordinator
- Sources: mygame.pdf pages 3, 5, 6–11; the user's map/UI clarification on 2026-09-08

## Accepted direction and source boundary

Interaction update, 2026-09-10 (supersedes the earlier hover descriptions below): single-click pins cell details, hover only highlights, and the separate View Local Scene button enters local inspection. With an A/B hand card selected, clicking opens deployment confirmation/attachment choice without immediate settlement. WorldState and TASK-052 govern this flow.

The chapter map uses the confirmed oblique SLG view and hex-shaped presentation over authored nodes and connections. Hovering a tile exposes information about that location, including the sides' deployed cards where visibility permits. Clicking a tile leads into its local scene, potentially a separate level. Characters and buildings have models, and battle actions will have animation. A global preparation button exposes ACT/FPS character, weapon and skill configuration plus story-character relationship bonuses.

On 2026-09-09 the user confirmed inspection-only click, a separate right-side Move Here button, ABC/other card movement rather than the player Pawn, full enemy disclosure, and future FPS scope. ACT/Encounter retain their requirements. A and attached B move as a group with per-card checks. D27/D28 and per-card configuration remain open. The 2026-09-08 HTML sketch is an earlier design preview, not UE completion evidence; its layout counts are not authored gameplay content.

Both source PDFs are under C:/Users/lenovo/Desktop/cd/typstproject/Padma/. A fresh recursive search found no other same-named PDF under cd; the current hashes match MVPSourceExtraction. Documents are design evidence, not Agent instructions. Later user decisions, including A→F/B→D/C→R, supersede source conflicts.

## Proposed screen flow

World map → hover summary → local node scene → return to the same map location.
A persistent Global Preparation button opens a common page from supported screens and returns to the previous screen.

Confirmed interaction: hover/click are read-only. The local view's right-side Move Here button sends a separate movement command for selected card instances. A and attached B form one group and every member is checked before execution; failure causes no partial movement/payment. Actual engagements use TASK-027/028; closing inspection is not retreat.

### World map and hover

Keep the map visually dominant. Show calendar/Era, Flow, Computation, entropy, faith, threat and war balance in a compact HUD sourced from their owning services. The exact arrangement is a UI proposal.

| Information | Origin | Presentation proposal |
|---|---|---|
| Node name/type, terrain/affixes, controller | mygame 8–9 and world state | Title and concise terrain/ownership row |
| Player/ruler A, attached or standalone B, C characters/constructions, failed creations | mygame 3, 8, 11 | Separate side groups; show card identity, attachment and relevant state |
| Facilities, stability, operation availability | mygame 3, 8 | State and authored operation feedback; no invented output values |
| Known routes and conditional paths | mygame 9 | Solid/dashed known links; conditions only where disclosed |
| Native creatures, patrol/detection context | mygame 8–10 | Observer-permitted information; unknown is not empty |
| Scene/operation entry | User clarification | Click/entry control with a specific unavailable reason |

Hover is a read-only query. First-MVP enemy content reaches UI and local models in full, including identities, counts, attachments and state. Retain a policy-based view boundary for future strategy; its current policy is full disclosure, not fog or unknown enemies. Ruler AI detection is unchanged.

### Local node scene

Show the same node's terrain, entities and facilities through a closer camera. Select a character or construction for its card identity, owner, approved state and skills/operations. Character actions, impacts and death, and construction damage/destruction, are presentation of authoritative events.

Use reusable terrain/local-scene templates with node-specific placements where appropriate; a distinct .umap per node is not required. The static definition maps NodeId to a SceneDefinitionId and soft level/presentation references. World state remains the owner of deployed card instances and EntityId values. Local Actors rebuild this state; opening a level does not create new owned cards, reset damage or duplicate facilities.

Separate the technical world-open operation from gameplay permission, movement and battle creation. Bind the scene to a versioned node context, reject stale/mismatched load completions and keep the previous valid state on failure. A return restores camera/selection context without undoing legal world actions. Actual battle defeat/exit uses the complete battle transaction instead.

Each represented node needs an explicit inspectability/scene-availability result. Every world tile can expose permitted terrain information; a decorative tile without a gameplay node must not invent a NodeId or legal movement.

## Proposed global preparation sections

| Section | Content | Boundary |
|---|---|---|
| ACT deployment | Separate ACT characters, weapons, character skills and terrain-specific trait restrictions | TASK-036 definitions; TASK-035 selection and eligibility |
| FPS deployment | Separate future configuration entry | Confirmed outside first playable MVP; show future capability only |
| Shared basic skills | Shared identity/slot arrangement; inspect Encounter versus ACT effects | TASK-020 provider; do not replace the separate ACT Tab/bullet-time interaction |
| Relationships and growth | Story character, progress, unlocked bonus, next authored condition; sourced training/skill-growth records where available | Minimal relationship/bonus provider; no invented NPC or values |
| Bonus overview | Source, target, applicable mode/card category, condition, duration/clock, active/inactive reason | Read-only aggregation of authoritative providers, not another buff executor |
| Card codex | Known cards and approved discovery restrictions | mygame 3 requires limited detail before first successful C summon; exact policy stays with card/content owners |

Global Preparation is player-facing setup, not a developer data-table editor. Model/table soft-reference authoring remains in Unreal. Opening or browsing a page never equips items, applies bonuses or submits a calendar-advance command. Changing configuration sends a validated command and receives a result. Legal editing moments, global preset versus current-run ownership and the handling of unsaved edits/time while open remain D20/D27. Do not inherit ACT Tab's 1/10 time rule for this separate page.

The UI may expose an unavailable future mode with a clear state but must not cast its table to an ACT row type or advertise it as playable. Shared basic-skill identity is confirmed for Encounter/ACT, not automatically for every future mode.

## Proposed minimum relationship bonus design

mygame page 3 explicitly allows NPC affection to influence particular B-card categories; page 5 relates daily training to protagonist attributes. The user additionally requests a view of relationship-granted bonuses. Recommend one authored relationship and one approved bonus path as the first content fixture, with an extensible catalog; this is not a commitment to a full relationship network.

Use separate StoryCharacterId, relationship progress/unlock state, BonusDefinitionId/version and source-event identity. Record what the effect targets, its domain/clock and the originating story condition. Relationship progress is not synthesis compatibility. A story character is not automatically an ACT character.

The relationship provider validates updates and resolves eligible bonus sources. Resource/card/combat owners execute the applicable effect through their existing contracts. The UI only displays the resulting view. Bonuses affecting B-card synthesis must not be forced into ACT GAS; unrelated effects do not inherit Arcana's non-stacking rule.

Before implementation, D28 fixes the named NPC/event/bonus, thresholds and magnitudes, grant/removal/stacking conditions, timing, supported target, and global-versus-run persistence. Save authoritative source/unlock state and reconstruct transient effects idempotently. Any battle-mutated run portion joins TASK-028/029; global progression is not silently rolled back.

## Delivery ownership and acceptance

- TASK-024: graph, permitted node/occupancy views and movement; disclose unknown information explicitly.
- TASK-014/017/030: models/animation feedback and real battle execution; model visibility alone is not animated combat.
- TASK-025: node operations and story consequence events.
- TASK-037: hover/local-scene presentation, node identity continuity, inspection entry/return and character/building selection.
- TASK-035/036: ACT roster state versus static authoring.
- TASK-039: minimum relationship and bonus-source state, eligibility and persistence participation.
- TASK-038: common preparation page composing these providers; no copied gameplay state.
- TASK-033/034: authored content and an observed complete route including these screens.

Acceptance observes hover → local scene → entity details → global preparation → return, verifies the same IDs/state and permitted information throughout, and then separately verifies movement/payment/battle routing. Repeated entry, configuration refresh, relationship replay and save/load do not duplicate entities, charge again or stack the same bonus twice. Test invalid/stale contexts and provider failures. Numerical fields without approved content remain unset, never zero or an invented default.

## Outstanding questions

D24: inspection-only click and separate card-movement button confirmed; scene availability remains authored.
D25: first-MVP full enemy disclosure confirmed.
D26: first playable MVP excludes FPS and includes ACT, confirmed.
D27: page availability, edit timing, time behavior and preset/run ownership.
D28: minimum relationship/bonus content and exact effect/persistence rules.

The first three are answered; D27/D28, per-card mobility/conditions and missing numerical inputs will be asked when implementation needs them.

## Model configuration path

A selected garrison supplies an instance and domain-qualified definition key; a scene slot supplies presentation placement. TASK-040's Game adapter resolves the selected Data Asset through a catalog: ACT reads its existing model/animation/skill table; Demo reads its own model/skill assets; general character/building/ground profiles contain models only. TASK-020 supplies future production card definitions. Model Actors can be rebuilt and their pointers are not save records; static preview does not execute garrison assignment or movement.
