# TASK-026 Ruler World AI

- Chinese companion: TASK-026-Ruler-World-AI.zh-CN.md
- Document ID: TASK-026
- Version: 0.1
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: Ruler AI
- Primary Agent: one designated owner at Ready; no writer started
- Branch/worktree: assign a Local feature branch at implementation preflight; no worktree by default

## Goal and why now

Deliver an explainable ruler policy for patrol, detection/stealth, reinforcement and counterattack on the shared sandbox. This extends the reviewed predecessors through one bounded delivery result. It must not turn their temporary fixtures into unapproved game rules.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, [TASK-011](TASK-011-Full-MVP-Task-Cluster.md), [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), [MVP decisions](../MVPDecisionRegister.md), and the [PadmaGameplay contract](../../Architecture/Modules/PadmaGameplay/README.md). Also read `Docs/Design/EN/03_CombatAIPlanner.md`, `Docs/Design/EN/04_LevelContentPlanner.md`, the predecessor contracts, and nested Source/Content/Docs instructions for the paths below.

Source: [PDF card, rule and screen extraction](../MVPSourceExtraction.md). Old contract-card sandbox summon effects are excluded from the first version; missing ACT definitions are not filled by converting turn-based cards.

## Decisions and readiness

- Frozen: the accepted MVP/rules, separate Encounter/ACT GAS systems and tables, separate turn-based/ACT character-card collections, and shared basic-skill identity/slot mapping with mode-owned effects.
- Frozen: placeholder assets will be imported by the user; final animation/rendering polish is later work. Every unaccepted numerical value requires individual user confirmation before use, including test profiles.
- Gate references: D03, D13 See the register for the owner and exact missing information.
- Backlog describes an approved planning item, not a runnable implementation task. Record gate answers in the canonical rule/architecture source and this report, complete predecessors, then promote to Ready. Technical schemas are reviewed by the Architect; gameplay and numerical choices belong to the user.
- If gate closure requires a normative pair or structural ADR outside this write set, the Coordinator must add those exact paths before Ready; this is planned preflight, not permission for arbitrary edits.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/Gameplay/RulerAI/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/RulerAI/`
- `Source/DreamOfPadma/Public/MVP/UI/RulerStatus/`
- `Source/DreamOfPadma/Private/MVP/UI/RulerStatus/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-026/`
- `Content/Padma/MVP/Definitions/RulerAI/`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-026-Ruler-World-AI.md`
- `Docs/Production/Tasks/TASK-026-Ruler-World-AI.zh-CN.md`

Exclusions: LLM-driven AI, ACT enemy locomotion, a full strategic planner and animation-notify-owned decisions. Other task source/content, shared configuration not named above, the reference prototype, generated folders and new standalone UE modules are outside scope. Existing Demo code is consumed through public contracts; adaptation/migration needs an explicitly named write set.

## Dependencies, handoff and conflicts

- Required predecessors: [TASK-005](TASK-005-Deterministic-Random-Foundation.md), [TASK-015](TASK-015-Combat-Calculation-Contracts.md), [TASK-018](TASK-018-Calendar-and-Phase-Scheduler.md), [TASK-019](TASK-019-Resource-and-Global-Ledgers.md), [TASK-024](TASK-024-Sandbox-Graph-and-Movement.md), [TASK-025](TASK-025-Node-Operations-and-Story.md).
- Conditional predecessor: TASK-005 before any random outcome where it is not already listed.
- Hand off the reviewed public commands, read-only views, definition fixtures and test evidence. A state owner implements TASK-012 snapshot participation; UI never owns mutable gameplay state.
- Module README pairs, shared headers, maps, binary assets and central files are serial leases. Do not edit while another owner/build consumes them. One UE build/Editor/PIE lane.
- No writable subagent is authorized. One bounded read-only architect/reviewer may inspect the relevant contract/diff. The Primary Agent remains accountable; the Coordinator owns shared integration state and Changelog.

## Acceptance criteria

- [ ] Consume approved entropy/calamity/world events without recomputing TASK-019's attribution or inventing extra penalties; resulting ruler changes are observable and snapshot-restorable.
- [ ] Record detection, threat progression, candidate priorities and response conditions for the first ruler profile.
- [ ] One patrol, one hidden approach, one detected approach, reinforcement and counterattack can be triggered by authored fixtures.
- [ ] AI reads public world views, submits normal commands and exposes candidate/reason/selected-action traces; it never edits World internals.
- [ ] Daily/event threat changes and random tie decisions are reproducible; snapshot recovery restores patrol, pending arrivals/responses and stream positions.
- [ ] Exact write set, synchronized task/module documentation, required checks and independent review pass; no unresolved rule has been filled by a default.

## Verification and evidence

Policy fixtures with same-seed decisions and no-legal-action behavior; user advances the world and observes visible ruler responses.

Compile the Editor target when native code changes. For the implemented narrow automation, use Scripts/RunTests.ps1 with the verified EngineRoot and TestFilter `DreamOfPadma.MVP.TASK026`; confirm reported cases actually passed, not only process exit. Run Scripts/AuditDocs.ps1, applicable Scripts/ValidateProject.ps1 -Strict and scoped git diff --check. Do not add broad UI automation merely for this task. Record reproducible user PIE observations (written evidence is acceptable), input/profile IDs and logs; images/video may supplement them. No runtime/manual success is claimed by this contract.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| Explainable deterministic AI | Predict which candidate wins after a threat event, inspect the trace and explain its priority. | Not started |

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
