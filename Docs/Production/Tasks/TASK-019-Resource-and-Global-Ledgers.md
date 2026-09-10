# TASK-019 Resource and Global Ledgers

- Chinese companion: TASK-019-Resource-and-Global-Ledgers.zh-CN.md
- Document ID: TASK-019
- Version: 0.1
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: Core Resources
- Primary Agent: one designated owner at Ready; no writer started
- Branch/worktree: assign a Local feature branch at implementation preflight; no worktree by default

## Goal and why now

Provide separate Flow/Computation ledgers and typed global-value changes with payment validation, visible balances and traceable daily/event effects. This extends the reviewed predecessors through one bounded delivery result. It must not turn their temporary fixtures into unapproved game rules.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, [TASK-011](TASK-011-Full-MVP-Task-Cluster.md), [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), [MVP decisions](../MVPDecisionRegister.md), and the [PadmaCore contract](../../Architecture/Modules/PadmaCore/README.md). Also read `Docs/Design/EN/02_SystemPlanner.md`, `Docs/Design/EN/05_NumericalPlanner.md`, the predecessor contracts, and nested Source/Content/Docs instructions for the paths below.

Source: [PDF card, rule and screen extraction](../MVPSourceExtraction.md). Old contract-card sandbox summon effects are excluded from the first version; missing ACT definitions are not filled by converting turn-based cards.

## Decisions and readiness

- Frozen: the accepted MVP/rules, separate Encounter/ACT GAS systems and tables, separate turn-based/ACT character-card collections, and shared basic-skill identity/slot mapping with mode-owned effects.
- Frozen: placeholder assets will be imported by the user; final animation/rendering polish is later work. Every unaccepted numerical value requires individual user confirmation before use, including test profiles.
- Gate references: D03, D08 See the register for the owner and exact missing information.
- Backlog describes an approved planning item, not a runnable implementation task. Record gate answers in the canonical rule/architecture source and this report, complete predecessors, then promote to Ready. Technical schemas are reviewed by the Architect; gameplay and numerical choices belong to the user.
- If gate closure requires a normative pair or structural ADR outside this write set, the Coordinator must add those exact paths before Ready; this is planned preflight, not permission for arbitrary edits.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/Core/Resources/`
- `Source/DreamOfPadma/Private/MVP/Core/Resources/`
- `Source/DreamOfPadma/Public/MVP/UI/Resources/`
- `Source/DreamOfPadma/Private/MVP/UI/Resources/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-019/`
- `Content/Padma/MVP/Definitions/Resources/`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-019-Resource-and-Global-Ledgers.md`
- `Docs/Production/Tasks/TASK-019-Resource-and-Global-Ledgers.zh-CN.md`

Exclusions: Invented debt permission, free conversion loops, synthesis formulas and automatic campaign defeat from a local value. Other task source/content, shared configuration not named above, the reference prototype, generated folders and new standalone UE modules are outside scope. Existing Demo code is consumed through public contracts; adaptation/migration needs an explicitly named write set.

## Dependencies, handoff and conflicts

- Required predecessors: [TASK-005](TASK-005-Deterministic-Random-Foundation.md), [TASK-012](TASK-012-Run-State-and-Contracts.md), [TASK-018](TASK-018-Calendar-and-Phase-Scheduler.md).
- Conditional predecessor: TASK-005 before any random outcome where it is not already listed.
- Hand off the reviewed public commands, read-only views, definition fixtures and test evidence. A state owner implements TASK-012 snapshot participation; UI never owns mutable gameplay state.
- Module README pairs, shared headers, maps, binary assets and central files are serial leases. Do not edit while another owner/build consumes them. One UE build/Editor/PIE lane.
- No writable subagent is authorized. One bounded read-only architect/reviewer may inspect the relevant contract/diff. The Primary Agent remains accountable; the Coordinator owns shared integration state and Changelog.

## Acceptance criteria

- [ ] Calamity results carry stable source/target and command/event IDs plus named random-stream positions. Re-entry with the same event identity cannot trigger the consequence twice; all new mutable attribution/calamity state participates in complete snapshot export/restore.
- [ ] Own entropy contributions attributed to their causing side and the approved calamity trigger/result value contract, with user-confirmed thresholds, probabilities and consequences. Prove a low-entropy safe trace and a seeded reproducible high-entropy benefit/calamity trace. World consequences are applied through TASK-025; ruler responses consume those events in TASK-026.
- [ ] Record initial values/caps, debt permission/bounds, payment timing and rejection policy; every change records source, reason and command/event identity.
- [ ] Flow and Computation remain separate; entropy, faith, threat, war balance, core life and stability are named values with explicit owners, not duplicate GAS health stores.
- [ ] Reject insufficient or invalid payment atomically; repeated commands cannot double-charge; one approved paid/risky loop and its compensation fixture are observable.
- [ ] Daily/event gains and HUD changes follow commands/events; snapshot restoration recovers all balances, debts and relevant event positions.
- [ ] Exact write set, synchronized task/module documentation, required checks and independent review pass; no unresolved rule has been filled by a default.

## Verification and evidence

Ledger conservation, cap/debt/insufficient-funds/duplicate payment tests and bounded loop fixtures; inspect visible source/reason for a resource change. Random compensation additionally requires TASK-005.

Compile the Editor target when native code changes. For the implemented narrow automation, use Scripts/RunTests.ps1 with the verified EngineRoot and TestFilter `DreamOfPadma.MVP.TASK019`; confirm reported cases actually passed, not only process exit. Run Scripts/AuditDocs.ps1, applicable Scripts/ValidateProject.ps1 -Strict and scoped git diff --check. Do not add broad UI automation merely for this task. Record reproducible user PIE observations (written evidence is acceptable), input/profile IDs and logs; images/video may supplement them. No runtime/manual success is claimed by this contract.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| Atomic payment and accounting | Predict a rejected payment's balances and event count, then verify both remain unchanged. | Not started |

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

## Optional relationship contribution input

Resource operations consumes TASK-012's neutral source/modifier port only for targets selected in D28 and owned by this task. TASK-039 supplies real relationship eligibility/source data; 034 injects it after integration. Use explicit provider fixtures first, not a dependency on 039. Freeze target/clock/version and a single application owner in 012; validate eligibility and reject duplicates/unsupported contributions. If another layer owns the affected scalar, pass the contribution through its approved contract rather than applying it twice. This is interface preparation; no bonus target, magnitude or stacking rule is selected here.
