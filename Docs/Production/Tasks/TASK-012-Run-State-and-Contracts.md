# TASK-012 Run State and Shared Contracts

- Chinese companion: TASK-012-Run-State-and-Contracts.zh-CN.md
- Document ID: TASK-012
- Version: 0.1
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: Game Composition
- Primary Agent: one designated owner at Ready; no writer started
- Branch/worktree: assign a Local feature branch at implementation preflight; no worktree by default

## Goal and why now

Provide one run/session boundary with stable IDs, typed command/results/events, validated definition loading and versioned state-participant export/restore contracts. This extends the reviewed predecessors through one bounded delivery result. It must not turn their temporary fixtures into unapproved game rules.

## Read first

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, [TASK-011](TASK-011-Full-MVP-Task-Cluster.md), [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md), [MVP decisions](../MVPDecisionRegister.md), and the [PadmaGame contract](../../Architecture/Modules/PadmaGame/README.md). Also read `Docs/Architecture/SaveSchema.md`, the predecessor contracts, and nested Source/Content/Docs instructions for the paths below.

Source: [PDF card, rule and screen extraction](../MVPSourceExtraction.md). Old contract-card sandbox summon effects are excluded from the first version; missing ACT definitions are not filled by converting turn-based cards.

## Decisions and readiness

- Frozen: the accepted MVP/rules, separate Encounter/ACT GAS systems and tables, separate turn-based/ACT character-card collections, and shared basic-skill identity/slot mapping with mode-owned effects.
- Frozen: placeholder assets will be imported by the user; final animation/rendering polish is later work. Every unaccepted numerical value requires individual user confirmation before use, including test profiles.
- Gate references: D04 See the register for the owner and exact missing information.
- Backlog describes an approved planning item, not a runnable implementation task. Record gate answers in the canonical rule/architecture source and this report, complete predecessors, then promote to Ready. Technical schemas are reviewed by the Architect; gameplay and numerical choices belong to the user.
- If gate closure requires a normative pair or structural ADR outside this write set, the Coordinator must add those exact paths before Ready; this is planned preflight, not permission for arbitrary edits.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/Core/Contracts/`
- `Source/DreamOfPadma/Private/MVP/Core/Contracts/`
- `Source/DreamOfPadma/Public/MVP/Game/Session/`
- `Source/DreamOfPadma/Private/MVP/Game/Session/`
- `Source/DreamOfPadma/Public/MVP/Game/DefinitionLoading/`
- `Source/DreamOfPadma/Private/MVP/Game/DefinitionLoading/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-012/`
- `Content/Padma/MVP/Definitions/Bootstrap/`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-012-Run-State-and-Contracts.md`
- `Docs/Production/Tasks/TASK-012-Run-State-and-Contracts.zh-CN.md`

Exclusions: Gameplay formulas, automatic extraction of all Demo code, disk saving and concrete mode abilities. Other task source/content, shared configuration not named above, the reference prototype, generated folders and new standalone UE modules are outside scope. Existing Demo code is consumed through public contracts; adaptation/migration needs an explicitly named write set.

## Dependencies, handoff and conflicts

- Required predecessors: [TASK-010](TASK-010-Playable-Prototype-Integration.md), [TASK-011](TASK-011-Full-MVP-Task-Cluster.md), [TASK-040](TASK-040-Model-Assets-and-Scene-Bindings.md).
- Conditional predecessor: TASK-005 before any random outcome where it is not already listed.
- Hand off the reviewed public commands, read-only views, definition fixtures and test evidence. A state owner implements TASK-012 snapshot participation; UI never owns mutable gameplay state.
- Module README pairs, shared headers, maps, binary assets and central files are serial leases. Do not edit while another owner/build consumes them. One UE build/Editor/PIE lane.
- No writable subagent is authorized. One bounded read-only architect/reviewer may inspect the relevant contract/diff. The Primary Agent remains accountable; the Coordinator owns shared integration state and Changelog.

## Acceptance criteria

- [ ] Freeze the mode-neutral card-instance/slot/query/legal-play/payment/lifecycle provider ports as Core value contracts. TASK-017/030 consume these ports, TASK-020 implements production providers and TASK-034 injects them; no port depends on a concrete Gameplay ability, Actor or final card-definition row.
- [ ] Preserve run identity across a test scene lifecycle; re-entry does not create a second authoritative state owner.
- [ ] Reject missing IDs, duplicate definitions and unresolved mode references before partial initialization; use explicit adapters when consuming TASK-007/008/009 fixtures.
- [ ] Distinguish definition ID, owned card-instance ID and battle-entity ID; keep turn-based and ACT character identity domains separate. Shared identity/slots apply only to basic non-A skill cards, not character cards, live abilities or cooldown state. Separate all logical IDs from Actor/ASC/ability/effect handles and do not force one mode definition schema.
- [ ] Define participant registration, versioned export/restore and missing-participant failure so future battle-mutated state cannot silently escape rollback.
- [ ] Exact write set, synchronized task/module documentation, required checks and independent review pass; no unresolved rule has been filled by a default.

## Verification and evidence

Session recreate/re-entry, invalid-definition and participant snapshot round-trip automation; view the surviving run/context IDs in PIE.

Compile the Editor target when native code changes. For the implemented narrow automation, use Scripts/RunTests.ps1 with the verified EngineRoot and TestFilter `DreamOfPadma.MVP.TASK012`; confirm reported cases actually passed, not only process exit. Run Scripts/AuditDocs.ps1, applicable Scripts/ValidateProject.ps1 -Strict and scoped git diff --check. Do not add broad UI automation merely for this task. Record reproducible user PIE observations (written evidence is acceptable), input/profile IDs and logs; images/video may supplement them. No runtime/manual success is claimed by this contract.

## Learning contract

| Target | Evidence | Status |
|---|---|---|
| State ownership and identity | Predict which values survive changing a map, then compare the IDs and explain the state owner. | Not started |

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

## Neutral bonus-source and modifier contract

Before downstream calculations start, freeze presentation-independent value types and a read-only query port for versioned effect sources/contributions: stable source/bonus identity, target domain/definition identity, scope/clock/context, eligibility and contribution identity. No GAS handles, Actor pointers, arbitrary executable payload or guessed magnitude belongs in this neutral contract. D28 selects the actual supported target and stacking/timing/value semantics; D04 records the technical shape.

TASK-039 produces relationship source/eligibility data. The selected target's existing authority consumes it: 019 resource operations, 020 authored card/lifecycle projections, 022 synthesis calculation, or 015 combat calculation. Register one owning consumer for each selected scalar/effect so the same contribution cannot be applied by multiple layers. These consumers use explicit injected provider fixtures first; 034 later injects 039's real provider. No consumer depends on the later relationship implementation. Unknown domains, duplicate contribution identity and incompatible versions fail explicitly. Add this port and producer/consumer matrix to the contract acceptance and validation evidence; an interface named only by its future consumer is insufficient.

## 2026-09-09 confirmed model/movement handoff

Reuse TASK-040's presentation-independent FPadmaCardMobilityDefinition in card contracts; do not create another three-state enum. Future instance/node/group commands remain this task's state contract, not preview keys. D29 requires explicit selected card instances and an atomic A-plus-attached-B movement result.
