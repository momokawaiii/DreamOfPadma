# TASK-017 Playable Encounter with GAS

- Chinese companion: TASK-017-Encounter-GAS-Playable-Loop.zh-CN.md
- Document ID: TASK-017
- Version: 0.1
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: Encounter Gameplay
- Primary Agent: one designated owner at Ready; no writer started
- Branch/worktree: assign a Local feature branch at implementation preflight; no worktree by default

## Goal and why now

Complete one visible GAS-backed Encounter with its own definition catalog, abilities, attributes/effect policies, enemy action policy and battle UI. This extends the reviewed predecessors through one bounded delivery result. It must not turn their temporary fixtures into unapproved game rules.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, [TASK-011](TASK-011-Full-MVP-Task-Cluster.md), [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), [MVP decisions](../MVPDecisionRegister.md), and the [PadmaGameplay contract](../../Architecture/Modules/PadmaGameplay/README.md). Also read `Docs/Rules/Combat.md`, `Docs/Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.md`, the predecessor contracts, and nested Source/Content/Docs instructions for the paths below.

Source: [PDF card, rule and screen extraction](../MVPSourceExtraction.md). Old contract-card sandbox summon effects are excluded from the first version; missing ACT definitions are not filled by converting turn-based cards.

## Decisions and readiness

- Frozen: the accepted MVP/rules, separate Encounter/ACT GAS systems and tables, separate turn-based/ACT character-card collections, and shared basic-skill identity/slot mapping with mode-owned effects.
- Frozen: placeholder assets will be imported by the user; final animation/rendering polish is later work. Every unaccepted numerical value requires individual user confirmation before use, including test profiles.
- Gate references: D01, D03, D05, D06 See the register for the owner and exact missing information.
- Backlog describes an approved planning item, not a runnable implementation task. Record gate answers in the canonical rule/architecture source and this report, complete predecessors, then promote to Ready. Technical schemas are reviewed by the Architect; gameplay and numerical choices belong to the user.
- If gate closure requires a normative pair or structural ADR outside this write set, the Coordinator must add those exact paths before Ready; this is planned preflight, not permission for arbitrary edits.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/Gameplay/Encounter/Runtime/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/Encounter/Runtime/`
- `Source/DreamOfPadma/Public/MVP/Gameplay/Encounter/Abilities/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/Encounter/Abilities/`
- `Source/DreamOfPadma/Public/MVP/Gameplay/Encounter/Data/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/Encounter/Data/`
- `Source/DreamOfPadma/Public/MVP/UI/Encounter/`
- `Source/DreamOfPadma/Private/MVP/UI/Encounter/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-017/`
- `Content/Padma/MVP/Encounter/Playable/`
- `Content/Padma/MVP/Encounter/Definitions/`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-017-Encounter-GAS-Playable-Loop.md`
- `Docs/Production/Tasks/TASK-017-Encounter-GAS-Playable-Loop.zh-CN.md`

Exclusions: ACT tables/sequences, the full card-deck lifecycle, generic world AI and campaign victory implementation. Other task source/content, shared configuration not named above, the reference prototype, generated folders and new standalone UE modules are outside scope. Existing Demo code is consumed through public contracts; adaptation/migration needs an explicitly named write set.

## Dependencies, handoff and conflicts

- Required predecessors: [TASK-013](TASK-013-GAS-Infrastructure.md), [TASK-014](TASK-014-Character-Presentation.md), [TASK-015](TASK-015-Combat-Calculation-Contracts.md), [TASK-016](TASK-016-Encounter-Timeline.md).
- Conditional predecessor: TASK-005 before any random outcome where it is not already listed.
- Hand off the reviewed public commands, read-only views, definition fixtures and test evidence. A state owner implements TASK-012 snapshot participation; UI never owns mutable gameplay state.
- Module README pairs, shared headers, maps, binary assets and central files are serial leases. Do not edit while another owner/build consumes them. One UE build/Editor/PIE lane.
- No writable subagent is authorized. One bounded read-only architect/reviewer may inspect the relevant contract/diff. The Primary Agent remains accountable; the Coordinator owns shared integration state and Changelog.

## Acceptance criteria

- [ ] Consume the provider ports already frozen by TASK-012; do not introduce a new public provider interface that TASK-020 would need to depend on Encounter to implement.
- [ ] Encounter definitions load through a mode-specific resolver and drive targeting, damage/status and GAS execution; ACT definitions cannot activate here.
- [ ] The user sees action order, target choice, one legal basic card, invalid/repeated-card feedback, enemy action, animation/cues, health and a local result.
- [ ] Expose injectable card-state, legal-play/payment and lifecycle providers. Use explicit approved fixtures until TASK-020 supplies production providers; do not convert TASK-009's demo summon into ABC in-battle legality.
- [ ] Timeline permission gates GAS activation; animation/Cue failure cannot change the authoritative outcome, and duplicate completion cannot apply an effect twice.
- [ ] Ability interruption or an individual unit's death cleans up only that execution/entity and returns control to the timeline. Only an approved local battle end or exit closes the battle, cleans remaining mode state and emits one typed battle result; expose snapshot participation for TASK-028.
- [ ] Exact write set, synchronized task/module documentation, required checks and independent review pass; no unresolved rule has been filled by a default.

## Verification and evidence

Editor compile and activation/result/cleanup tests; user plays the Encounter, witnesses an illegal action and completes a victory and a configured local defeat fixture.

Compile the Editor target when native code changes. For the implemented narrow automation, use Scripts/RunTests.ps1 with the verified EngineRoot and TestFilter `DreamOfPadma.MVP.TASK017`; confirm reported cases actually passed, not only process exit. Run Scripts/AuditDocs.ps1, applicable Scripts/ValidateProject.ps1 -Strict and scoped git diff --check. Do not add broad UI automation merely for this task. Record reproducible user PIE observations (written evidence is acceptable), input/profile IDs and logs; images/video may supplement them. No runtime/manual success is claimed by this contract.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| Timeline permission to GAS execution | Trace a card request through timeline validation, the Encounter ability and result-driven HUD. | Not started |

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
