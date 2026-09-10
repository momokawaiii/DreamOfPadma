# TASK-034 Full MVP Integration and Acceptance

- Chinese companion: TASK-034-Full-MVP-Integration.zh-CN.md
- Document ID: TASK-034
- Version: 0.2
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: Integration Coordinator
- Primary Agent: one designated owner at Ready; no writer started
- Branch/worktree: assign a Local feature branch at implementation preflight; no worktree by default

## Goal and why now

Wire reviewed feature adapters and the First Era pack into one default-entry MVP, then prove complete playable runs and failure recovery with user evidence. This extends the reviewed predecessors through one bounded delivery result. It must not turn their temporary fixtures into unapproved game rules.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, [TASK-011](TASK-011-Full-MVP-Task-Cluster.md), [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), [MVP decisions](../MVPDecisionRegister.md), and the [PadmaGame contract](../../Architecture/Modules/PadmaGame/README.md). Also read `Docs/Production/BuildMatrix.md`, `Docs/Architecture/SaveSchema.md`, the predecessor contracts, and nested Source/Content/Docs instructions for the paths below.

Source: [PDF card, rule and screen extraction](../MVPSourceExtraction.md). Old contract-card sandbox summon effects are excluded from the first version; missing ACT definitions are not filled by converting turn-based cards.

## Decisions and readiness

