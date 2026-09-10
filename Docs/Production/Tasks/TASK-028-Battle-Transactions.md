# TASK-028 Battle Snapshot Transactions

- Chinese companion: TASK-028-Battle-Transactions.zh-CN.md
- Document ID: TASK-028
- Version: 0.1
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: Game Transactions
- Primary Agent: one designated owner at Ready; no writer started
- Branch/worktree: assign a Local feature branch at implementation preflight; no worktree by default

## Goal and why now

Capture the complete versioned pre-battle run state, pause sandbox time, commit success once and restore exactly on local defeat, exit or failed entry. This extends the reviewed predecessors through one bounded delivery result. It must not turn their temporary fixtures into unapproved game rules.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, [TASK-011](TASK-011-Full-MVP-Task-Cluster.md), [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), [MVP decisions](../MVPDecisionRegister.md), and the [PadmaGame contract](../../Architecture/Modules/PadmaGame/README.md). Also read `Docs/Architecture/SaveSchema.md`, `Docs/Rules/Combat.md`, the predecessor contracts, and nested Source/Content/Docs instructions for the paths below.

Source: [PDF card, rule and screen extraction](../MVPSourceExtraction.md). Old contract-card sandbox summon effects are excluded from the first version; missing ACT definitions are not filled by converting turn-based cards.

## Decisions and readiness

- Frozen: the accepted MVP/rules, separate Encounter/ACT GAS systems and tables, separate turn-based/ACT character-card collections, and shared basic-skill identity/slot mapping with mode-owned effects.
- Frozen: placeholder assets will be imported by the user; final animation/rendering polish is later work. Every unaccepted numerical value requires individual user confirmation before use, including test profiles.
- Gate references: D04, D15 See the register for the owner and exact missing information.
- Backlog describes an approved planning item, not a runnable implementation task. Record gate answers in the canonical rule/architecture source and this report, complete predecessors, then promote to Ready. Technical schemas are reviewed by the Architect; gameplay and numerical choices belong to the user.
- If gate closure requires a normative pair or structural ADR outside this write set, the Coordinator must add those exact paths before Ready; this is planned preflight, not permission for arbitrary edits.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/Game/BattleTransactions/`
- `Source/DreamOfPadma/Private/MVP/Game/BattleTransactions/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-028/`
- `Content/Padma/MVP/Tests/BattleTransactions/`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-028-Battle-Transactions.md`
- `Docs/Production/Tasks/TASK-028-Battle-Transactions.zh-CN.md`

Exclusions: Partial rollback, saving Actor/GAS handles, disk persistence and assuming every local defeat is campaign defeat. Other task source/content, shared configuration not named above, the reference prototype, generated folders and new standalone UE modules are outside scope. Existing Demo code is consumed through public contracts; adaptation/migration needs an explicitly named write set.

## Dependencies, handoff and conflicts

- Required predecessors: [TASK-005](TASK-005-Deterministic-Random-Foundation.md), [TASK-018](TASK-018-Calendar-and-Phase-Scheduler.md), [TASK-019](TASK-019-Resource-and-Global-Ledgers.md), [TASK-020](TASK-020-Card-Catalog-and-Lifecycle.md), [TASK-021](TASK-021-ABC-Sandbox-Operations.md), [TASK-022](TASK-022-Synthesis-Resolution.md), [TASK-024](TASK-024-Sandbox-Graph-and-Movement.md), [TASK-025](TASK-025-Node-Operations-and-Story.md), [TASK-026](TASK-026-Ruler-World-AI.md), [TASK-027](TASK-027-Battle-Routing-and-Travel.md), [TASK-035](TASK-035-ACT-Character-Cards-and-Roster.md).
- Conditional predecessor: TASK-005 before any random outcome where it is not already listed.
- Hand off the reviewed public commands, read-only views, definition fixtures and test evidence. A state owner implements TASK-012 snapshot participation; UI never owns mutable gameplay state.
- Module README pairs, shared headers, maps, binary assets and central files are serial leases. Do not edit while another owner/build consumes them. One UE build/Editor/PIE lane.
- No writable subagent is authorized. One bounded read-only architect/reviewer may inspect the relevant contract/diff. The Primary Agent remains accountable; the Coordinator owns shared integration state and Changelog.

## Acceptance criteria

- [ ] Reject battle entry if a mutating participant lacks a supported snapshot adapter; enumerate calendar, resources/debt, cards, world, story/history, AI work and RNG positions.
- [ ] Prove success commits once and defeat/exit restores every participant, including card lifecycle, pending events, entropy, faith, threat and war balance. Include both character collections, shared skill-card state and selected character/weapon state to the extent D20 classifies them as battle-mutated run state; keep any global preset outside that classification unless explicitly decided otherwise.
- [ ] Entry/load/cancel failures leave a coherent pre-battle state; late callbacks and duplicate completion are rejected by battle instance/transaction identity.
- [ ] Restore closes mode abilities/effects/input and reinstates authoritative values before returning control; ACT subsequently plugs its own participant/cleanup into this same outer transaction.
- [ ] Exact write set, synchronized task/module documentation, required checks and independent review pass; no unresolved rule has been filled by a default.

## Verification and evidence

Whole-state equality/hash and stream-position comparisons across success/defeat/exit, failed entry and duplicate result cases; user observes resource/card/world restoration after retreat.

Compile the Editor target when native code changes. For the implemented narrow automation, use Scripts/RunTests.ps1 with the verified EngineRoot and TestFilter `DreamOfPadma.MVP.TASK028`; confirm reported cases actually passed, not only process exit. Run Scripts/AuditDocs.ps1, applicable Scripts/ValidateProject.ps1 -Strict and scoped git diff --check. Do not add broad UI automation merely for this task. Record reproducible user PIE observations (written evidence is acceptable), input/profile IDs and logs; images/video may supplement them. No runtime/manual success is claimed by this contract.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| Transactions across state owners | Predict all changes that retreat must undo, compare snapshots and explain one easily missed participant. | Not started |

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
