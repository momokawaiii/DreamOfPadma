# TASK-030 Playable ACT with GAS

- Chinese companion: TASK-030-ACT-GAS-Playable-Loop.zh-CN.md
- Document ID: TASK-030
- Version: 0.1
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: ACT Gameplay
- Primary Agent: one designated owner at Ready; no writer started
- Branch/worktree: assign a Local feature branch at implementation preflight; no worktree by default

## Goal and why now

Complete one real-time ACT battle using its own GAS abilities, character catalog/typed tables, sequence execution, input/camera, hit validation and enemy behavior. This extends the reviewed predecessors through one bounded delivery result. It must not turn their temporary fixtures into unapproved game rules.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, [TASK-011](TASK-011-Full-MVP-Task-Cluster.md), [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), [MVP decisions](../MVPDecisionRegister.md), and the [PadmaGameplay contract](../../Architecture/Modules/PadmaGameplay/README.md). Also read `Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md`, `Docs/Rules/Combat.md`, the predecessor contracts, and nested Source/Content/Docs instructions for the paths below.

Source: [PDF card, rule and screen extraction](../MVPSourceExtraction.md). Old contract-card sandbox summon effects are excluded from the first version; missing ACT definitions are not filled by converting turn-based cards.

## Decisions and readiness

- Frozen: the accepted MVP/rules, separate Encounter/ACT GAS systems and tables, separate turn-based/ACT character-card collections, and shared basic-skill identity/slot mapping with mode-owned effects.
- Frozen: placeholder assets will be imported by the user; final animation/rendering polish is later work. Every unaccepted numerical value requires individual user confirmation before use, including test profiles.
- Gate references: D01, D03, D04, D06, D16, D23 See the register for the owner and exact missing information.
- Backlog describes an approved planning item, not a runnable implementation task. Record gate answers in the canonical rule/architecture source and this report, complete predecessors, then promote to Ready. Technical schemas are reviewed by the Architect; gameplay and numerical choices belong to the user.
- If gate closure requires a normative pair or structural ADR outside this write set, the Coordinator must add those exact paths before Ready; this is planned preflight, not permission for arbitrary edits.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/Gameplay/ACT/Runtime/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/ACT/Runtime/`
- `Source/DreamOfPadma/Public/MVP/Gameplay/ACT/Abilities/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/ACT/Abilities/`
- `Source/DreamOfPadma/Public/MVP/Gameplay/ACT/Tasks/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/ACT/Tasks/`
- `Source/DreamOfPadma/Public/MVP/Gameplay/ACT/Data/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/ACT/Data/`
- `Source/DreamOfPadma/Public/MVP/Gameplay/ACT/Input/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/ACT/Input/`
- `Source/DreamOfPadma/Public/MVP/UI/ACT/`
- `Source/DreamOfPadma/Private/MVP/UI/ACT/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-030/`
- `Content/Padma/MVP/ACT/Playable/`
- `Content/Padma/MVP/ACT/Definitions/`
- `Content/Padma/MVP/ACT/Input/`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-030-ACT-GAS-Playable-Loop.md`
- `Docs/Production/Tasks/TASK-030-ACT-GAS-Playable-Loop.zh-CN.md`

Exclusions: Encounter row/schema reuse, full weapon/payload families, speculative combo/Motion Warping frameworks, external plugins and network prediction. Other task source/content, shared configuration not named above, the reference prototype, generated folders and new standalone UE modules are outside scope. Existing Demo code is consumed through public contracts; adaptation/migration needs an explicitly named write set.

## Dependencies, handoff and conflicts

- Required predecessors: [TASK-013](TASK-013-GAS-Infrastructure.md), [TASK-014](TASK-014-Character-Presentation.md), [TASK-015](TASK-015-Combat-Calculation-Contracts.md), [TASK-017](TASK-017-Encounter-GAS-Playable-Loop.md), [TASK-027](TASK-027-Battle-Routing-and-Travel.md), [TASK-028](TASK-028-Battle-Transactions.md), [TASK-035](TASK-035-ACT-Character-Cards-and-Roster.md).
- Conditional predecessor: TASK-005 before any random outcome where it is not already listed.
- Hand off the reviewed public commands, read-only views, definition fixtures and test evidence. A state owner implements TASK-012 snapshot participation; UI never owns mutable gameplay state.
- Module README pairs, shared headers, maps, binary assets and central files are serial leases. Do not edit while another owner/build consumes them. One UE build/Editor/PIE lane.
- No writable subagent is authorized. One bounded read-only architect/reviewer may inspect the relevant contract/diff. The Primary Agent remains accountable; the Coordinator owns shared integration state and Changelog.

## Acceptance criteria

- [ ] Record the first playable action, ACT AttributeSet/effect policies, clock domains, hit/cancel/interrupt and input rules; preserve the ACT-specific catalog/AbilityDefinition/ACTSequence split.
- [ ] Movement/camera/attack, native ability tasks, one authored execution sequence and simple enemy approach/attack/death form a complete local result.
- [ ] ACT tables and runtime state cannot fall back to Encounter rows; any reused calculation is selected explicitly with mode-owned values and approved meaning.
- [ ] Native target/hit validation owns effect application; montage timing markers and Gameplay Cues remain presentation inputs/outputs rather than damage authority.
- [ ] Cancellation, interruption, death, scene return and load clean up effects/tasks/buffers; actual ACT routing and complete commit/rollback work with TASK-027/028.
- [ ] Spawn/control the validated character/weapon loadout from battle settings; enforce terrain/context restrictions at the approved validation point, including passive/active traits. Direct commands bypassing UI must still reject a restricted trait; a trait-only restriction cannot change card identity or silently remove the character.
- [ ] Exact write set, synchronized task/module documentation, required checks and independent review pass; no unresolved rule has been filled by a default.

## Verification and evidence

Editor compile; sequence/hit/double-hit/cancel/cleanup and wrong-mode tests; user completes ACT victory and defeat/exit. Reproducibility claims cover controlled rule inputs and clocks, not arbitrary physics from seed alone.

Compile the Editor target when native code changes. For the implemented narrow automation, use Scripts/RunTests.ps1 with the verified EngineRoot and TestFilter `DreamOfPadma.MVP.TASK030`; confirm reported cases actually passed, not only process exit. Run Scripts/AuditDocs.ps1, applicable Scripts/ValidateProject.ps1 -Strict and scoped git diff --check. Do not add broad UI automation merely for this task. Record reproducible user PIE observations (written evidence is acceptable), input/profile IDs and logs; images/video may supplement them. No runtime/manual success is claimed by this contract.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| ACT data and native execution | Change one ACT sequence fixture, predict its timing and verify the Encounter fixture is unaffected. | Not started |

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


TASK-036 owns static ACT character/weapon/skill definitions, typed skill tables and ACTCharacterCards content. Consume its reviewed public interface before implementation; do not duplicate those types. ACT/Data and Definitions hold execution/effect configuration only; RosterData holds roster projections/adapters only. The user still supplies character/weapon content; empty templates do not close playable-content gates.
