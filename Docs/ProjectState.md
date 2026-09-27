# Project State

- Updated: 2026-09-22. This is a working-tree summary; verify Git and affected files before editing.
- Retained scope: the turn-based L_PadmaWorld/L_PadmaBattle pair, Chen L_ChenACT, and the PadmaNPR plugin. Standalone previews and obsolete records were retired; see [cleanup and recovery](Production/ProjectCleanup.md). This is not new shipping acceptance.
- Current product: installable offline Win64 Chapter Zero Demo. Scope: [ChapterZero](Rules/ChapterZero.md); order and gaps: [DemoDeliveryPlan](Production/DemoDeliveryPlan.md).

## Usable entry points

- NPR update (2026-09-27): placed Chen uses `DA_Chen_NPR_Reference` across nine slots, now calibrated for dry skin/cloth, planar/feathered face SDF and localized .88 bang opacity. Eleven automation checks and a subsequent actual-idle head-basis regression passed; final artistic and high-speed gameplay acceptance remain open in [TASK-057](Production/Tasks/TASK-057-NPR-Visual-Reconstruction.md). [Reference workflow](../Plugins/PadmaNPR/ReferenceCharacter.md).

- Strategy prototype: `/Game/Padma/MVP/Playable/Maps/L_PadmaWorld`. Current painted tutorial has 55 cells, confirmed A/B placement, map navigation and native CommonUI/Slate pages. [Playable guide](Content/NativePlayableDemo.md).
- Chen ACT/render lab: `/Game/Sandbox/ACT/Training/Maps/L_ChenACT`. Chen and three targets are now placed Blueprint instances, visible before PIE; native combat borrows them and F8 reuses them. Daily authoring no longer reads Unity/Artifacts caches. The same map hosts training and manual toon-material study, with the Toon environment; see [lab guide](Content/ChenACTRenderLab.md) and [ownership decision](Decisions/ADR-0012-Scene-Owned-ACT-Participants.md). Animation/GAS actions, FX, camera, weapon lifecycle and HUD are in [TASK-056](Production/Tasks/TASK-056-Chen-ACT-Actions.md). The Sandbox layout does not establish shipping acceptance.
- Turn-based UI: existing native menu/divination entry; current result and visual limits in [TASK-054](Production/Tasks/TASK-054-Divination-Motion-Preview.md). Attack01 Montage/FX evidence is retained only as part of the Chen ACT action path; the old standalone Attack01 map/assets are retired.
- ACT canonical assets: 1,427 audited packages consolidated to 1,422 active packages with 5 obsolete fixtures archived; historical Attack01/Centimeter directories retired. See [layout](Content/ACTAssetLayout.md). All asset/reference checks and seven targeted D3D12 regressions passed; this does not implement the future global Buff/projectile/summon tables.

## Active work and major gaps

- PadmaNPR character lab: native Studio Profile Apply/Restore, per-character component, Face SDF, artistic skin/hair/eye responses and masked bangs/helper shadows are implemented. [Usage and limits](../Plugins/PadmaNPR/Character.md). Advanced renderer/Atlas/outline and final visual acceptance remain open.

- Chen: latest weapon/input/plunge changes have build/authoring evidence; physical playtest remains. The four-page training HUD has build and one focused automation pass; final layout, page interaction and flicker acceptance remain. Full original FX/camera parity is not established.
- Chapter Zero still needs the final fixed-seed baked 3D map, chapter/story/tutorial/reward persistence and complete ordered golden path. Approved final card/reward values are still required. Existing temporary values do not satisfy final acceptance.
- Development and Shipping package/clean-install acceptance remain outstanding. Final public assets must pass the agreed source/reference audit. Story editor and networking are deferred; final audio requirements remain in ChapterZero.
- Art/interaction acceptance of the strategy and divination candidates remains separate from earlier automated/build evidence. Prior results live in the owning TASKs, not this startup page.

## Working boundaries

Current Local checkout; preserve unrelated changes and one UE build/Editor/PIE lane. User decisions and Rules/Architecture own game meaning. Follow [Workflow](Agent/Workflow.md): small fixes need no TASK, long tasks keep only current decisions/evidence/next action, and Chinese companions are short summaries. No implicit Git integration authorization.

Prior detailed entry-page facts are retained in [history](Production/ProjectCleanup.md); read only for a specific historical question.