- Frozen: the accepted MVP/rules, separate Encounter/ACT GAS systems and tables, separate turn-based/ACT character-card collections, and shared basic-skill identity/slot mapping with mode-owned effects.
- Frozen: placeholder assets will be imported by the user; final animation/rendering polish is later work. Every unaccepted numerical value requires individual user confirmation before use, including test profiles.
- Gate references: ALL See the register for the owner and exact missing information.
- Backlog describes an approved planning item, not a runnable implementation task. Record gate answers in the canonical rule/architecture source and this report, complete predecessors, then promote to Ready. Technical schemas are reviewed by the Architect; gameplay and numerical choices belong to the user.
- If gate closure requires a normative pair or structural ADR outside this write set, the Coordinator must add those exact paths before Ready; this is planned preflight, not permission for arbitrary edits.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/Game/MVPComposition/`
- `Source/DreamOfPadma/Private/MVP/Game/MVPComposition/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-034/`
- `Content/Padma/MVP/Integration/FirstEra/`
- `Config/DefaultEngine.ini`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-034-Full-MVP-Integration.md`
- `Docs/Production/Tasks/TASK-034-Full-MVP-Integration.zh-CN.md`
- `Docs/ProjectState.md`
- `Docs/ProjectState.zh-CN.md`
- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`
- `Docs/Production/Roadmap.md`
- `Docs/Production/Roadmap.zh-CN.md`
- `Docs/Changelog.md`
- `Docs/Changelog.zh-CN.md`

Exclusions: Fixing child feature code/maps, broad refactors, remote publication and Windows packaging deferred after MVP acceptance. Other task source/content, shared configuration not named above, the reference prototype, generated folders and new standalone UE modules are outside scope. Existing Demo code is consumed through public contracts; adaptation/migration needs an explicitly named write set.

## Dependencies, handoff and conflicts

- Required predecessors: [TASK-005](TASK-005-Deterministic-Random-Foundation.md), [TASK-010](TASK-010-Playable-Prototype-Integration.md), [TASK-012](TASK-012-Run-State-and-Contracts.md), [TASK-013](TASK-013-GAS-Infrastructure.md), [TASK-014](TASK-014-Character-Presentation.md), [TASK-015](TASK-015-Combat-Calculation-Contracts.md), [TASK-016](TASK-016-Encounter-Timeline.md), [TASK-017](TASK-017-Encounter-GAS-Playable-Loop.md), [TASK-018](TASK-018-Calendar-and-Phase-Scheduler.md), [TASK-019](TASK-019-Resource-and-Global-Ledgers.md), [TASK-020](TASK-020-Card-Catalog-and-Lifecycle.md), [TASK-021](TASK-021-ABC-Sandbox-Operations.md), [TASK-022](TASK-022-Synthesis-Resolution.md), [TASK-023](TASK-023-Synthesis-UI.md), [TASK-024](TASK-024-Sandbox-Graph-and-Movement.md), [TASK-025](TASK-025-Node-Operations-and-Story.md), [TASK-026](TASK-026-Ruler-World-AI.md), [TASK-027](TASK-027-Battle-Routing-and-Travel.md), [TASK-028](TASK-028-Battle-Transactions.md), [TASK-029](TASK-029-Safe-Boundary-Save-Load.md), [TASK-030](TASK-030-ACT-GAS-Playable-Loop.md), [TASK-031](TASK-031-ACT-Repository-and-Bullet-Time.md), [TASK-032](TASK-032-Run-Outcomes-and-Restart.md), [TASK-033](TASK-033-First-Era-Content-Pack.md), [TASK-035](TASK-035-ACT-Character-Cards-and-Roster.md), [TASK-037](TASK-037-Node-Inspection-and-Local-Scenes.md), [TASK-038](TASK-038-Global-Preparation-and-Bonus-UI.md), [TASK-039](TASK-039-Story-Relationships-and-Bonus-State.md).
- Conditional predecessor: TASK-005 before any random outcome where it is not already listed.
- Hand off the reviewed public commands, read-only views, definition fixtures and test evidence. A state owner implements TASK-012 snapshot participation; UI never owns mutable gameplay state.
- Module README pairs, shared headers, maps, binary assets and central files are serial leases. Do not edit while another owner/build consumes them. One UE build/Editor/PIE lane.
- No writable subagent is authorized. One bounded read-only architect/reviewer may inspect the relevant contract/diff. The Primary Agent remains accountable; the Coordinator owns shared integration state and Changelog.

## Acceptance criteria

- [ ] Run the First Era pack's low-entropy safe route and reproducible high-entropy benefit/calamity route, verifying causing-side attribution, world/ruler consequences and full rollback/save preservation.
- [ ] Default entry loads the approved pack, session and world; inputs, camera, HUD and mode-specific catalogs bind through public composition contracts. Bind TASK-020's production card/payment/lifecycle providers into Encounter and ACT instead of fixture providers, and verify actual payment and lifecycle state changes in each mode.
- [ ] User plays a seven-day sandbox/economy/synthesis route with Encounter and ACT battles, visible world consequences, save/load and a campaign result.
- [ ] Separately verify both campaign victories and explicit defeat, local retreat/defeat full rollback, failed transition/load handling, clean restart and repeated scene entry.
- [ ] Verify cross-mode table/clock/input/lifetime isolation and the shared card identity mapping; independent mode mechanics do not regress through integration.
- [ ] Required Editor build, focused automation, asset-load checks, performance observations, bilingual docs and independent reviews pass; user observations are not fabricated.
- [ ] Exact write set, synchronized task/module documentation, required checks and independent review pass; no unresolved rule has been filled by a default.

## Verification and evidence

Run the union of required child regressions once on the final integrated state, documentation/project checks and a reproducible end-to-end PIE script. Return failures to their owning task and rerun affected checks after repairs.

Compile the Editor target when native code changes. For the implemented narrow automation, use Scripts/RunTests.ps1 with the verified EngineRoot and TestFilter `DreamOfPadma.MVP.TASK034`; confirm reported cases actually passed, not only process exit. Run Scripts/AuditDocs.ps1, applicable Scripts/ValidateProject.ps1 -Strict and scoped git diff --check. Do not add broad UI automation merely for this task. Record reproducible user PIE observations (written evidence is acceptable), input/profile IDs and logs; images/video may supplement them. No runtime/manual success is claimed by this contract.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| Evidence-based integration | Follow the complete route, identify each state owner and explain which task should fix one observed failure. | Not started |

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

## Map and preparation integration acceptance

Bind TASK-037 navigation hosts to TASK-038 and TASK-039 story/bonus sources to the approved card/resource/combat consumers. Verify map hover → node scene → entity detail → global preparation → same-node return; observer disclosure applies to models as well as labels. Separately verify approved movement/payment/battle and actual animation. Refresh, repeated visits and save/rollback do not duplicate entity ownership or bonus sources. FPS acceptance follows D26 only; no unspecified playable route is assumed.
