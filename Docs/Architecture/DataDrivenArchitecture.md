# Data-Driven Architecture

- Document ID: ARCH-DATA-001
- Version: 0.4
- Status: Current catalog plus accepted Demo target; target schemas are not implemented APIs
- Chinese companion: [DataDrivenArchitecture.zh-CN.md](DataDrivenArchitecture.zh-CN.md)
- Owner: Lead Programmer / Architect
- Authority: [ProgramArchitecture](ProgramArchitecture.md), [ADR-0010](../Decisions/ADR-0010-Offline-Demo-Content-and-Map.md)

## Current implementation

`UPadmaContentCatalog` references typed soft DataTable shards and presentation/ACT assets. Game validates and resolves them into `FPadmaContentSnapshot`; Core consumes values. Duplicate identities reject loading. The current entry loads synchronously; soft references and UPrimaryDataAsset inheritance are not evidence of an asynchronous Asset Manager pipeline.

Current concrete contracts: [PadmaContentTypes.h](../../Source/DreamOfPadma/Public/Core/Content/PadmaContentTypes.h), [PadmaContentCatalog.h](../../Source/DreamOfPadma/Public/Game/Content/PadmaContentCatalog.h), [PadmaRunRules.h](../../Source/DreamOfPadma/Public/Core/Run/PadmaRunRules.h). The native catalog has dialogue/checkpoint bindings, but not the unified chapter runtime, phase story pool, tutorial run/reward ledger or map manifest described below.

## Data layers and field authority

| Layer | Owns | Never owns |
|---|---|---|
| Definition | Rule values, IDs, initial setup, content references | Current HP, ownership, reward receipts |
| Generated result | Frozen nodes/coordinates/terrain/edges/anchors/locks plus map identity | Mutable occupation, progression or live Actors |
| Run state | Current cards/resources/ownership, flags, story decisions and RNG progress | Editable source graphs or art assets |
| Presentation | Images/models/materials/motion/cues selected by stable binding | Damage, reward or branch settlement |
| Authoring tool | Validated edits/imports/previews of the above definitions | A second independent copy of rules |

A field has one source. For example, card base statistics live in its rule definition; a presentation asset binds art and cannot override attack. Generated output is a frozen derivative, not an independently editable source. Existing map checkpoint-table import explicitly updates the template; table edits alone do not change a live map.

## Asset choice

| Carrier | Use | Project constraint |
|---|---|---|
| Typed DataTable | Comparable rows: card values, dialogue structure, pools, costs and thresholds | Stable row/definition IDs; explicit units and mode |
| DataAsset | Nested chapter configuration and cohesive presentation references | Prefer a simple asset where managed discovery/loading is unnecessary |
| PrimaryDataAsset | Existing catalogs/ACT/model definitions or a justified Asset Manager identity | Do not convert current assets merely for naming consistency |
| Curve / CurveTable | A named continuous relationship with units | Does not replace an unconfirmed formula |
| StringTable | Localized text behind TextId | Accepted story target; current direct text needs explicit migration |
| GameplayTag | Registered categories, permissions and query vocabulary | Not numerical storage or executable behavior |
| SaveGame | Versioned run values and compatibility metadata | Not a content-authoring database |

Current synthesis semantic tags are weighted `TMap<FName, int32>` values, not a GameplayTagContainer. Replacing them with unweighted tags would change the rule. Fixed condition/effect registration is accepted; using GameplayTags as its type IDs was proposed in unanswered Q41 and is not frozen.

## Accepted chapter target

The following are conceptual minimum contracts. Implement with existing typed asset/table workflows first; final C++ layouts and migrations belong to a scoped implementation.

| Contract | Minimum information | Consumer |
|---|---|---|
| ChapterDefinition | ChapterId, ChapterContentVersion, map binding, entry/end references, story event index, tutorial steps, pool and presentation bindings | Game chapter orchestration |
| StoryEvent / StoryBeat | Stable EventId, track, trigger, typed conditions, choices/effects, continuation IDs, repeat/cooldown policy, CueId | Core/Game story execution |
| TutorialStep | StepId, required player operation, completion condition, progression/unlock references | Tutorial runtime; completion must represent a real operation |
| Dialogue row | DialogueId, speaker, TextId, choice IDs and dialogue-local transitions | Dialogue runtime/UI |
| StoryPool | PoolId/version, eligibility period/conditions, weighted EventId candidates, repeat policy and explicit selection priority | Stage assignment service |
| PresentationProfile / Cue | CueId; dialogue/portrait/sequence/audio/Niagara/action bindings; input/skip policy | UI/Presentation |
| Condition / Effect entry | Registered type plus validated, type-specific parameters | Authoritative services; no arbitrary Blueprint function dispatch |

