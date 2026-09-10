# Production Roadmap

- Chinese companion: [Roadmap.zh-CN.md](Roadmap.zh-CN.md)
- Current priority: Offline Chapter Zero Demo, accepted 2026-09-10
- Authority: [ChapterZero](../Rules/ChapterZero.md), [ADR-0010](../Decisions/ADR-0010-Offline-Demo-Content-and-Map.md)

## Delivery sequence

The existing native rules and separate battle modes are a starting point. Extend verified paths instead of recreating the retired fixed Demo chain. Stages below are work packages, not claims that code/content is already complete.

| Stage | Work to complete | Evidence / dependency |
|---|---|---|
| 1. Freeze chapter content | Bind golden-path steps, nodes, Encounter then one ACT, royal-court ending and rewards; receive final card data | Approved values and rule/action/state coverage; unresolved content blocks only its dependent work |
| 2. Establish package baseline | Produce installable offline Win64 Development and Shipping candidates from existing content | Launch/load/exit in both configurations; expose cook/dependency failures early |
| 3. Map production | Fixed anchors/edges/locks, terrain generation, independent decorative PCG, immutable manifest and bake | Determinism/locks/MapKey/save compatibility plus matching packaged visuals |
| 4. Chapter runtime | Main menu/opening, event/condition/effect flow, stage pool assignment, tutorial progress/skip/replay | Persisted branches/assignments; same-run reward idempotency and deliberate replay reward |
| 5. Playable golden path | Real map/calendar/resources/ABC/synthesis/dialogue actions, Encounter, ACT, court and ending | One normal run actually operates every required system; rollback/restart/continue cases |
| 6. Presentation and final content | Approved terrain/UI/card/character art, animation, audio/VFX; replace prototype values/assets | User visual/input acceptance, asset provenance and final content validation |
| 7. Final dual-build acceptance | Rebuild and install both configurations with final content | [BuildMatrix](BuildMatrix.md) evidence; no claim based solely on PIE |

Content, map and package preparation can progress independently where dependencies permit. Final acceptance requires the same reviewed content in both packages; a successful early prototype package is not the finished Demo.

## Explicitly deferred

- Slate story graph editor, UEdGraph compiler, dialogue/localization input tooling and Cue editor workflow.
- Actual Steam submission, Steam startup validation, cloud saves, achievements and online services.
- FPS gameplay, the full campaign and systems introduced only at later unlocks.
- A universal content-production tool, forced module split or full Asset Manager redesign without a concrete need.

The required story/tutorial runtime is not deferred. Existing typed tables/assets and authoring scripts may supply it.

## Existing work and remaining decisions

Current code/asset/test evidence belongs in [ProjectState](../ProjectState.md). [TASK-011](Tasks/TASK-011-Full-MVP-Task-Cluster.md) and its children remain the longer-term full-MVP allocation, not the current release checklist. Historical TASK-007/008 fixture stages are retired; later native implementations should be reused.

[MVPDecisionRegister](MVPDecisionRegister.md) remains the full-project numerical/semantic backlog. Latest user decisions take precedence. Missing final card/reward data, ending versus alternative victories, replay inventory scope, rendering migration and target hardware/performance criteria remain explicit; do not promote prototype values to final defaults.

This replaces the older roadmap instruction to postpone packaging until after the complete MVP. Documentation approval does not authorize implementation of every stage, Git integration or publishing.
