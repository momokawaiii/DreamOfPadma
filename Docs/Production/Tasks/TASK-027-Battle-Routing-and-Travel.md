# TASK-027 Battle Routing and Scene Travel

- Chinese companion: TASK-027-Battle-Routing-and-Travel.zh-CN.md
- Document ID: TASK-027
- Version: 0.2
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: Game Routing
- Primary Agent: one designated owner at Ready; no writer started
- Branch/worktree: assign a Local feature branch at implementation preflight; no worktree by default

## Goal and why now

Resolve enemy-point engagements from both sides' constraints and carry stable context across sandbox/local-scene travel, camera and control-mode changes. This extends the reviewed predecessors through one bounded delivery result. It must not turn their temporary fixtures into unapproved game rules.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, [TASK-011](TASK-011-Full-MVP-Task-Cluster.md), [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), [MVP decisions](../MVPDecisionRegister.md), and the [PadmaGame contract](../../Architecture/Modules/PadmaGame/README.md). Also read `Docs/Architecture/RuntimeFlow.md`, `Docs/Rules/Combat.md`, the predecessor contracts, and nested Source/Content/Docs instructions for the paths below.

Source: [PDF card, rule and screen extraction](../MVPSourceExtraction.md). Old contract-card sandbox summon effects are excluded from the first version; missing ACT definitions are not filled by converting turn-based cards.

## Decisions and readiness

- Frozen: the accepted MVP/rules, separate Encounter/ACT GAS systems and tables, separate turn-based/ACT character-card collections, and shared basic-skill identity/slot mapping with mode-owned effects.
- Frozen: placeholder assets will be imported by the user; final animation/rendering polish is later work. Every unaccepted numerical value requires individual user confirmation before use, including test profiles.
- Gate references: D14 See the register for the owner and exact missing information.
- Backlog describes an approved planning item, not a runnable implementation task. Record gate answers in the canonical rule/architecture source and this report, complete predecessors, then promote to Ready. Technical schemas are reviewed by the Architect; gameplay and numerical choices belong to the user.
- If gate closure requires a normative pair or structural ADR outside this write set, the Coordinator must add those exact paths before Ready; this is planned preflight, not permission for arbitrary edits.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/Game/BattleRouting/`
- `Source/DreamOfPadma/Private/MVP/Game/BattleRouting/`
- `Source/DreamOfPadma/Public/MVP/UI/BattleRouting/`
- `Source/DreamOfPadma/Private/MVP/UI/BattleRouting/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-027/`
- `Content/Padma/MVP/Definitions/BattleRoutes/`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-027-Battle-Routing-and-Travel.md`
- `Docs/Production/Tasks/TASK-027-Battle-Routing-and-Travel.zh-CN.md`

Exclusions: Assuming every Encounter automatically becomes ACT, treating mode as only a camera option, broad world-streaming infrastructure and editing child maps. Other task source/content, shared configuration not named above, the reference prototype, generated folders and new standalone UE modules are outside scope. Existing Demo code is consumed through public contracts; adaptation/migration needs an explicitly named write set.

## Dependencies, handoff and conflicts

- Required predecessors: [TASK-017](TASK-017-Encounter-GAS-Playable-Loop.md), [TASK-021](TASK-021-ABC-Sandbox-Operations.md), [TASK-024](TASK-024-Sandbox-Graph-and-Movement.md), [TASK-025](TASK-025-Node-Operations-and-Story.md), [TASK-026](TASK-026-Ruler-World-AI.md), [TASK-035](TASK-035-ACT-Character-Cards-and-Roster.md).
- Conditional predecessor: TASK-005 before any random outcome where it is not already listed.
- Hand off the reviewed public commands, read-only views, definition fixtures and test evidence. A state owner implements TASK-012 snapshot participation; UI never owns mutable gameplay state.
- Module README pairs, shared headers, maps, binary assets and central files are serial leases. Do not edit while another owner/build consumes them. One UE build/Editor/PIE lane.
- No writable subagent is authorized. One bounded read-only architect/reviewer may inspect the relevant contract/diff. The Primary Agent remains accountable; the Coordinator owns shared integration state and Changelog.

## Acceptance criteria

- [ ] An ABC-controlled unit reaching an occupied enemy node triggers legal route resolution at any calendar phase; terrain/cards and both sides' constraints are explicit inputs.
- [ ] Resolve mode/site/spawn/definition and story consequence identities; invalid or missing bindings report failure without partially consuming context or changing control.
- [ ] Encounter travel and return work against its approved adapter; the ACT route contract can be tested with a stub until TASK-030 supplies the real adapter, and unavailable ACT entry fails visibly.
- [ ] Camera/possession/input-context lifetime follows the active mode; old-scene delegates and stale battle IDs cannot affect the destination. TASK-028 adds transactional entry/return.
- [ ] Carry the selected ACT roster/loadout and terrain context as separate IDs/views; resolve card/trait restrictions through the ACT roster validator, not by reusing deployed turn-based character identity.
- [ ] Exact write set, synchronized task/module documentation, required checks and independent review pass; no unresolved rule has been filled by a default.

## Verification and evidence

Joint-constraint fixtures, missing destination and stale-context failure tests; PIE sandbox-to-Encounter-and-back. Real ACT travel is verified again by TASK-030/034.

Compile the Editor target when native code changes. For the implemented narrow automation, use Scripts/RunTests.ps1 with the verified EngineRoot and TestFilter `DreamOfPadma.MVP.TASK027`; confirm reported cases actually passed, not only process exit. Run Scripts/AuditDocs.ps1, applicable Scripts/ValidateProject.ps1 -Strict and scoped git diff --check. Do not add broad UI automation merely for this task. Record reproducible user PIE observations (written evidence is acceptable), input/profile IDs and logs; images/video may supplement them. No runtime/manual success is claimed by this contract.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| Mode routing versus presentation | Trace why the same node can resolve different legal routes without using a map name as a rule. | Not started |

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

## Inspection is a separate intent

TASK-037 owns node-inspection entry/return and local presentation. This task routes actual engagements after approved movement/operation outcomes; opening a node scene alone must not fabricate an enemy-occupancy trigger or pre-battle transaction. D24 specifies any composite click behavior. Share stable site/context through public contracts; a scene can host later battle presentation without making every visit a battle. FPS scope remains D26.

## 2026-09-09 confirmed model/movement handoff

D24 confirms that node inspection never creates battle/transaction requests. Only the actual approved movement/operation outcome may trigger route resolution. FPS is outside the first playable MVP (D26); Encounter and ACT remain distinct routed modes.
