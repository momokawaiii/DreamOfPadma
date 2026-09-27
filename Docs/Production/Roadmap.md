# Production Roadmap

- Chinese companion: [Roadmap.zh-CN.md](Roadmap.zh-CN.md)
- Current priority: Offline Chapter Zero Demo, refined by Q46–Q90 on 2026-09-10
- Authority: [ChapterZero](../Rules/ChapterZero.md), [ADR-0010](../Decisions/ADR-0010-Offline-Demo-Content-and-Map.md)

## Delivery sequence

Reuse the integrated native rules/transaction/authoring foundation. [DemoDeliveryPlan](DemoDeliveryPlan.md) owns the detailed work, learning and asset checklist: one learnable unit, concrete feature and evidence. Do not restart the user's completed Blueprint/data/UI/GAS basics.

| Stage | Result | Dependency / acceptance |
|---|---|---|
| 1. Small contract foundation | Stable IDs, typed data ownership, UI intents/results and ACT migration matrix | Missing final card values gate dependent settlement/content only; user supplies complete UI specification. |
| 2. First ACT learning unit | One new model, retargeted animations, keyboard/mouse third-person movement/camera | Real operable 3C evidence; no full skill framework in this first task. |
| 3. Complete ACT P0 | Native GAS/Definition/SequenceRuntime, combos/hits/interrupts and one projectile/summon/execution; repeatable enemy scene | Independent behavior/dependency checks plus user feel iteration; see ACT contract/matrix. |
| 4. Map and chapter | Fixed-Seed baked 3D terrain/cosmetic PCG; menu/opening, events/tutorial/skip/replay and final card data | Seven anchors, explicit routes/locks, MapKey consistency and saved assignments/reward identity. |
| 5. Production UI and integration | User-designed page hierarchy, CommonUI focus/Back, materials/motion; actual ABC/synthesis/dialogue → Encounter → ACT → court → ending | Stable domain semantics, failure/cancel/rollback/continue, real player operations. |
| 6. Release candidate | Approved Chinese/art/animation/VFX/audio, cleared asset graph and both offline packages | [BuildMatrix](BuildMatrix.md); user presentation/playtest acceptance is distinct from automation. |

Start package/cook smoke checks and development-machine measurements at stage 1; repeat when relevant changes require them. Do not postpone every integration failure until stage 6. Map, content and ACT may be interleaved when dependencies are ready; each learning unit stays bounded. Both final packages use the same approved content.

## Deferred and gated work

- Custom Slate/UEdGraph story editor/compiler, dialogue/localization/Cue authoring tools and a dedicated asset-replacement tool.
- Multiplayer replication/prediction/RPC/session/server work; hide or clearly disable its entry.
- Steam integration/submission, cloud saves, achievements, FPS, the full campaign and bulk Combat migration.
- Hand-controller polish, full English, voice acting and broad performance optimization. Measure a generous baseline now and fix severe failures; no invented 1080p/60 FPS requirement.
- Missing/placeholder audio is allowed during functional development; **production audio is required for RC**.
- Manually replace references for the user's public/Steam asset boundary; renamed/transitive civili or prohibited dependencies fail that target's audit. No custom replacement editor is a prerequisite.

Story/tutorial runtime, complete representative ACT and final production card values are not deferred. [MVPDecisionRegister](MVPDecisionRegister.md) retains unrelated full-MVP decisions; old TASK-011 sequencing does not override this delivery.

## Needed inputs

One new character asset choice with actual skeleton/provenance details; final card/reward definitions; and the user's full UI interaction specification. Independent work can proceed while these arrive. Exact replay inventory/failure resume content and concrete budgets remain to be recorded. This document itself authorizes no implementation batch, Git integration or publishing.
