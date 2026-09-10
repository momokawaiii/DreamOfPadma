# Project State

- Chinese companion: ProjectState.zh-CN.md
- Updated: 2026-09-10; current facts only. Check Git and affected files before acting.
- Workspace: E:/2026ue/DreamOfPadma; current Local branch main at MVP baseline 00b894d. Keep future Demo rewrite work on a separate Local branch.

## Current priority

The next product delivery is an installable offline Chapter Zero Demo: real core operations, Encounter then one ACT at separate nodes, and royal-court ending. Both Development and Shipping must be tested. Final path values must come from approved production data; the card document is pending. See [ChapterZero](Rules/ChapterZero.md) and [Roadmap](Production/Roadmap.md).

[TASK-053](Production/Tasks/TASK-053-Grill-Technical-Docs.md) reconciles the grill decisions into concise technical owners; documentation checks and focused independent review passed. The target adds editor-baked map/manifest identity, chapter runtime, saved stage-story assignment and tutorial skip/replay reward identity. These are not implemented by this documentation change. The Slate story editor is deferred; current data/table workflows remain available.

Current native work remains TASK-052's verified working-tree candidate: 55-cell painted tutorial, click-pinned details, A/B confirmation, vertical parchment hand, map navigation, liquid war UI and PIE-exit fixes. The separate continuous-terrain/PCG sample remains available. User art/physical long-hold acceptance and packaged behavior remain unverified; evidence stays in the individual TASKs.

The current MVP snapshot was committed as `00b894d` and fast-forwarded into local `main` on 2026-09-10. Keep `main` as this baseline while Demo rewrite work proceeds on a new branch.

The user reports prior Blueprint, DataAsset/DataTable/soft-reference, UMG/CommonUI basics and GAS study. Teach only when requested; do not repeat onboarding.

## Delivery state

| Area | Current result |
|---|---|
| UE TASK-052 | Working-tree verified candidate: click-pinned details, explicit A/B placement, five-column vertical parchment hand, 55-cell new tutorial, single-owner painted-map input and 50:50 liquid motion UI; build, targeted regression and native evidence in TASK-052 |
| UE TASK-051 | Fixed tutorial painting and GPU atmosphere retained; version-1 162-cell saves remain supported alongside TASK-052's new grid. Earlier checks remain in TASK-051 |
| UE TASK-007 / 008 | Historical fixed travel/summon chain retired in TASK-048; old assets, source and fixture tests removed |
| UE TASK-036 / 040 | ACT authoring templates and reusable model catalog/preview retained; garrison preview migrated to a general model asset |
| HTML TASK-042 / 043 / 044 | HTML 0.4 accepted by user after iteration; older manual-pending notes in reports are historical |
| UE TASK-046 | Native working-tree candidate: calendar/resources/ABC/synthesis/graph/preparation/dialogue, Encounter and ACT GAS, transactions and safe saves; Editor build and 11 focused tests passed; real map/home/two-mode entry rendering passed |
| UE TASK-047 | Runtime naming migrated; Editor build and 12 focused tests passed, including legacy save reading. All 18 table contents preserved, native type tags and two map references re-saved; real world/battle/rollback/world route passed |
| UE TASK-048 | Working-tree candidate: editable map kit, fixed anchors, arrival checkpoints, seeded branches, 3D camera/terrain/roads/models and PCG; Editor build and 22 focused tests passed; PCG generated 200 instances. User interaction acceptance pending |
| UE TASK-049 / 050 | Reference-inspired strategy HUD, continuous terrain sample, 500 surface-filtered PCG instances, 7 sample meshes/18 materials/3 landmark definitions and 2 UI skin textures. Real activatable stacks, nested Back/focus and readonly instance/battle inspection; 26 focused tests passed. User playtest/art acceptance pending |
| Beyond accepted HTML | Formal ACT actions/combos, unsupported PDF abilities, relationship buffs, FPS gameplay and production art remain |
| Workflow TASK-045 | Verified: concise routing, scoped delegation, risk-based review and optional teaching |

