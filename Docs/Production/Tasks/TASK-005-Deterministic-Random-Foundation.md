# TASK-005 Deterministic Random Foundation

- Chinese companion for user reading: `TASK-005-Deterministic-Random-Foundation.zh-CN.md`
- Document ID: `TASK-005`
- Version: `0.1`
- Status: `Review`
- Parent milestone or integration Goal: `M1 Core rules / first implementation slice`
- Primary Role: `Module Agent`
- Primary Agent: `module_worker`, assigned only after this contract is approved as `Ready`
- Branch/worktree: `feature/TASK-005-deterministic-random`, Local feature branch; no independent Worktree under the current one-feature/one-writer policy
- Task mode: bounded Core implementation and automation-test slice; no gameplay formula or content implementation

## Goal

Implement the smallest presentation-free deterministic-random context and trace boundary inside the existing `DreamOfPadma` module, with the first project-owned Unreal automation tests. Prove that the same master seed, named stream, draw position, and fixture identity reproduce the same sequence and trace, while named streams remain isolated. Do not implement synthesis, calendar, resource, save, network, or gameplay-probability semantics.

The observable result is a compilable Core slice and repeatable automation evidence that later rule services can consume without depending on UI, Actors, maps, or a universal Manager.

## Why now

TASK-003 is closed and freezes deterministic calculation metadata, including seed/stream/position and input identity where required. The architecture and numerical baseline require reproducible outcomes, but the repository has no project-owned gameplay automation tests yet. A narrow deterministic fixture is the lowest-risk first implementation slice and can support later synthesis, combat, AI, calendar, and replay work without choosing their open rules.

## Read first

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Production/Tasks/TASK-003-Core-Contracts.md`
- `Docs/Rules/Synthesis.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/SaveSchema.md`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaTests/README.md`
- `Source/AGENTS.md`
- `.codex/agents/module-worker.toml`

## Decision state

### Frozen inputs

- Core contracts remain independent from UMG, Niagara, concrete Actors, maps, input devices, and presentation state.
- A random calculation records a master/run seed, a named stream, an event or command position, input definition identity/version where required, and roll evidence in debug/test builds.
- Repeating the same deterministic fixture with the same seed, stream, position, and inputs must reproduce the same result; candidate selection and success/failure are separate future random steps.
- The implementation remains inside the current generated `DreamOfPadma` module. No standalone UE module, plugin, GAS dependency, asset, or central configuration change is authorized.
- The first test must consume the production boundary and must not create a second fake rule implementation.

### Deferred or open inputs

- Final stable-ID namespace, production content naming, and import/version policy.
- The long-term random algorithm/version guarantee, cross-engine-version replay compatibility, and save serialization shape.
- Gameplay distributions, roll counts, ranges, probability clamps, synthesis candidate filtering, Softmax, success formulas, Sigma consequences, combat formulas, AI policy, and calendar/resource policies.
- Network authority, preview/execution integration, persistence, telemetry format, and any rule-specific stream allocation policy.

These items remain owned by their named design, numerical, architecture, or user owners. If this slice needs one of them to pass acceptance, stop and return the decision; do not add a default.

## Scope

### Allowed paths and write set

The `module_worker` Primary Agent may edit exactly:

- `Source/DreamOfPadma/Public/Core/Random/PadmaDeterministicRandom.h`
- `Source/DreamOfPadma/Private/Core/Random/PadmaDeterministicRandom.cpp`
- `Source/DreamOfPadma/Private/Tests/Core/PadmaDeterministicRandomTests.cpp`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-005-Deterministic-Random-Foundation.md`
- `Docs/Production/Tasks/TASK-005-Deterministic-Random-Foundation.zh-CN.md`

Integration Coordinator-only bookkeeping paths for task creation and closure:

- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`
- `Docs/ProjectState.md`
- `Docs/ProjectState.zh-CN.md`

The Primary Agent must not edit the four bookkeeping paths above.

### Out of scope

