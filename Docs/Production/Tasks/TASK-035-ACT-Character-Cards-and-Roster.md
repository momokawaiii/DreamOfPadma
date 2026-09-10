# TASK-035 ACT Character Cards and Battle Roster

- Chinese companion: TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md
- Document ID: TASK-035
- Version: 0.2
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: ACT Roster / UI
- Primary Agent: one designated owner at Ready; no writer started
- Branch/worktree: assign a Local feature branch at implementation preflight; no worktree by default

## Goal and why now

Provide the separate ACT character-card collection and battle settings for an ACT loadout of characters, weapons and the approved equipment subset, with contextual eligibility checks. This extends the reviewed predecessors through one bounded delivery result. It must not turn their temporary fixtures into unapproved game rules.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, [TASK-011](TASK-011-Full-MVP-Task-Cluster.md), [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), [MVP decisions](../MVPDecisionRegister.md), and the [PadmaGame contract](../../Architecture/Modules/PadmaGame/README.md). Also read `Docs/Architecture/SaveSchema.md`, `Docs/Rules/Combat.md`, the predecessor contracts, and nested Source/Content/Docs instructions for the paths below.

Source: [PDF card, rule and screen extraction](../MVPSourceExtraction.md). Old contract-card sandbox summon effects are excluded from the first version; missing ACT definitions are not filled by converting turn-based cards.

## Decisions and readiness

- Frozen: the accepted MVP/rules, separate Encounter/ACT GAS systems and tables, separate turn-based/ACT character-card collections, and shared basic-skill identity/slot mapping with mode-owned effects.
- Frozen: placeholder assets will be imported by the user; final animation/rendering polish is later work. Every unaccepted numerical value requires individual user confirmation before use, including test profiles.
- Gate references: D01, D02, D03, D20, D23 See the register for the owner and exact missing information.
- Backlog describes an approved planning item, not a runnable implementation task. Record gate answers in the canonical rule/architecture source and this report, complete predecessors, then promote to Ready. Technical schemas are reviewed by the Architect; gameplay and numerical choices belong to the user.
- If gate closure requires a normative pair or structural ADR outside this write set, the Coordinator must add those exact paths before Ready; this is planned preflight, not permission for arbitrary edits.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/Core/ACTRoster/`
- `Source/DreamOfPadma/Private/MVP/Core/ACTRoster/`
- `Source/DreamOfPadma/Public/MVP/Game/ACTRoster/`
- `Source/DreamOfPadma/Private/MVP/Game/ACTRoster/`
- `Source/DreamOfPadma/Public/MVP/UI/BattleSettings/`
- `Source/DreamOfPadma/Private/MVP/UI/BattleSettings/`
- `Source/DreamOfPadma/Public/MVP/Gameplay/ACT/RosterData/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/ACT/RosterData/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-035/`
- `Content/Padma/MVP/UI/BattleSettings/`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.md`
- `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md`

Exclusions: Treating a deployed turn-based card as the same ACT character card, inventing roster size/cost/unlock policies and ACT attack execution. Other task source/content, shared configuration not named above, the reference prototype, generated folders and new standalone UE modules are outside scope. Existing Demo code is consumed through public contracts; adaptation/migration needs an explicitly named write set.

## Dependencies, handoff and conflicts

- Required predecessors: [TASK-012](TASK-012-Run-State-and-Contracts.md), [TASK-019](TASK-019-Resource-and-Global-Ledgers.md), [TASK-020](TASK-020-Card-Catalog-and-Lifecycle.md), [TASK-036](TASK-036-ACT-Authoring-Definitions.md), [TASK-040](TASK-040-Model-Assets-and-Scene-Bindings.md).
- Conditional predecessor: TASK-005 before any random outcome where it is not already listed.
- Hand off the reviewed public commands, read-only views, definition fixtures and test evidence. A state owner implements TASK-012 snapshot participation; UI never owns mutable gameplay state.
- Module README pairs, shared headers, maps, binary assets and central files are serial leases. Do not edit while another owner/build consumes them. One UE build/Editor/PIE lane.
- No writable subagent is authorized. One bounded read-only architect/reviewer may inspect the relevant contract/diff. The Primary Agent remains accountable; the Coordinator owns shared integration state and Changelog.

## Acceptance criteria

- [ ] Turn-based character cards are deployed on the sandbox; ACT character cards belong to a separate collection with separate stable definitions and mode bindings.
- [ ] Record roster capacity, selection/change timing, unlock/ownership, initial availability, costs and preset-versus-run-save ownership before implementation.
- [ ] The battle-settings UI selects and validates the ACT roster and displays readable unavailable/incomplete selection errors without mutating the deployed turn-based unit.
- [ ] A public entry contract passes versioned roster state and stable character/definition IDs; verify it here with approved context fixtures and a test consumer. TASK-027/030 own real route/ACT consumption. The relation to the triggering map unit follows the user's recorded decision.
- [ ] State explicitly classified as run-owned in D20 exports/restores through the participant contract, with defeat/exit/new-run/save behavior tested. Any global preset belongs to a separate boundary; do not assume every battle-setting field participates in local battle rollback.
- [ ] Represent terrain/context restrictions with an explicit target: a character, weapon, ability or trait. A trait-only restriction must not silently ban the entire character; exact restriction scopes, timing and feedback are confirmed in D20.
- [ ] Exact write set, synchronized task/module documentation, required checks and independent review pass; no unresolved rule has been filled by a default.

## Verification and evidence

Separate card collections, character/weapon selection, invalid roster, terrain-restricted trait, stale context and snapshot tests; user configures a loadout and sees its eligibility in the ACT entry context.

Compile the Editor target when native code changes. For the implemented narrow automation, use Scripts/RunTests.ps1 with the verified EngineRoot and TestFilter `DreamOfPadma.MVP.TASK035`; confirm reported cases actually passed, not only process exit. Run Scripts/AuditDocs.ps1, applicable Scripts/ValidateProject.ps1 -Strict and scoped git diff --check. Do not add broad UI automation merely for this task. Record reproducible user PIE observations (written evidence is acceptable), input/profile IDs and logs; images/video may supplement them. No runtime/manual success is claimed by this contract.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| Deployed unit versus selected roster | Compare the map unit ID with the configured ACT roster IDs and explain which service owns each. | Not started |

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

## Global preparation composition

TASK-038 hosts this task's ACT selection panel and validated roster provider; do not duplicate either authority. TASK-036 retains static definitions. D27 adds availability/edit/time policy to D20's preset/run ownership gate. Relationship bonus sources arrive through approved provider contracts, not by moving their state into the ACT roster. FPS has its own unimplemented capability boundary until D26 is answered.

## 2026-09-09 confirmed model/movement handoff

Reuse the additive TASK-040 mobility metadata already on ACT character definitions; it does not make an ACT roster entry a deployed world card. ACT model/animation/skill-table references remain in TASK-036 definitions and are read by 040's Game adapter. Actual player roster commands and terrain checks remain owned here. D26 excludes FPS gameplay.
