# TASK-003 Core Rule Contract Slice

- Chinese companion for user reading: `TASK-003-Core-Contracts.zh-CN.md`
- Document ID: `TASK-003`
- Version: `0.1`
- Status: `Done`
- Parent milestone or integration Goal: `M1 Core rules / first core-rule foundation`
- Primary Role: `Integration Coordinator`
- Primary Agent: `Integration Coordinator` for this serial contract-freeze Goal; downstream implementation Agents are assigned only after this contract is approved
- Branch/worktree: `main`, integration-owned checkout; no independent worktree
- Task mode: design and contract freeze only; this task authorizes no code or asset implementation

## Goal

Freeze the smallest shared, presentation-free contract for the first M1 core-rule foundation so that later Calendar, Resource Ledger, and Deterministic Random Goals can be implemented and reviewed without redefining ownership or public semantics.

The observable result is a bilingual task contract and downstream test matrix covering only already-frozen concepts: stable identity and value semantics, explicit calendar state and events, separate resource-ledger semantics, deterministic calculation context and trace metadata, and typed command/result/error/event boundaries. This task does not implement the Calendar, Resource Ledger, or Deterministic Random runtime features.

## Why now

The MVP design and architecture baseline is accepted, but the current project still has only the generated `DreamOfPadma` module and no project-owned gameplay test suite. Shared Core contracts must be recorded before downstream implementation Goals fan out; otherwise Calendar, resource, and random work can silently choose incompatible state, error, event, or dependency shapes.

This is the serial prerequisite for the first M1 implementation wave and preserves the project rule that a worktree belongs to an independently mergeable Goal, not to a logical module by default.

## Read first

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Production/Tasks/TASK-002-Agent-Workflow-Bootstrap.md`
- `Docs/Rules/Time.md`
- `Docs/Rules/WorldState.md`
- `Docs/Design/EN/02_SystemPlanner.md`
- `Docs/Design/EN/05_NumericalPlanner.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/ModuleMap.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/SaveSchema.md`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaTests/README.md`

## Decision state

### Frozen inputs

The following are the only semantics this contract may freeze:

| Contract area | Frozen boundary |
|---|---|
| Core ownership | Core rules and contracts remain independent from UMG, Niagara, concrete Actors, maps, input devices, and presentation state. The current generated `DreamOfPadma` module is the staging home; no new UE module is created by this task. |
| Identity and state | Stable IDs are separate from display names, localization, asset paths, Actor pointers, and map coordinates. Static definitions and mutable run state remain separate. IDs are value-like and serializable; this task does not choose a new naming namespace. |
| Calendar | Calendar state stores explicit `ChapterIndex`, `EraIndex`, `DayIndex`, and `Phase`. The fixed hierarchy is Dawn → Morning → Dusk, seven days per Era, and seven Eras per Chapter. Phase transitions are explicit events; Era and Chapter changes have separate events. |
| Resources | `Flow` and `Computation` are separate ledgers. Current value, cap, debt representation, source, and reason must not be collapsed into one unexplained number. No product default, debt policy, daily gain, or conversion loop is chosen here. |
| Determinism | A reproducible calculation context can identify the run/master seed, named stream, command or event position, input definition IDs and versions where required, and roll identifiers/values in debug or test builds. Candidate selection and success/failure remain separate random steps when a later synthesis task consumes this seam. |
| Command path | State-changing work follows command → validation → authoritative state change → typed result and explicit event(s). Invalid commands expose a readable failure. UI, GM tools, automation tests, and future network callers use the same public path. |
| Test boundary | Tests consume production contracts and rule paths; they must not create a second fake rules implementation or depend on map loading/widgets when a Core-level test is sufficient. |

### Deferred or open inputs

This task records these as non-binding inputs and does not resolve them:

- Initial values and caps for resources and core values; debt limits, daily gains, conversion costs, loop safeguards, and any stochastic compensation case.
- Zero-based versus one-based index convention if it is not already established by an existing implementation; serialization shape and definition-version policy beyond the requirements already stated by the architecture.
- Final stable-ID namespace and content naming convention, and the final CSV/JSON/DataTable/Primary Data Asset or mixed import pipeline.
- Synthesis candidate-filter semantics, Softmax filtering/normalization details, formula coefficients, probability clamp and units, B-field timing/cost, no-shared-tag fallback, and Sigma dice/consequences.
- Card lifecycle probabilities, ABC active-ability legality inside local battles, Encounter action-bar precedence and window card allowance, and deferred ACT page/toggle/boundary details.
- Combat, world graph, battle transaction, outcome, save migration, UI, and presentation implementation details.

If an implementation needs one of these decisions to satisfy a contract, stop and return the issue to the relevant System Planner, Numerical Planner, Architect, or user/design owner. Do not add a default to keep the task moving.

## Scope

### Allowed paths and write set

This contract-design Goal may edit exactly these paths:

- `Docs/Production/Tasks/TASK-003-Core-Contracts.md`
- `Docs/Production/Tasks/TASK-003-Core-Contracts.zh-CN.md`
- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`
- `Docs/ProjectState.md`
- `Docs/ProjectState.zh-CN.md`

No `Source/`, `Content/`, `Config/`, `Build/`, or generated path is writable under this task. The future implementation paths below are handoff references only, not permission to edit them in this Goal:

- `Source/DreamOfPadma/Public/Core/`
- `Source/DreamOfPadma/Private/Core/`
- `Source/DreamOfPadma/Private/Tests/Core/`
- `Docs/Architecture/Modules/PadmaCore/README.md` and `README.zh-CN.md`

### Out of scope

- Any C++, Blueprint, UE module, `Build.cs`, Config, map, `.uasset`, or content implementation.
- Calendar, resource-ledger, or random-stream runtime code; automation test code; data assets; import/validation tooling; save code; or a new module split.
- Card-definition implementation, card piles/lifecycle, synthesis service, preview/execution parity, Softmax, success formula, Sigma dice, or failure-side content.
- Encounter, ACT/RealTimeAction, combat, world graph, node mutation, battle-route, battle transaction, outcome, UI, audio, VFX, camera, or input implementation.
- Choosing numerical defaults, probability clamps, ID naming conventions, import formats, serialization formats, or any other Open/Deferred rule.
- Creating downstream task files or worktrees, changing canonical design/rule/architecture meaning, merging, tagging, pushing, or packaging.

## Dependencies and integration order

- Required predecessor: `TASK-002-Agent-Workflow-Bootstrap` is `Done`; the accepted MVP and architecture baseline dated 2026-09-02 is the design authority.
- Direct inputs: the English rule, design, architecture, module, and learning documents listed under **Read first**. Chinese companions mirror the English sources and do not add semantics.
- Downstream consumers: the future Calendar, Resource Ledger, and Deterministic Random Goals; later Card/Synthesis, World, Gameplay, Save, UI, and PadmaTests work.
- Merge order: review this contract and obtain user/design-owner approval before any downstream writer worktree is created. A public-contract change returns to this serial integration Goal before implementation continues.
- Shared conflicts: public Core headers, central `DreamOfPadma.Build.cs`, module dependencies, public data schemas, `Docs/ProjectState.md`, task indexes, module READMEs, and the single UE build/Editor/PIE lane remain integration-owned or serial. No concurrent writer is authorized for these paths by this task.
- Ownership questions route to the role named by the source document: System Planner for rule meaning, Numerical Planner for coefficients/tables, Architect for data/dependency/schema boundaries, and the user/design owner for unresolved gameplay meaning.

## Independent worktree decision

**No. This Goal is not suitable for an independent worktree.** It freezes shared public contracts, dependency direction, task indexes, and project-state routing that downstream Goals must consume consistently. Keep it on the integration-owned checkout (`main`) with one writer. After this contract is reviewed and approved, Calendar, Resource Ledger, and Deterministic Random may be evaluated as separate worktree Goals only if their write sets remain exclusive and their merge order is explicit.

## Delegation plan

No writable subagent is authorized. The Integration Coordinator remains accountable for the contract and may request these bounded read-only checks before approval:

| Role | Work package | Authority | Expected output |
|---|---|---|---|
| System Planner | Check that the frozen calendar/resource/determinism statements match accepted rule meaning and that every unresolved input remains unresolved. | Read-only | Invariant and open-item review with source references. |
| Architect | Check Core ownership, dependency direction, public-contract scope, save/replay seams, and the proposed downstream write sets. | Read-only | Blockers, ADR needs, and integration-order recommendation. |
| Learning Tutor | Check that the two learning targets require user-produced evidence and remain limited to this slice. | Read-only | Exercise, prediction, teach-back, and transfer review. |

## Acceptance criteria

- [x] The English and Chinese task files exist as a synchronized pair with the same Document ID, version, status, and acceptance boundary.
- [x] The Goal is one observable serial result: a frozen shared Core contract and downstream test matrix; no runtime implementation is implied.
- [x] Stable-ID/value semantics, explicit calendar state/events, separate Flow/Computation ledgers, deterministic trace metadata, and command/result/error/event boundaries are stated without inventing numeric or gameplay policy.
- [x] Every Open/Deferred input that could affect this slice is listed as non-binding, with an escalation route; none is converted into a default or hidden rule.
- [x] The downstream test obligations cover calendar boundaries, resource transaction isolation/provenance, deterministic reproduction, and readable command failures without depending on UI or maps.
- [x] The exact write set, serial conflict list, dependency order, Primary Agent ownership, and no-independent-worktree decision are explicit.
- [x] `Docs/00_INDEX.md` and `Docs/00_INDEX.zh-CN.md` link the new task, and the corresponding project-state entries remain synchronized.
- [x] The current design-only turn changes no source, asset, config, build, generated, or test implementation file.

## Tests

This task is contract-only. It runs documentation and scope checks, not gameplay or UE automation tests. The following matrix is the required test contract for the downstream implementation Goals:

| Future slice | Required test | Expected observation | Constraint |
|---|---|---|---|
| Calendar | Exercise the three phase transitions and the first/seventh-day, next-Era, and next-Chapter boundaries. | Explicit calendar fields advance coherently and phase/Era/Chapter events are emitted at the proper boundaries. | Do not replace explicit state with a modulo-only gameplay calculation; do not assume an unapproved index origin. |
| Resource Ledger | Apply valid and invalid transactions to Flow and Computation separately. | One ledger cannot mutate the other; the result/event records delta, source, reason, and command identity; invalid input leaves authoritative state unchanged. | Use named test fixtures, not product defaults; do not invent debt or loop policy. |
| Deterministic Random | Repeat the same fixture with the same seed, stream, position, and inputs. | The reproducible result/trace matches, including the identity metadata and roll evidence required for debug/test builds. | Do not couple unrelated streams or choose a synthesis formula in this task. |
| Shared command path | Exercise a valid command and a validation failure through the same public service path. | Results and explicit events describe the state change; failure is readable and has no hidden mutation. | Tests call production contracts and must not duplicate rule logic. |

### Checks for this contract-design task

- `pwsh -NoProfile -File .\Scripts\AuditDocs.ps1` — documentation pairs and links pass.
- `pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict` — project structure remains valid.
- `git diff --check` — no whitespace errors.
- A changed-path audit confirms that only the six allowed Markdown paths above changed.
- Manual review confirms the English/Chinese pair has the same semantics and that no Open/Deferred item was resolved.

## Verification plan

### Automated checks

- Run the three commands under **Checks for this contract-design task** from the repository root.
- Inspect `git diff --name-only` and compare it with the exact allowed write set.
- Search the final diff for `Source/`, `Content/`, `Config/`, `Build/`, generated folders, numeric defaults, and language that turns an Open/Deferred item into a decision.

### Manual checks

- Trace every frozen statement to an accepted English rule or architecture source.
- Read the Chinese companion against the English file and verify the Document ID, version, status, links, and acceptance boundary match.
- Walk the proposed Calendar, Resource Ledger, and Deterministic Random downstream Goals and confirm they can receive non-overlapping write sets only after this contract is approved.
- Confirm that no independent worktree is allocated to this shared public-contract Goal.

### Required evidence

- Audit, validation, and `git diff --check` output.
- Exact changed-path list.
- Coordinator review of frozen versus open/deferred statements.
- User/design-owner approval before any downstream implementation Goal or worktree is created.

## Learning contract

Choose no more than two primary targets.

| Target | Evidence required | Status |
|---|---|---|
| Contract-first ownership and integration routing | The user predicts why shared public contracts stay serial, identifies the owner of one boundary, and routes Calendar/Resource/Random work to later Goals without overlapping write sets. | `Not started` |
| Deterministic fixture reasoning | The user predicts what must remain identical when seed, named stream, position, and inputs repeat, then explains how the trace exposes a mismatch. | `Not started` |

- Learner's hands-on exercise: before downstream implementation, draw the command → validation → state → result/event path for one resource transaction and predict the seventh-day boundary plus a repeated deterministic fixture; later perform the named automation test and inspect its output.
- Transfer question: if a future synthesis task needs a new public field or changes the meaning of a resource, should it patch this contract in place, create a new owned task, or request an ADR—and what evidence would decide?

## Risks and recovery

- Scope drift could turn a shared contract freeze into Calendar, resource, random, or synthesis implementation. Recovery: split the request into a new owned Goal and keep this task at the contract boundary.
- An Open/Deferred rule may appear necessary during implementation. Recovery: stop, record the exact dependency and decision owner, and return it to serial integration; do not guess.
- Public headers or central configuration could be edited concurrently. Recovery: keep this Goal serial and rebase/merge only through the Integration Coordinator after review.
- The final stable-ID naming and import/version policies are not fully fixed. Recovery: use only opaque test fixtures and request a separate decision task when production content needs the policy.
- Safe rollback is a Git revert of this documentation pair, index pair, and project-state pair; no destructive filesystem operation is needed.

## Completion report

- Integration boundary note: prerequisite checkpoint `9bf49fa` also carried the user's pre-existing TASK-001 bilingual PIE-completion edits. Those two files are separately identified in that checkpoint and were not TASK-003-owned writes. The TASK-003 closure diff is restricted to this task's allowed paths.
- Final status: `Done`; contract reviewed and approved, implementation not started.
- Primary Agent and Role: current Integration Coordinator / Integration Coordinator.
- Changed files: `TASK-003-Core-Contracts.md`, `TASK-003-Core-Contracts.zh-CN.md`, `Docs/00_INDEX.md`, `Docs/00_INDEX.zh-CN.md`, `Docs/ProjectState.md`, and `Docs/ProjectState.zh-CN.md`.
- Acceptance evidence: bilingual task pair, index links, and synchronized project-state entry are present; the TASK-003 closure diff contains only TASK-003 allowed Markdown paths, with no source, asset, config, build, generated, or test implementation file changed. The earlier checkpoint's separate TASK-001 pair is recorded above and remains outside this task's write set.
- Checks run and results: `Scripts/AuditDocs.ps1` passed for 108 Markdown files and 54 language pairs; `Scripts/ValidateProject.ps1 -Strict` passed; `git diff --check` and `git diff --cached --check` passed with no output; exact staged-scope audit passed for the TASK-003 closure paths.
- Checks not run and reason: gameplay, UE automation, compile, PIE, packaging, and implementation tests are intentionally not run because this is a design-only task.
- Review findings resolved or accepted: independent read-only Review identified the pre-existing TASK-001 pair carried by checkpoint `9bf49fa`; the boundary is explicitly recorded above, and the TASK-003 closure diff was re-reviewed with no blocking finding. The user approved the contract on 2026-09-07.
- Remaining risks and open questions: all items under **Deferred or open inputs** remain unresolved by design.
- English/Chinese documentation updated: yes.
- Agent-produced learning evidence: task contract and test/learning plan only.
- User-produced learning evidence: pending the downstream hands-on exercise.
- Integration commit or handoff reference: TASK-003 closure commit; the resulting hash is reported in the Integration Coordinator handoff. No merge or push is part of this task.