- `DreamOfPadma.Build.cs`, `.uproject`, new UE modules, plugins, GAS, Config, maps, `.uasset` files, generated folders, or external prototype content.
- Any source file outside the three exact Core/random paths above.
- Calendar transitions, resource values or ledgers, synthesis candidate/success rules, combat/AI rolls, card systems, save/load, replay, network, UI, input, actors, or presentation.
- Choosing a production stable-ID namespace, a cross-version replay guarantee, a product random distribution, a gameplay probability, or a content default.
- Changing TASK-001 through TASK-004, accepted canonical rules, or any Open/Deferred meaning.
- Merging, tagging, pushing, releasing, or changing shared ProjectState/index from the Primary Agent.

## Dependencies and integration order

- Required predecessors: `TASK-003` is closed in commit `45061f1`; the accepted ACT boundary is integrated in `b2f652b` and does not affect this slice.
- Direct inputs: the Core contract, deterministic-calculation architecture, synthesis rule contract, numerical reproducibility guidance, and module/test boundaries listed under **Read first**.
- Current implementation dependency: use the existing `DreamOfPadma` module and its approved dependencies. No Build.cs or plugin change is expected or allowed.
- Merge order: this bilingual task contract must be reviewed and approved as `Ready` before a Primary Agent starts. The implementation branch is reviewed independently; the Integration Coordinator then updates shared index/state and integrates the result.
- Consumer order: later synthesis, combat, AI, calendar, resource, save, replay, and network tasks may consume this seam only through its reviewed public contract. A public-contract change returns to serial integration review.
- Shared conflicts: public Core contracts, `DreamOfPadma.Build.cs`, module dependencies, `Docs/00_INDEX*`, `Docs/ProjectState*`, maps, binary assets, and the UE build/Editor/PIE lane remain serial under the Integration Coordinator.

## Independent worktree decision

**No Worktree for the initial execution.** This is one bounded feature with one writer under the current policy. Start from the approved `main` state on the Local branch `feature/TASK-005-deterministic-random`. Re-evaluate a Worktree only if independent concurrent work is explicitly introduced and the exact write set remains exclusive.

## Delegation plan

The Primary Agent owns the whole implementation Goal. It may request bounded read-only help, but no second writer is authorized.

| Role | Work package | Authority | Expected output |
|---|---|---|---|
| `reviewer` | Inspect the actual implementation diff, tests, Core boundary, deferred-rule preservation, and bilingual task/module documentation. | Read-only | P0-P3 findings, checks, residual risks, and `Block`, `Conditional pass`, or `Pass`. |
| `architect` | Consult only if the implementation needs a new public schema, module dependency, save shape, or algorithm-version decision. | Read-only | Boundary/ADR recommendation; no patch. |

## Acceptance criteria

- [ ] The exact implementation and test files compile inside the existing `DreamOfPadma` module without a new module or dependency.
- [ ] The public random context/trace boundary is presentation-free, uses no concrete Actor or map state, and introduces no universal Manager or hidden global mutable state.
- [ ] Two fresh contexts with the same master seed, named stream, draw position, and named fixture inputs reproduce the same result sequence and trace within the same approved build/runtime.
- [ ] Named streams are isolated: consuming one stream does not alter another stream's result or position for the same fixture.
- [ ] Debug/test trace evidence includes the seed, stream identity, position, fixture/input identity where supplied, and roll identity/value required to reproduce the fixture.
- [ ] The automation tests exercise the production boundary, are discoverable by the project test command, and pass without loading a map or widget.
- [ ] The Core module README pair and TASK pair remain synchronized and record the implementation evidence without changing canonical gameplay meaning.
- [ ] No Open/Deferred decision is converted into a product default, and no file outside the Primary Agent write set is changed by the implementation.
- [ ] An independent Reviewer returns `Pass`, all required checks pass, and the Integration Coordinator records the result in shared ProjectState.

## Tests

Required automation coverage for this slice:

| Test | Required observation | Constraint |
|---|---|---|
| Same fixture replay | Same master seed, named stream, position, and fixture inputs produce identical values and trace fields in two fresh contexts. | Do not assert a product probability or synthesis result. |
| Named stream isolation | Drawing from stream A does not change stream B's value or position for the same fixture. | Do not use a global random singleton or couple unrelated streams. |
| Position and trace | Each intentional draw advances the recorded position exactly once and emits reproducible roll identity/value evidence. | The trace is evidence, not a gameplay rule. |
| Production boundary | Tests call the public Core interface used by future services. | Do not duplicate the random algorithm inside tests. |

