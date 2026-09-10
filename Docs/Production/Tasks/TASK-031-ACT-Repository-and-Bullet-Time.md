# TASK-031 ACT Repository and Bullet Time

- Chinese companion: TASK-031-ACT-Repository-and-Bullet-Time.zh-CN.md
- Document ID: TASK-031
- Version: 0.1
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: ACT Input / UI
- Primary Agent: one designated owner at Ready; no writer started
- Branch/worktree: assign a Local feature branch at implementation preflight; no worktree by default

## Goal and why now

Integrate the shared card identity/slot view with the ACT-only Tab repository window, background blur, 1/10 world time and legal numeric-slot dispatch. This extends the reviewed predecessors through one bounded delivery result. It must not turn their temporary fixtures into unapproved game rules.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, [TASK-011](TASK-011-Full-MVP-Task-Cluster.md), [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), [MVP decisions](../MVPDecisionRegister.md), and the [PadmaUI contract](../../Architecture/Modules/PadmaUI/README.md). Also read `Docs/Rules/Combat.md`, `Docs/Architecture/RuntimeFlow.md`, the predecessor contracts, and nested Source/Content/Docs instructions for the paths below.

Source: [PDF card, rule and screen extraction](../MVPSourceExtraction.md). Old contract-card sandbox summon effects are excluded from the first version; missing ACT definitions are not filled by converting turn-based cards.

## Decisions and readiness

- Frozen: the accepted MVP/rules, separate Encounter/ACT GAS systems and tables, separate turn-based/ACT character-card collections, and shared basic-skill identity/slot mapping with mode-owned effects.
- Frozen: placeholder assets will be imported by the user; final animation/rendering polish is later work. Every unaccepted numerical value requires individual user confirmation before use, including test profiles.
- Gate references: D01, D09, D17, D23 See the register for the owner and exact missing information.
- Backlog describes an approved planning item, not a runnable implementation task. Record gate answers in the canonical rule/architecture source and this report, complete predecessors, then promote to Ready. Technical schemas are reviewed by the Architect; gameplay and numerical choices belong to the user.
- If gate closure requires a normative pair or structural ADR outside this write set, the Coordinator must add those exact paths before Ready; this is planned preflight, not permission for arbitrary edits.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/Gameplay/ACT/RepositoryWindow/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/ACT/RepositoryWindow/`
- `Source/DreamOfPadma/Public/MVP/UI/ACTRepository/`
- `Source/DreamOfPadma/Private/MVP/UI/ACTRepository/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-031/`
- `Content/Padma/MVP/ACT/Repository/`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-031-ACT-Repository-and-Bullet-Time.md`
- `Docs/Production/Tasks/TASK-031-ACT-Repository-and-Bullet-Time.zh-CN.md`

Exclusions: Applying Encounter's per-turn cap to ACT, hard-pausing physics, changing Encounter clocks and guessing dismissal/page boundaries. Other task source/content, shared configuration not named above, the reference prototype, generated folders and new standalone UE modules are outside scope. Existing Demo code is consumed through public contracts; adaptation/migration needs an explicitly named write set.

## Dependencies, handoff and conflicts

- Required predecessors: [TASK-020](TASK-020-Card-Catalog-and-Lifecycle.md), [TASK-030](TASK-030-ACT-GAS-Playable-Loop.md).
- Conditional predecessor: TASK-005 before any random outcome where it is not already listed.
- Hand off the reviewed public commands, read-only views, definition fixtures and test evidence. A state owner implements TASK-012 snapshot participation; UI never owns mutable gameplay state.
- Module README pairs, shared headers, maps, binary assets and central files are serial leases. Do not edit while another owner/build consumes them. One UE build/Editor/PIE lane.
- No writable subagent is authorized. One bounded read-only architect/reviewer may inspect the relevant contract/diff. The Primary Agent remains accountable; the Coordinator owns shared integration state and Changelog.

## Acceptance criteria

- [ ] Record toggle/dismissal/re-entry behavior, page count/boundaries, card selection/end-window behavior and cost/cooldown time policy before implementation.
- [ ] Tab visibly opens a blurred five-card panel and sets world time to 1/10; movement/attack requests are rejected while allowed card/panel controls operate.
- [ ] The scene retains inertia; numeric keys map to the current page's 1–5 slots, the wheel requests the next page, and the authoritative mode validator rejects invalid input.
- [ ] The shared card binding resolves the ACT definition rather than an Encounter ability; lifecycle/payment uses approved owners and does not duplicate side effects.
- [ ] Dismissal, death, defeat, exit, travel and load restore time scale, blur and input state, including nested/error paths; repeat the full battle loop with card use.
- [ ] Exact write set, synchronized task/module documentation, required checks and independent review pass; no unresolved rule has been filled by a default.

## Verification and evidence

State/input/time-restoration/page-boundary tests and controlled inertia observations; user completes Tab, slot, page, exit/error and battle-result routes.

Compile the Editor target when native code changes. For the implemented narrow automation, use Scripts/RunTests.ps1 with the verified EngineRoot and TestFilter `DreamOfPadma.MVP.TASK031`; confirm reported cases actually passed, not only process exit. Run Scripts/AuditDocs.ps1, applicable Scripts/ValidateProject.ps1 -Strict and scoped git diff --check. Do not add broad UI automation merely for this task. Record reproducible user PIE observations (written evidence is acceptable), input/profile IDs and logs; images/video may supplement them. No runtime/manual success is claimed by this contract.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| Input gates and separate clocks | Predict which inputs and clocks continue in bullet time, then verify them and explain restoration. | Not started |

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
