# Development Learning Workflow

- Chinese companion for user reading: `Docs/Learning/Workflow.zh-CN.md`

## Purpose

Dream of Padma is both a game project and a full-stack game-development curriculum. Learning is attached to accepted project deliverables, but feature completion and learner mastery are measured separately.

The project assumes the learner may begin with no background. Agents explain prerequisites in plain language, expose real evidence, and preserve opportunities for the user to operate the tools personally.

## One task, two outcomes

Every implementation task produces:

1. **Project outcome:** a tested, reviewable artifact that satisfies the task contract.
2. **Learning outcome:** evidence that the user can recognize, operate, explain, or transfer one or two concepts.

An Agent can complete the first outcome. It cannot claim the second merely because its code or asset works.

## Learning loop

```text
Orient -> Predict -> Observe -> Practice -> Verify -> Teach back -> Transfer
```

### 1. Orient

Explain the current player-facing need, the subsystem that owns it, the inputs and outputs, and why this task comes before later topics. Introduce only the vocabulary required for the task.

### 2. Predict

Before running a test, changing data, opening a debugger, or pressing Play, ask the user what they expect. A written prediction creates evidence of the user's mental model.

### 3. Observe

Show the smallest useful artifact: one header contract, data row, Blueprint graph boundary, animation state, Gameplay Debugger view, log category, RenderDoc/Insights trace, material node group, or test output. Avoid turning orientation into a tour of the whole engine.

### 4. Practice

The user performs one safe, reversible operation. Examples include changing a controlled data value, setting a breakpoint, running a named automation test, inspecting a Gameplay Tag, profiling one captured frame, fixing one test fixture, or reproducing one documented Editor action.

### 5. Verify

Compare the observed result with the prediction. Explain the causal chain and one likely failure mode. Restore the controlled value if the exercise is not intended to become project content.

### 6. Teach back

The user explains in their own words:

- what the concept does;
- where it belongs in Padma;
- what it must not depend on;
- how they would recognize one failure.

### 7. Transfer

Ask one nearby application question, such as how seeded random behavior affects saves, how an input mode affects UI and Gameplay, or how a Niagara budget changes content authoring. Transfer prevents memorizing a single set of clicks.

## Evidence and status

| Status | Meaning | Minimum evidence |
|---|---|---|
| `Not started` | No structured exposure | None |
| `Introduced` | Purpose and vocabulary were explained | Orientation note and question log |
| `Practiced` | User performed the planned operation and inspected its result | User action plus observable output |
| `Demonstrated` | User can explain and apply the concept beyond the exact example | Successful teach-back plus transfer answer and evidence |

Record separately:

- Agent-produced artifact/test evidence;
- user-produced operation or observation;
- user prediction and teach-back;
- misconceptions and unresolved questions;
- next prerequisite or transfer exercise.

Screenshots and logs may support evidence, but a copied command output without interpretation is not mastery.

## Responsibility split

| Participant | Responsibility |
|---|---|
| Primary Agent | Delivers the feature safely and preserves a learnable seam |
| `learning_tutor` | Scopes concepts, teaches prerequisites, defines exercise/evidence, and checks teach-back |
| Reviewer | Confirms learning work did not weaken architecture, tests, or task scope |
| User | Makes design decisions, performs the hands-on step, asks questions, and demonstrates understanding |
| Integration Coordinator | Ensures evidence is recorded and sequences prerequisites across tasks |

## Selecting learning targets

Choose at most two primary targets per task. Prefer one system concept and one tool or evidence skill. Examples:

| Development output | Suitable focused targets |
|---|---|
| Core calendar value and test | `USTRUCT`/value semantics; automation-test boundary fixtures |
| Data-driven card definition | Data Asset versus runtime instance; Gameplay Tags and validation |
| Encounter action timeline | state machine/event ordering; deterministic scenario test |
| ACT card-selection mode | Enhanced Input context/gating; time dilation and UI/gameplay separation |
| Battle rollback | snapshot identity/versioning; save round-trip verification |
| Niagara ability effect | emitter/system ownership; effect budget inspection |
| World Partition content slice | streaming cell ownership; reproducible Editor validation |
| Performance task | Unreal Insights capture; hypothesis-to-measurement reasoning |

Do not attach animation, physics, rendering, UI, networking, AI, audio, packaging, and tooling to one task merely because they all appear in the full curriculum. The curriculum is covered across milestones through real vertical slices.

## Novice-friendly task sequence

For each new module or discipline:

1. vocabulary and ownership map;
2. smallest working artifact;
3. observable test or debugger evidence;
4. one controlled failure and diagnosis;
5. user practice and teach-back;
6. integration with one neighboring subsystem;
7. later performance, production, and edge-case depth.

This order prevents advanced engine features from becoming isolated tutorials with no place in the game's architecture.

## When learning blocks completion

The project outcome may be `Verified` even if the user learning target is only `Introduced` or `Practiced`, provided the task contract did not require `Demonstrated`. Do not falsely close learning evidence. Carry the next exercise into a later task.

Conversely, do not keep expanding an implementation Goal just to finish a broad topic. Create a later learning-focused or integration task when the missing exercise requires a materially different artifact.
