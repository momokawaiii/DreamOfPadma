# TASK-037 Node Inspection and Local Scenes

- Chinese companion: TASK-037-Node-Inspection-and-Local-Scenes.zh-CN.md
- Document ID: TASK-037
- Version: 0.1
- Status: Backlog
- Parent: [TASK-011](TASK-011-Full-MVP-Task-Cluster.md)
- Primary Role: World presentation / Game composition
- Primary Agent: one designated writer at Ready; none started
- Branch/worktree: Local branch assigned at preflight; worktree only for actual isolation

## Goal

Present hover information and modeled local-node scenes from the same observer-filtered world view, with validated entry/return and selectable character/building details. This adds inspection presentation without deciding movement or battle rules.

## Inputs and readiness

Read AGENTS.md, Docs/00_INDEX.md, Docs/ProjectState.md, the predecessor contracts, [screen design](../../Design/EN/06_MapAndPreparation.md), [MVP decisions](../MVPDecisionRegister.md), ADR-0005, relevant module READMEs and nested instructions. English is canonical; the Chinese companion mirrors it.

- Required predecessors: [TASK-012](TASK-012-Run-State-and-Contracts.md), [TASK-014](TASK-014-Character-Presentation.md), [TASK-024](TASK-024-Sandbox-Graph-and-Movement.md), [TASK-025](TASK-025-Node-Operations-and-Story.md), [TASK-040](TASK-040-Model-Assets-and-Scene-Bindings.md).
- Decision gates: D02, D12, D21, D24, D25. Pending answers are not defaults.
- The user requests this capability; the current contract only plans it. Complete dependencies, record exact approved behavior/content/values, review public schemas and promote to Ready before coding. All unaccepted numbers require individual confirmation.
- Core remains presentation-independent; stable identities and typed command/result/view boundaries apply. No runtime success is claimed.

## Scope and allowed paths

- `Source/DreamOfPadma/Public/MVP/UI/NodeInspection/`
- `Source/DreamOfPadma/Private/MVP/UI/NodeInspection/`
- `Source/DreamOfPadma/Public/MVP/Game/NodeVisit/`
- `Source/DreamOfPadma/Private/MVP/Game/NodeVisit/`
- `Source/DreamOfPadma/Public/MVP/World/NodePresentation/`
- `Source/DreamOfPadma/Private/MVP/World/NodePresentation/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-037/`
- `Content/Padma/MVP/World/LocalNodes/`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.md`
- `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.zh-CN.md`

The Coordinator separately owns shared index/state/task-matrix updates. Add any required normative/ADR/public-contract paths explicitly before Ready; do not infer a broad lease. Other features' code, configuration/build files, .agents/.codex, generated folders and third-party content are excluded.

## Out of scope

New movement prices/visibility rules, battle results/transactions, full NPC relationships, global preparation internals, ACT/FPS combat, World Partition/PCG or per-node mandatory unique maps.

## Acceptance criteria

- [ ] Hover and keyboard/touch inspection show approved terrain, node type, controller, deployments, A/B attachment, facilities/status and route conditions; unavailable/unknown information is explicit.
- [ ] The query provider applies D25's full first-MVP disclosure to friendly/enemy names, counts, positions, cards and model construction. Empty remains distinguishable from missing data; no hidden-intelligence mechanic is introduced.
- [ ] World and local representations bind the same NodeId, card-instance IDs, EntityId values and state version. Entering/rebuilding cannot duplicate entities or reset damage/facilities.
- [ ] Scene definitions use typed soft references and explicit placements; validate missing maps/models and stale or mismatched load completions without partial state.
- [ ] Click only inspects all node information. A separate right-side Move Here button submits selected card-instance IDs to the movement service, using D29's group checks. Inspection itself issues no payment/battle command; returning from inspection is distinct from battle exit/rollback.
- [ ] Characters/buildings can be selected for read-only card/state/skill/operation views. TASK-014 supplies compatible visual components; battle animation/execution remains TASK-017/030.
- [ ] Return restores map selection/camera context. Loading failures preserve the last valid world state; legal changes made through node-operation services remain reflected.
- [ ] Expose a navigation host/intent for TASK-038; TASK-034 binds the real global preparation page without a reverse implementation dependency.
- [ ] Bilingual module/task reports, scoped validation and independent read-only Review pass.

## Verification

Observer filtering including model construction; unknown versus empty; same-ID repeated entry; stale/wrong-node load; missing assets; context restore; approved movement/payment separation. Observe hover → scene → entity → return in PIE.

Compile the Editor target for native changes; run the narrow TASK037 automation through Scripts/RunTests.ps1 and inspect actual case results. Run Scripts/AuditDocs.ps1, applicable ValidateProject.ps1 -Strict and git diff --check. Capture reproducible manual observations; no broad UI automation suite is required. TASK-034 performs real feature composition. Failed integration findings return to the feature owner.

## Ownership, learning and recovery

One Primary writer and one serial UE build/Editor lane; maps, assets, schemas and module README pairs are exclusive leases. A bounded architect/reviewer may inspect read-only; no second writer. Commands/events and state ownership must survive scene reconstruction and save boundaries without Actor pointers.

Learning target: Trace one NodeId and EntityId across map, local Actor and saved state; explain why rebuilding visuals cannot recreate ownership. User evidence: Not started. Provide one prediction and hands-on observation at implementation handoff; Agent output does not prove user learning.

Recover by reverting only this task's reviewed change after checking ownership. Do not delete generated/unrelated data or merge/push without explicit authorization.

## Completion report

- Status: Backlog; this is a planning amendment, implementation not started.
- Code/assets, runtime tests, user observations and independent implementation Review: pending.
- English/Chinese pair created together. Implementation handoff supplies the Changelog draft, exact changed paths, evidence and remaining risks.

## 2026-09-09 confirmed model/movement handoff

D24/D25 are accepted: click shows all node information; place Move Here on the right and submit selected card-instance IDs through the movement service. Show complete enemy content. D29 governs A/attached-B groups; this UI cannot move Actors directly. Consume TASK-040 presentation through real world views and TASK-020 production definitions, composed by 034; replace preview bindings with authoritative projections. Inspection entry/return never creates a battle snapshot.