Node type (ordinary/camp/furnace/anchor), story track (main/side/NPC/random), trigger, pool policy and presentation are independent dimensions. Do not encode them as one mutually exclusive enum. The suggested main > NPC > side > random priority is not a universal finalized rule; author the applicable ordering when content conflicts arise.

Keep execution ownership unambiguous: dialogue rows own dialogue-local transitions; event continuation owns transitions between events. A choice result crosses that boundary once. LevelSequence and a future editor graph do not duplicate rule effects.

The fixed registry and presentation separation are accepted in Q36/Q38. Detailed parameter encoding, cross-event atomic transaction design and exact tutorial result-event vocabulary from Q39–Q45 remain design proposals. Implementers must preserve existing command/rollback invariants without claiming those unanswered designs were approved.

## Map definition target

| Contract | Fields / responsibility |
|---|---|
| MapKey | ChapterId + Seed + GeneratorVersion |
| GeneratorManifest | Immutable version identity covering terrain algorithm, PCG graph, theme/biome, map configuration and generated schema |
| Generation input | MapKey, fixed gameplay anchors/explicit edges/locks, terrain settings and independent decoration seed |
| Frozen map result | MapKey, stable nodes, coordinates, terrain/elevation/water data needed by rules, edges and checkpoints |
| Baked presentation metadata | Matching MapKey and cooked terrain/theme/decoration asset bindings |

A manifest change creates a new GeneratorVersion; never mutate an old manifest in place. Dialogue edits change ChapterContentVersion, not MapKey, unless they also change map configuration/locks. Current integer generator versions and `MapId` are not automatically the new manifest or ChapterId.

Generation, baking and validation order is owned by [WorldMapAuthoring](../Content/WorldMapAuthoring.md). Saved fields, compatibility and trusted lookup are owned by [SaveSchema](SaveSchema.md). Gameplay topology remains authoritative; PCG cannot move anchors, add edges or consume gameplay RNG.

## Existing mode and presentation contracts retained

- [ADR-0004](../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md): Encounter and ACT have separate rows, validators, execution and clocks. Turn-based instances, ACT character/weapon roster and shared basic-skill identity are distinct. Trait-only restrictions do not become character bans.
- [ACTAuthoring](../Content/ACTAuthoring.md): ACT character model/AnimClass/typed skill-table bindings, weapon presentation and same-skeleton validation. An authored executor ID alone does not prove a runtime ability exists.
- [ModelAuthoring](../Content/ModelAuthoring.md): one source per domain-qualified presentation key; generic model assets contain visual references/transforms. Mobility metadata is a Core value and runtime permission still requires rule validation. Visual and skill-detail readiness are separate.
- [Combat](../Rules/Combat.md), [Synthesis](../Rules/Synthesis.md), [Time](../Rules/Time.md), [WorldState](../Rules/WorldState.md) own rule meaning. Do not copy formulas, card allowance rules or the full unresolved numerical register into architecture.

## Loading, validation and authoring tools

`Authored definition → validation → typed snapshot → service → result/state → presentation`

Validate unique IDs, references, units/ranges, mode/row compatibility, curve domains, map connectivity/locks and known effect types. Report asset/field/reason. Missing final values are unconfigured, not zero. Calculation previews use execution's evaluator without state changes or RNG consumption; candidate and success rolls remain distinct.

For the target, also validate MapKey-to-bake consistency, chapter/map node references, stage pool eligibility, dialogue/TextId references and tutorial reward identities. These are implementation acceptance needs, not a claim that a complete cook gate exists. Scope unresolved validators from Q45 when that implementation is authorized.

The user will provide final card data. Existing labelled prototype numbers may support development but cannot satisfy final Demo content acceptance. Content receipts should retain source/field/unit/mode/version; no new numerical defaults are approved here.

Slate story authoring, UEdGraph source assets, graph compilation, text-entry/localization tools and Cue editing are **deferred**. Preserve the accepted future separation `ChapterSourceAsset → compiled ChapterDefinition`, with a restricted DAG, typed execution edges and no editor objects in Shipping. Existing Python authoring and map previews remain usable; a universal one-click content tool is not a prerequisite.
