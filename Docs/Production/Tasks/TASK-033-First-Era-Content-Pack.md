# TASK-033 First Era Content Pack

- Chinese companion: TASK-033-First-Era-Content-Pack.zh-CN.md
- Document ID: TASK-033
- Version: 0.2
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: Level / Content / Numerical
- Primary Agent: one designated owner at Ready; no writer started
- Branch/worktree: assign a Local feature branch at implementation preflight; no worktree by default

## Goal and why now

Assemble and validate one authored seven-day content pack that exercises the implemented economy, synthesis, sandbox, distinct battle modes and outcomes. This extends the reviewed predecessors through one bounded delivery result. It must not turn their temporary fixtures into unapproved game rules.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, [TASK-011](TASK-011-Full-MVP-Task-Cluster.md), [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), [MVP decisions](../MVPDecisionRegister.md), and the [PadmaWorld contract](../../Architecture/Modules/PadmaWorld/README.md). Also read `Docs/Design/EN/01_MainPlanner.md`, `Docs/Design/EN/04_LevelContentPlanner.md`, `Docs/Design/EN/05_NumericalPlanner.md`, the predecessor contracts, and nested Source/Content/Docs instructions for the paths below.

Source: [PDF card, rule and screen extraction](../MVPSourceExtraction.md). Old contract-card sandbox summon effects are excluded from the first version; missing ACT definitions are not filled by converting turn-based cards.

## Decisions and readiness

- Frozen: the accepted MVP/rules, separate Encounter/ACT GAS systems and tables, separate turn-based/ACT character-card collections, and shared basic-skill identity/slot mapping with mode-owned effects.
- Frozen: placeholder assets will be imported by the user; final animation/rendering polish is later work. Every unaccepted numerical value requires individual user confirmation before use, including test profiles.
- Gate references: D02, D03, D11, D12, D13, D18, D19, D22, D23 See the register for the owner and exact missing information.
- Backlog describes an approved planning item, not a runnable implementation task. Record gate answers in the canonical rule/architecture source and this report, complete predecessors, then promote to Ready. Technical schemas are reviewed by the Architect; gameplay and numerical choices belong to the user.
- If gate closure requires a normative pair or structural ADR outside this write set, the Coordinator must add those exact paths before Ready; this is planned preflight, not permission for arbitrary edits.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/ContentValidation/FirstEra/`
- `Source/DreamOfPadma/Private/MVP/ContentValidation/FirstEra/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-033/`
- `Content/Padma/MVP/ContentPacks/FirstEra/`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaWorld/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-033-First-Era-Content-Pack.md`
- `Docs/Production/Tasks/TASK-033-First-Era-Content-Pack.zh-CN.md`

Exclusions: New gameplay mechanics, modifying another task's map/templates, full 49-day content and unapproved production art migration. Other task source/content, shared configuration not named above, the reference prototype, generated folders and new standalone UE modules are outside scope. Existing Demo code is consumed through public contracts; adaptation/migration needs an explicitly named write set.

## Dependencies, handoff and conflicts

- Required predecessors: [TASK-023](TASK-023-Synthesis-UI.md), [TASK-025](TASK-025-Node-Operations-and-Story.md), [TASK-026](TASK-026-Ruler-World-AI.md), [TASK-029](TASK-029-Safe-Boundary-Save-Load.md), [TASK-031](TASK-031-ACT-Repository-and-Bullet-Time.md), [TASK-032](TASK-032-Run-Outcomes-and-Restart.md), [TASK-035](TASK-035-ACT-Character-Cards-and-Roster.md), [TASK-037](TASK-037-Node-Inspection-and-Local-Scenes.md), [TASK-038](TASK-038-Global-Preparation-and-Bonus-UI.md), [TASK-039](TASK-039-Story-Relationships-and-Bonus-State.md).
- Conditional predecessor: TASK-005 before any random outcome where it is not already listed.
- Hand off the reviewed public commands, read-only views, definition fixtures and test evidence. A state owner implements TASK-012 snapshot participation; UI never owns mutable gameplay state.
- Module README pairs, shared headers, maps, binary assets and central files are serial leases. Do not edit while another owner/build consumes them. One UE build/Editor/PIE lane.
- No writable subagent is authorized. One bounded read-only architect/reviewer may inspect the relevant contract/diff. The Primary Agent remains accountable; the Coordinator owns shared integration state and Changelog.

## Acceptance criteria

- [ ] Author two user-approved reproducible acceptance routes: low-entropy safe play and high-entropy benefit/calamity play, including attribution to the causing side and visible world/ruler consequences. This does not turn a synthesis failure into a reward.
- [ ] Provide at least two A/two B/two C, basic battle cards sufficient for the approved repository presentation, a non-reward failure definition, player/ruler cores and a boss.
- [ ] Provide approximately eight nodes with required node types, one conditional route, one terrain effect, anecdote choices, recovery/furnace and ruler patrol/response fixtures.
- [ ] Author separate Encounter and ACT catalog/table instances and mode bindings, with consistent card identity and independently validated mode data.
- [ ] A seven-day pacing route includes synthesis success/no-reward failure, economy changes and each battle mode; both victory fixtures and an approved defeat are reachable.
- [ ] Version the profile and record seeds, costs, balance observations, asset-reference validation and basic animation/VFX/audio readability; data-only tuning does not alter rules.
- [ ] Exact write set, synchronized task/module documentation, required checks and independent review pass; no unresolved rule has been filled by a default.

## Verification and evidence

Content-ID/reference/mode-binding validation and bounded balance fixtures; play the authored route and record one decision's visible world consequence.

Compile the Editor target when native code changes. For the implemented narrow automation, use Scripts/RunTests.ps1 with the verified EngineRoot and TestFilter `DreamOfPadma.MVP.TASK033`; confirm reported cases actually passed, not only process exit. Run Scripts/AuditDocs.ps1, applicable Scripts/ValidateProject.ps1 -Strict and scoped git diff --check. Do not add broad UI automation merely for this task. Record reproducible user PIE observations (written evidence is acceptable), input/profile IDs and logs; images/video may supplement them. No runtime/manual success is claimed by this contract.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| Content configuration versus mechanics | Change an allowed test-profile value, predict its consequence and identify which rules remain the same. | Not started |

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

## Screen and relationship content acceptance

Include the user-approved node-local scene/model mappings, hover visibility fixture, global preparation content and minimum named relationship/bonus path from TASK-037/038/039. Inspect a modeled character/construction, see the same world state after returning, and trace one approved relationship source to its actual target and bonus overview. Exact NPC, layout, content and numerical values remain D12/D19/D24–D28. Do not treat the conversation mockup as a gameplay fixture or silently add FPS combat.
