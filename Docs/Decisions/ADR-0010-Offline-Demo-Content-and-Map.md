# ADR-0010: Offline Demo Content and Baked Map Boundary

- Document ID: ADR-0010
- Version: 1.0
- Status: Accepted target; implementation pending
- Date: 2026-09-10
- Chinese companion: [ADR-0010-Offline-Demo-Content-and-Map.zh-CN.md](ADR-0010-Offline-Demo-Content-and-Map.zh-CN.md)
- Decision owner: User for scope/semantics; Architect for technical mapping
- Source/evidence: [TASK-053](../Production/Tasks/TASK-053-Grill-Technical-Docs.md)

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

Rendering choice/migration from the current painting, concrete main-menu/ending content, final card/reward values, replay inventory ownership, failure resume policy and measurable visual/performance acceptance remain implementation or user content decisions. Do not convert these into new approved defaults.