Current interaction, motion and grid-compatibility evidence is in TASK-052; accepted gameplay evidence remains in TASK-046 and naming compatibility in TASK-047. User art/physical hold acceptance and packaged-build compatibility remain unverified. Earlier evidence stays in its original task. The playable MVP snapshot is integrated in local `main`; remote synchronization is handled by this delivery.

## Active boundaries

- HTML supplies accepted implemented behavior, not proof that the entire PDF is implemented. Use Docs/Rules and the latest user decisions for semantics; missing formal ACT content, progression and relationship buffs remain explicit.
- Encounter and ACT use separate GAS execution/configuration and separate character collections. Basic skills share identity/slots with separately authored mode effects. ACT equipment comes from battle settings; terrain restrictions name their target.
- Ordinary SLG maps use directed authored nodes/edges over a hex appearance. The fixed painting tutorial explicitly opts into generated hex adjacency with seven painting-specific key positions; new version-3 grids have 55 cells and one enemy guard at fire and saved version-1 grids retain 162. Impassable cells and authored road locks remain enforced. SaveGame freezes layout and mutable/story state against trusted configuration. Click pins inspection; Move Here moves deployed cards. A/B placement captures a target and revalidates the live Core command on confirmation. Enemy information is public.
- A with attached B moves atomically with each card validated; movement supports movable/immovable/conditional metadata.
- Synthesis inputs discard on success/failure; successful C appears at the current node. B accumulates A tendencies at noon. Shared basic cards have an independent discard/recycle pile.
- Encounter ordinary attack arms target selection and allows cancellation. Basic hand cards preview on drop, confirm to play, and have a card/environment source. True damage ignores C resistance; physical basic damage does not.
- Local battle time freezes the sandbox. Success commits; defeat/exit restores the complete snapshot. Current two victory routes are ruler core and war balance.
- UI pages and modal overlays use separate activation stacks. Only the top overlay accepts input; details never settle gameplay. Painted-map Slate owns its navigation through the CommonUI page and handles clicks/middle drag/wheel; cancellation clears its keys and capture. The controller does not also poll painted WASD; the legacy 3D camera remains controller-owned. Encounter inspection waits for enemy autoplay, while committed actions can finish. ACT inspection does not change its clock or Tab policy. Terrain and PCG are cosmetic; the graph remains authoritative.
- Current prototypes may use placeholders and labelled temporary UE Demo numbers; clear non-conflicting mycard values retain provenance. The final offline Demo path requires approved production values/assets under ChapterZero; missing card/reward inputs remain unresolved.
- One UE build/Editor/PIE lane; keep current Local work and unrelated changes. Task status never authorizes Git integration or remote operations.

## Next entry points

- Native UE play route, exact source map and art/table authoring: [NativePlayableDemo](Content/NativePlayableDemo.md), [WorldMapAuthoring](Content/WorldMapAuthoring.md), [StrategyPresentation](Content/StrategyPresentation.md). Default map is `/Game/Padma/MVP/Playable/Maps/L_PadmaWorld`; author positions in `/Game/Padma/World/Maps/Editing/L_MapAuthoring`.
- Workflow and compact delegation: [Agent Workflow](Agent/Workflow.md).
- UE model/ACT preparation: [ModelAuthoring](Content/ModelAuthoring.md), [ACTAuthoring](Content/ACTAuthoring.md).
- UE task dependencies: [TASK-011 matrix](Production/Tasks/TASK-011-Full-MVP-Task-Cluster.md). Reconcile old TASK-009/010 fixture scope with accepted HTML when activating that delivery; do not create a redundant old UI.
- Relevant unresolved decisions: [MVPDecisionRegister](Production/MVPDecisionRegister.md); use only entries affecting the chosen slice. Latest user decisions override stale generic backlog prose.
- Historical evidence and earlier decisions: [state snapshot](Production/History/ProjectState-2026-09-09.md), [Changelog](Changelog.md), exact TASK reports. Do not load history for routine startup.
