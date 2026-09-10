# TASK-014 Character Rendering and Animation

- Chinese companion: TASK-014-Character-Presentation.zh-CN.md
- Document ID: TASK-014
- Version: 0.2
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: Character Presentation
- Primary Agent: one designated owner at Ready; no writer started
- Branch/worktree: assign a Local feature branch at implementation preflight; no worktree by default

## Goal and why now

Render the first player and enemy with reusable visual components, basic materials and idle/move/hit/death animation driven by authoritative view/events. This extends the reviewed predecessors through one bounded delivery result. It must not turn their temporary fixtures into unapproved game rules.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, [TASK-011](TASK-011-Full-MVP-Task-Cluster.md), [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), [MVP decisions](../MVPDecisionRegister.md), and the [PadmaUI contract](../../Architecture/Modules/PadmaUI/README.md). Also read `Docs/Content/Naming.md`, `Content/AGENTS.md`, the predecessor contracts, and nested Source/Content/Docs instructions for the paths below.

Source: [PDF card, rule and screen extraction](../MVPSourceExtraction.md). Old contract-card sandbox summon effects are excluded from the first version; missing ACT definitions are not filled by converting turn-based cards.

## Decisions and readiness

- Frozen: the accepted MVP/rules, separate Encounter/ACT GAS systems and tables, separate turn-based/ACT character-card collections, and shared basic-skill identity/slot mapping with mode-owned effects.
- Frozen: placeholder assets will be imported by the user; final animation/rendering polish is later work. Every unaccepted numerical value requires individual user confirmation before use, including test profiles.
- Gate references: D02 See the register for the owner and exact missing information.
- Backlog describes an approved planning item, not a runnable implementation task. Record gate answers in the canonical rule/architecture source and this report, complete predecessors, then promote to Ready. Technical schemas are reviewed by the Architect; gameplay and numerical choices belong to the user.
- If gate closure requires a normative pair or structural ADR outside this write set, the Coordinator must add those exact paths before Ready; this is planned preflight, not permission for arbitrary edits.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/Presentation/Characters/`
- `Source/DreamOfPadma/Private/MVP/Presentation/Characters/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-014/`
- `Content/Padma/MVP/Presentation/Characters/`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-014-Character-Presentation.md`
- `Docs/Production/Tasks/TASK-014-Character-Presentation.zh-CN.md`

Exclusions: Final animation detail/rendering polish, combat authority, ACT controller and broad prototype migration. Other task source/content, shared configuration not named above, the reference prototype, generated folders and new standalone UE modules are outside scope. Existing Demo code is consumed through public contracts; adaptation/migration needs an explicitly named write set.

## Dependencies, handoff and conflicts

- Required predecessors: [TASK-012](TASK-012-Run-State-and-Contracts.md), [TASK-040](TASK-040-Model-Assets-and-Scene-Bindings.md).
- Conditional predecessor: TASK-005 before any random outcome where it is not already listed.
- Hand off the reviewed public commands, read-only views, definition fixtures and test evidence. A state owner implements TASK-012 snapshot participation; UI never owns mutable gameplay state.
- Module README pairs, shared headers, maps, binary assets and central files are serial leases. Do not edit while another owner/build consumes them. One UE build/Editor/PIE lane.
- No writable subagent is authorized. One bounded read-only architect/reviewer may inspect the relevant contract/diff. The Primary Agent remains accountable; the Coordinator owns shared integration state and Changelog.

## Acceptance criteria

- [ ] Use the placeholder assets imported by the user after implementation starts, once their exact paths are recorded; wrap external content under Content/Padma and keep source packs unchanged.
- [ ] Both silhouettes, lighting and scale are legible; idle/move/hit/death events show the correct animation without authoring combat changes in animation.
- [ ] Spawn/despawn and scene re-entry clean up visual attachments and delegates; the component can be attached to either later mode without sharing its gameplay state.
- [ ] Exact write set, synchronized task/module documentation, required checks and independent review pass; no unresolved rule has been filled by a default.

## Verification and evidence

Asset-reference validation and Editor compile; user-observed animation sequence, camera framing and repeated spawn/despawn in PIE.

Compile the Editor target when native code changes. For the implemented narrow automation, use Scripts/RunTests.ps1 with the verified EngineRoot and TestFilter `DreamOfPadma.MVP.TASK014`; confirm reported cases actually passed, not only process exit. Run Scripts/AuditDocs.ps1, applicable Scripts/ValidateProject.ps1 -Strict and scoped git diff --check. Do not add broad UI automation merely for this task. Record reproducible user PIE observations (written evidence is acceptable), input/profile IDs and logs; images/video may supplement them. No runtime/manual success is claimed by this contract.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| Presentation consumes state | Trigger a test hit event, predict the animation, then explain why a missing animation cannot cancel damage. | Not started |

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

## Local scene presentation supplement

TASK-037 consumes these components for both characters and constructions. Extend the Goal to construction models and authored damaged/destroyed feedback. Add exclusive Public/Private/MVP/Presentation/Buildings/ source and Content/Padma/MVP/Presentation/Buildings/ to this task's write set. Animations/material changes consume authoritative events; missing animation cannot cancel an effect. Validate imported model/animation references and cleanup on repeated scene entry. No combat values or actions are chosen here.

- `Source/DreamOfPadma/Public/MVP/Presentation/Buildings/`
- `Source/DreamOfPadma/Private/MVP/Presentation/Buildings/`
- `Content/Padma/MVP/Presentation/Buildings/`

## 2026-09-09 confirmed model/movement handoff

Consume TASK-040's neutral loaded presentation descriptor/Actor. General profiles support character/building/ground models; Game adapts existing ACT/Demo sources without duplicate authority. This task still owns real animation/event integration and user-imported art; a graybox authoring preview does not meet combat-animation acceptance.
