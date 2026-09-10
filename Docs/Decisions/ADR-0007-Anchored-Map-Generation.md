# ADR-0007: Anchored Map Generation and Frozen Run Layouts

- Chinese companion: ADR-0007-Anchored-Map-Generation.zh-CN.md
- Document ID: ADR-0007
- Version: 1.0
- Status: Accepted; implementation TASK-048
- Date: 2026-09-09

## Decision

The user's confirmed anchor definition supersedes the earlier suggestion of mandatory visits. FixedNPC and MainStory anchors retain identity and coordinates across seeds. FixedNPC provides a consistently located character. MainStory encounters/completion affect later character-related branches. Detours are valid. An authored checkpoint records its matched/missed result once on arrival, using Seen or Completed as configured. It does not reevaluate later or impose a universal victory gate. Node dialogues retain the existing noon operation restriction.

`UPadmaWorldMapAsset` owns an authored Template, optional GeneratedLayout and reproducible generator settings/seed. All connections are explicit directed edges. A finite, seeded generator adds branches to valid existing route corridors, preserving authored nodes and anchors. Validation checks IDs, finite coordinates, DAG reachability, player-owned home, flag producers and impossible locked prerequisites. Possible alternate checkpoint branches are considered independently; validation is not a balance proof or a solver proving every branch can win.

Checkpoint DataTables are explicit authoring imports into Template. Runtime reads only the selected frozen layout; editing a source table does not silently mutate a live map. Import switches back to Template and requires deliberate regeneration. An editor preview provides transient node handles; applying transforms validates before changing the asset. Editing Template is the designer's explicit authority to reposition anchors; editing generated output cannot reposition them.

Core owns value schemas, generation, commands, story state and rollback. Game owns Data Asset loading, save boundaries, input and the editor authoring bridge. World owns hex geometry, visual proxies, model references and PCG. UI renders command options and submits intents. These are folders within the current module, not gratuitous new modules or a universal manager.

UE's PCG plugin generates cosmetic static mesh instances from a graph created by the World adapter. Its random stream is separate from topology and combat outcomes. It avoids nodes and roads and never supplies traversability, NPC identity or story state. Visual theme fields expose meshes, materials, density/scale and node model soft references. A custom standalone plugin, World Partition and symmetric competitive balance are not requirements of this delivery.

## Save amendment to ADR-0005

Procedural maps require a frozen **value layout** in each run: MapId/version/generator version/seed, node definitions, directed edges and checkpoints. This intentionally supersedes ADR-0005's earlier prohibition on duplicating static topology in saves. It is a run snapshot, not a second independently mutable authority. Only stable IDs and values are serialized; no Actor or loaded asset pointers. Restoring a save rebuilds the active content view from its layout and validates compatibility against the independently retained configured anchor reference before changing active state. Legacy saves without a layout use the original table graph, never the current generated graph.

Seen/Completed NPC identities, resolved checkpoint decisions, ownership, used/visited nodes and unlock flags belong to the same snapshot. Failed battle travel restores all of them. Regenerating a new run cannot change an old save's geometry or branch history. Changing anchor identity/position or map compatibility version may reject prior saves; artists can change presentation without changing topology.

## Evidence and limits

Build, automation, asset migration, PCG output and native rendering evidence are recorded in [TASK-048](../Production/Tasks/TASK-048-Anchored-World-Maps.md). Interaction acceptance remains the user's playtest. The initial chapter is a small authored graph plus optional branches, with placeholder art and temporary dialogue content. Full authored story production, landscape sculpting, free hex adjacency and symmetric competitive generation are separate scope.