Run from the repository root:

- `Scripts/RunTests.ps1 -EngineRoot "<approved UE root>" -TestFilter "Project.Padma.Core.Random"`
- `Scripts/ValidateProject.ps1 -Strict`
- `Scripts/AuditDocs.ps1`
- `git diff --check`

The UE Editor target must compile before automation results are accepted. Gameplay, synthesis, save, network, PIE, and packaging tests are not required for this bounded slice.

## Verification plan

### Automated checks

- Compare the actual changed paths with the seven Primary Agent paths; confirm the four Coordinator-only bookkeeping paths are untouched by the worker.
- Run the required automation filter and record its report path/output.
- Run strict project validation, the bilingual documentation audit, and both working/staged whitespace checks during integration.

### Manual checks

- Read the public header and test fixture together to confirm the test proves reproducibility rather than an implementation detail.
- Confirm no random state is stored in a DataTable, Data Asset, INI, Actor, widget, or universal Manager.
- Confirm the trace is sufficient for the fixture but does not claim cross-version replay or decide a gameplay formula.

### Required evidence

- Editor compile result and filtered automation-test output.
- Actual changed-path list and staged diff.
- Independent Reviewer report.
- Updated task completion report and Coordinator ProjectState/index evidence.

## Learning contract

| Target | Evidence required | Status |
|---|---|---|
| Deterministic fixture reasoning | The user predicts which seed/stream/position/input fields must match, predicts the trace position after repeated draws, then explains a mismatch from the test output. | `Not started` |
| Core boundary ownership | The user identifies why the random service belongs in Core and why UI, Actor, map, and global Manager state are excluded. | `Not started` |

- Learner's hands-on exercise: before the implementation Agent starts, predict the result and trace invariants for two identical fixtures and one interleaved-stream fixture; after the tests pass, compare the prediction with the automation output.
- Transfer question: when synthesis later needs a new roll or trace field, which parts can this task provide and which gameplay meaning must remain in a separate approved task or decision?

## Risks and recovery

- An implementation may accidentally promise cross-engine-version replay stability. Recovery: constrain acceptance to the approved build/runtime and create a separate versioning decision if persistence needs more.
- A worker may invent a stable-ID namespace, distribution, probability, or save shape. Recovery: stop and return the issue to the named owner; use only opaque task-local fixtures.
- Random state may leak into global or presentation-owned objects. Recovery: reject the diff and keep state in explicit Core contexts/services.
- A new dependency or module split may appear convenient. Recovery: stop and require an architecture/dependency task and ADR before changing it.
- Safe rollback: revert the implementation commit or return the branch to the last approved integration commit; do not erase unrelated worktree changes.

## Completion report

- Final status: `Review`; bilingual contract created and awaiting user approval, implementation not started.
- Primary Agent and Role: pending approval / `module_worker`.
- Changed files: the TASK-005 pair, bilingual index, and bilingual ProjectState routing are recorded in the contract-creation integration change; implementation files remain untouched.
- Acceptance evidence: the bilingual TASK pair contains Goal, exact write set, non-goals, dependencies, tests, learning targets, and Local/no-Worktree execution shape; both index links and the ProjectState route are present; no implementation file changed.
- Checks run and results: `Scripts/AuditDocs.ps1` passed for 110 Markdown files and 55 language pairs; `Scripts/ValidateProject.ps1 -Strict` passed; `git diff --check` and exact creation-scope checks passed.
- Checks not run and reason: compile, automation, gameplay, PIE, cook, and packaging checks are intentionally deferred until an approved Primary Agent implements this slice.
- Review findings resolved or accepted: pending independent implementation Review.
- Remaining risks and open questions: all items under **Deferred or open inputs** remain unresolved by design.
- English/Chinese documentation updated: in progress for task creation; implementation must update both module README members.
- Agent-produced learning evidence: pending.
- User-produced learning evidence: pending the fixture prediction and teach-back.
- Integration commit or handoff reference: pending user approval and later implementation integration.
