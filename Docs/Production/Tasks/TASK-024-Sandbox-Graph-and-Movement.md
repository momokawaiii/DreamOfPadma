# TASK-024 Sandbox Graph and Movement

- Chinese companion: TASK-024-Sandbox-Graph-and-Movement.zh-CN.md
- Document ID: TASK-024
- Version: 0.2
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: World Graph
- Primary Agent: one designated owner at Ready; no writer started
- Branch/worktree: assign a Local feature branch at implementation preflight; no worktree by default

## Goal and why now

Build a small shared sandbox with stable node topology, player/ruler positions, selection, discovery and legal resource-priced movement. This extends the reviewed predecessors through one bounded delivery result. It must not turn their temporary fixtures into unapproved game rules.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, [TASK-011](TASK-011-Full-MVP-Task-Cluster.md), [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), [MVP decisions](../MVPDecisionRegister.md), and the [PadmaWorld contract](../../Architecture/Modules/PadmaWorld/README.md). Also read `Docs/Rules/WorldState.md`, `Docs/Design/EN/04_LevelContentPlanner.md`, the predecessor contracts, and nested Source/Content/Docs instructions for the paths below.

Source: [PDF card, rule and screen extraction](../MVPSourceExtraction.md). Old contract-card sandbox summon effects are excluded from the first version; missing ACT definitions are not filled by converting turn-based cards.

## Decisions and readiness

- Frozen: the accepted MVP/rules, separate Encounter/ACT GAS systems and tables, separate turn-based/ACT character-card collections, and shared basic-skill identity/slot mapping with mode-owned effects.
- Frozen: placeholder assets will be imported by the user; final animation/rendering polish is later work. Every unaccepted numerical value requires individual user confirmation before use, including test profiles.
- Gate references: D03, D12, D21 See the register for the owner and exact missing information.
- Backlog describes an approved planning item, not a runnable implementation task. Record gate answers in the canonical rule/architecture source and this report, complete predecessors, then promote to Ready. Technical schemas are reviewed by the Architect; gameplay and numerical choices belong to the user.
- If gate closure requires a normative pair or structural ADR outside this write set, the Coordinator must add those exact paths before Ready; this is planned preflight, not permission for arbitrary edits.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/World/Graph/`
- `Source/DreamOfPadma/Private/MVP/World/Graph/`
- `Source/DreamOfPadma/Public/MVP/World/Movement/`
- `Source/DreamOfPadma/Private/MVP/World/Movement/`
- `Source/DreamOfPadma/Public/MVP/UI/WorldMap/`
- `Source/DreamOfPadma/Private/MVP/UI/WorldMap/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-024/`
- `Content/Padma/MVP/World/GraphFixture/`
- `Content/Padma/MVP/World/Maps/`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaWorld/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-024-Sandbox-Graph-and-Movement.md`
- `Docs/Production/Tasks/TASK-024-Sandbox-Graph-and-Movement.zh-CN.md`

Exclusions: World Partition/PCG production systems, hidden hard-coded map identity and inventing per-card movement conditions/costs. Other task source/content, shared configuration not named above, the reference prototype, generated folders and new standalone UE modules are outside scope. Existing Demo code is consumed through public contracts; adaptation/migration needs an explicitly named write set.

## Dependencies, handoff and conflicts

- Required predecessors: [TASK-012](TASK-012-Run-State-and-Contracts.md), [TASK-019](TASK-019-Resource-and-Global-Ledgers.md), [TASK-020](TASK-020-Card-Catalog-and-Lifecycle.md).
- Conditional predecessor: TASK-005 before any random outcome where it is not already listed.
- Hand off the reviewed public commands, read-only views, definition fixtures and test evidence. A state owner implements TASK-012 snapshot participation; UI never owns mutable gameplay state.
- Module README pairs, shared headers, maps, binary assets and central files are serial leases. Do not edit while another owner/build consumes them. One UE build/Editor/PIE lane.
- No writable subagent is authorized. One bounded read-only architect/reviewer may inspect the relevant contract/diff. The Primary Agent remains accountable; the Coordinator owns shared integration state and Changelog.

## Acceptance criteria

- [ ] Use the confirmed oblique SLG camera with nodes/preset connections over hex-shaped presentation, never implicit hex adjacency. Map definition asset, .umap presentation and SaveGame run changes follow ADR-0005; explicit edges are topology truth.
- [ ] A compact approximately eight-node fixture exposes stable IDs independently of display coordinates, with player/ruler territory and named node roles.
- [ ] Movement validates adjacency, discovery, unit/category restrictions and the approved price; failure changes neither position nor payment.
- [ ] Selection/movement is visible; enemy occupancy and terrain query contracts expose data for later battle routing without choosing a mode here.
- [ ] World identity, discovery, location and ownership export/restore through the participant contract; rebuilding visuals does not reset authoritative state.
- [ ] Exact write set, synchronized task/module documentation, required checks and independent review pass; no unresolved rule has been filled by a default.

## Verification and evidence

Graph validity, illegal move/no-payment, display-coordinate invariance and state restoration tests; user moves between legal nodes and attempts an illegal route.

Compile the Editor target when native code changes. For the implemented narrow automation, use Scripts/RunTests.ps1 with the verified EngineRoot and TestFilter `DreamOfPadma.MVP.TASK024`; confirm reported cases actually passed, not only process exit. Run Scripts/AuditDocs.ps1, applicable Scripts/ValidateProject.ps1 -Strict and scoped git diff --check. Do not add broad UI automation merely for this task. Record reproducible user PIE observations (written evidence is acceptable), input/profile IDs and logs; images/video may supplement them. No runtime/manual success is claimed by this contract.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| Topology versus map presentation | Move a visual node without changing its ID and predict whether connectivity or saves should change. | Not started |

Practice: the user performs the smallest observation/edit above and explains one failure case. Transfer: explain how the same ownership principle applies to the next dependent task. Agent artifacts and user practice are separate evidence.

## Risks and recovery

Risk: mode coupling, missing lifecycle/snapshot participation or scope drift. Stop before an unresolved decision or conflicting writer; preserve the evidence. Recover with a reviewed revert of this task's own change, never by deleting generated or unrelated content. Final integration returns feature defects to this owner.

## Completion report

- Final status: Backlog; contract created, implementation not started.
- Changed implementation files, checks, manual observations and review: pending.
- Agent-produced evidence: this contract only; no runtime or user learning claim.
- User-produced learning evidence: pending.
- Changelog draft: required at implementation handoff; category, visible result, validation and remaining limits.
- English/Chinese pair: created together; update together in every future change.
- Integration/commit: assigned at implementation; no merge/push authorization.

## Inspection provider supplement

D24/D25 are accepted. TASK-037 owns detailed hover/local-scene UI; this task supplies a versioned node/entity/card-instance view with full first-MVP friendly/enemy disclosure: terrain, node/controller, deployments, A/B attachment, facilities and paths. Missing data is distinct from empty. Click only inspects; the separate right-side Move Here command requests card movement. Inspection queries never submit movement/payment. Layout/node count remains user-authored, not inferred from a visual mockup.

## 2026-09-09 confirmed model/movement handoff

D24/D25 are answered: inspection-only click, a separate Move Here command for selected card instances, and full first-MVP enemy node disclosure. D29 adds three-state per-card metadata, explicit unconfigured rejection and A/attached-B atomic groups. Validate every member, ownership/source/version, authored route/condition/payment before mutation; unknown conditions reject without charge. Full disclosure is the current view policy, not permission to alter ruler AI rules.
