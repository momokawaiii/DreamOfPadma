# ADR-0010: Offline Demo Content and Baked Map Boundary

- Document ID: ADR-0010
- Version: 1.1
- Status: Accepted target; implementation pending
- Date: 2026-09-10
- Chinese companion: [ADR-0010-Offline-Demo-Content-and-Map.zh-CN.md](ADR-0010-Offline-Demo-Content-and-Map.zh-CN.md)
- Decision owner: User for scope/semantics; Architect for technical mapping
- Source/evidence: [TASK-053](../Production/ProjectCleanup.md)

## Context and precedence

The current native subset, painted tutorial and live 3D/PCG sample are working prototypes. The latest user decisions target an installable offline Chapter Zero Demo and defer the custom story editor. [ChapterZero](../Rules/ChapterZero.md) owns the accepted experience.

This supersedes the old roadmap's “packaging after complete MVP” priority. It does not retire current maps, rewrite serialized IDs or mark existing prototype values as production. ADR-0004's mode separation and ADR-0008's activation boundary remain. ADR-0007/0009 remain the current map/save contracts until a reviewed implementation migrates them.

## Accepted decisions

1. Chapter content, generated map results and mutable run state have separate ownership. ChapterDefinition coordinates typed dialogue/rule tables and presentation references.
2. Chapter Zero uses fixed Seed, fixed anchors and explicit edges/locks. Generate terrain and cosmetic PCG in the editor, validate and bake for both Development and Shipping; packaged play loads the result. Q21 supersedes Q16's earlier mixed runtime-generation option.
3. MapKey is ChapterId + Seed + GeneratorVersion. GeneratorVersion refers to an immutable composite manifest. Both baked metadata and the save retain MapKey; content versioning is independent.
4. Persist frozen gameplay map values and mutable state. Loading never silently regenerates a different layout. PCG cannot change gameplay edges/anchors/locks or consume gameplay randomness.
5. Runtime story follows trigger/condition/event/choice/effect/state separation. Persist stage story assignments and locked checkpoint decisions; a save load does not redraw.
6. Tutorial completion/skipping/replay uses distinct status and per-run reward identity. New replay can reward again; same-run retry cannot.
7. Conditions/effects use fixed registered types. Dialogue structure uses DataTable and text uses StringTable in the target. PresentationProfile/Cue binds playback; LevelSequence cannot settle rewards/flags/branches.
8. The accepted future story source asset is separate from runtime ChapterDefinition; a restricted typed DAG compiles out of editor data. Slate/UEdGraph tools, graph compilation and associated authoring UI are deferred, not a Demo prerequisite.

Map-generation and decoration randomness are separate from gameplay; the detailed story/combat/synthesis stream partition is an implementation design requirement, not an already-migrated RNG schema.

## Single owners and consequences

| Contract | Owner document |
|---|---|
| Data layers, fields and source-of-truth choices | [DataDrivenArchitecture](../Architecture/DataDrivenArchitecture.md) |
| Story/tutorial execution and mode transitions | [RuntimeFlow](../Architecture/RuntimeFlow.md) |
| MapKey, assignments, reward receipts and migration | [SaveSchema](../Architecture/SaveSchema.md) |
| Generate/validate/freeze/bake procedure | [WorldMapAuthoring](../Content/WorldMapAuthoring.md) |
| Work order and dual-build acceptance | [Roadmap](../Production/Roadmap.md), [BuildMatrix](../Production/BuildMatrix.md) |

No source, asset or save format is changed by this ADR. Later implementation must address trusted manifest lookup, baked assets/cook inclusion, legacy compatibility, production card input and recovery evidence. Existing authoring buttons do not yet implement this complete pipeline.

## Open decisions preserved

Q39–Q45 have no user answers: special-node dispatch mechanics, reusable subgraph organization, exact parameter/tag encoding and atomic effect design, tutorial event vocabulary, detailed card-reference authoring, sequence-binding implementation and exhaustive Shipping compiler gates remain proposals. Previously accepted runtime/presentation separation still applies.

Q47 resolves the painting-versus-3D choice: the final map is baked 3D terrain. Its concrete representation/bake API, main-menu/ending content, final card/reward values, replay inventory ownership and failure resume policy remain implementation or user content decisions. Q85 accepts a generous development-machine baseline; exact measured budgets are still unset.

## Q46–Q90 amendment

The later answers refine this ADR without changing current saves or assets:

| Decision | Accepted boundary / owner |
|---|---|
| Q47, Q50, Q56 | Baked 3D final map; typed hand-authored chapter data now, future editor emits the same runtime IDs/contracts. |
| Q75, Q81, Q82, Q89–Q90 | Reverse-engineer Combat behavior and rebuild native Padma GAS; one complete representative ACT P0, with model/retarget/3C as the first learning task only. See [ACTDevelopmentContract](../Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md). |
| Q72, Q78, Q84 | Full UI redesign allowed; preserve domain state/command/result semantics, not current widget hierarchy/gestures. User supplies the full interaction specification. |
| Q61, Q69 | Local authority only; networking remains deferred. |
| Q70, Q77, Q83, Q88 | No custom replacement tool; manual asset replacement plus provenance/reference/cook audit. Public/Steam-target packages reject civili/known prohibited sources even through renames; private learning packages are distinct. |
| Q71, Q87 | Audio may be absent/placeholder during development; production audio is an RC gate. Voice acting is optional. |
| Q73, Q79, Q85–Q86 | Generous measured development-machine baseline and Editor/PIE, Development, Shipping, clean offline installation evidence; no invented numerical budget. |
| Q65, Q80 | Production work is split into one learnable unit plus a concrete result and evidence; do not restart completed basics. |

[DemoDeliveryPlan](../Production/DemoDeliveryPlan.md) sequences these requirements; [ACTMigrationMatrix](../Content/ACTMigrationMatrix.md) distinguishes requested priority from independently verified playable promotion. Q39–Q45's unanswered implementation details remain open where not superseded by the later decisions above.
