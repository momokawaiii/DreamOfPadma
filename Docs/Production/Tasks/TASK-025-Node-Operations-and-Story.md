# TASK-025 Node Operations and Story Mutations

- Chinese companion: TASK-025-Node-Operations-and-Story.zh-CN.md
- Document ID: TASK-025
- Version: 0.2
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: World Content Systems
- Primary Agent: one designated owner at Ready; no writer started
- Branch/worktree: assign a Local feature branch at implementation preflight; no worktree by default

## Goal and why now

Make capture, a facility, furnace transformation, safe-house recovery, conditional road opening and one two-choice anecdote produce persistent visible world changes. This extends the reviewed predecessors through one bounded delivery result. It must not turn their temporary fixtures into unapproved game rules.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, [TASK-011](TASK-011-Full-MVP-Task-Cluster.md), [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), [MVP decisions](../MVPDecisionRegister.md), and the [PadmaWorld contract](../../Architecture/Modules/PadmaWorld/README.md). Also read `Docs/Rules/WorldState.md`, `Docs/Design/EN/04_LevelContentPlanner.md`, the predecessor contracts, and nested Source/Content/Docs instructions for the paths below.

Source: [PDF card, rule and screen extraction](../MVPSourceExtraction.md). Old contract-card sandbox summon effects are excluded from the first version; missing ACT definitions are not filled by converting turn-based cards.

## Decisions and readiness

- Frozen: the accepted MVP/rules, separate Encounter/ACT GAS systems and tables, separate turn-based/ACT character-card collections, and shared basic-skill identity/slot mapping with mode-owned effects.
- Frozen: placeholder assets will be imported by the user; final animation/rendering polish is later work. Every unaccepted numerical value requires individual user confirmation before use, including test profiles.
- Gate references: D03, D12, D19 See the register for the owner and exact missing information.
- Backlog describes an approved planning item, not a runnable implementation task. Record gate answers in the canonical rule/architecture source and this report, complete predecessors, then promote to Ready. Technical schemas are reviewed by the Architect; gameplay and numerical choices belong to the user.
- If gate closure requires a normative pair or structural ADR outside this write set, the Coordinator must add those exact paths before Ready; this is planned preflight, not permission for arbitrary edits.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/World/NodeOperations/`
- `Source/DreamOfPadma/Private/MVP/World/NodeOperations/`
- `Source/DreamOfPadma/Public/MVP/World/Story/`
- `Source/DreamOfPadma/Private/MVP/World/Story/`
- `Source/DreamOfPadma/Public/MVP/UI/NodeOperations/`
- `Source/DreamOfPadma/Private/MVP/UI/NodeOperations/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-025/`
- `Content/Padma/MVP/World/NodeOperationFixtures/`
- `Content/Padma/MVP/World/StoryFixtures/`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaWorld/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-025-Node-Operations-and-Story.md`
- `Docs/Production/Tasks/TASK-025-Node-Operations-and-Story.zh-CN.md`

Exclusions: Complete narrative, arbitrary world scripting, mutation directly from a Widget and inventing a free recovery/conversion rule. Other task source/content, shared configuration not named above, the reference prototype, generated folders and new standalone UE modules are outside scope. Existing Demo code is consumed through public contracts; adaptation/migration needs an explicitly named write set.

## Dependencies, handoff and conflicts

- Required predecessors: [TASK-018](TASK-018-Calendar-and-Phase-Scheduler.md), [TASK-021](TASK-021-ABC-Sandbox-Operations.md), [TASK-024](TASK-024-Sandbox-Graph-and-Movement.md).
- Conditional predecessor: TASK-005 before any random outcome where it is not already listed.
- Hand off the reviewed public commands, read-only views, definition fixtures and test evidence. A state owner implements TASK-012 snapshot participation; UI never owns mutable gameplay state.
- Module README pairs, shared headers, maps, binary assets and central files are serial leases. Do not edit while another owner/build consumes them. One UE build/Editor/PIE lane.
- No writable subagent is authorized. One bounded read-only architect/reviewer may inspect the relevant contract/diff. The Primary Agent remains accountable; the Coordinator owns shared integration state and Changelog.

## Acceptance criteria

- [ ] World consequence application is idempotent by event identity; a repeated calamity event cannot apply ownership/resource/world effects twice, and recovery preserves its applied-event record.
- [ ] Apply the approved world consequences of TASK-019 calamity results through the same validated mutation/event and snapshot boundary as node/story operations, preserving the causing-side attribution and refusing unknown effects without partial state.
- [ ] Record selected node conditions/costs and story consequences; capture and facility changes occur through validated commands/events.
- [ ] The furnace's approved operation turns it into an ordinary node; safe-house recovery and a conditionally opened path expose their cost and state change.
- [ ] A two-choice anecdote changes a flag and an approved resource/threat/faith/route consequence; revisiting respects the recorded event state.
- [ ] A terrain affix has one approved observable rule effect; all flags, ownership, facilities and event history are included in snapshot restoration.
- [ ] Exact write set, synchronized task/module documentation, required checks and independent review pass; no unresolved rule has been filled by a default.

## Verification and evidence

Prerequisite/payment/repeated-operation tests and mutation/restore round trips; user observes a capture, transformation, recovery and two alternative story fixtures.

Compile the Editor target when native code changes. For the implemented narrow automation, use Scripts/RunTests.ps1 with the verified EngineRoot and TestFilter `DreamOfPadma.MVP.TASK025`; confirm reported cases actually passed, not only process exit. Run Scripts/AuditDocs.ps1, applicable Scripts/ValidateProject.ps1 -Strict and scoped git diff --check. Do not add broad UI automation merely for this task. Record reproducible user PIE observations (written evidence is acceptable), input/profile IDs and logs; images/video may supplement them. No runtime/manual success is claimed by this contract.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| Persistent mutations from commands | Predict the map and flags after each story choice, then inspect the recorded event. | Not started |

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

## Relationship event handoff

TASK-039 may consume this task's stable story-outcome event IDs via TASK-034 composition. Export the event/source/choice identity without implementing relationship progression or bonus execution here. Replays remain identifiable; a relationship consumer can reject duplicates. D28 owns concrete relationship consequences. TASK-037 consumes current facility/story/node views, not another copy of their state.
