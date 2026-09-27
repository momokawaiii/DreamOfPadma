# TASK-057: Chen material visual reconstruction

Status: In Progress

Manual interface follow-up (2026-09-27): independent explicit-key-light opaque backend, semantic slot/texture recipe, actual skeletal Slate viewport and Generate NPR Character are implemented. Existing reference workflow remains accessible. Compilation/generation, invalid-mask/collision rejection, fresh PIE seven-MID binding and Slate lifecycle/capture passed in Artifacts/PadmaNPRSDF/manual-*. This is a basic authoring slice, not reference parity, automatic matching, outline or package acceptance. See Plugins/PadmaNPR/ManualAuthoring.md and ADR-0017.

## Outcome

Reconstruct every material used by the existing nine-slot Padma Chen mesh using ZMDRender Chen as the primary visual reference and Toon as a secondary algorithm reference. Deliver readable plugin shader implementations, actual saved character bindings, controlled visual comparisons and a learning guide identifying the equations and concepts actually used.

## Constraints

- No engine edits or UE 5.8 Toon BSDF/profile features. Preserve gameplay, animation and source projects.
- Do not infer fidelity from compilation, generic GPU math tests or the presence of similarly named effects.
- Compare source graph and replacement under the same geometry/camera/light/exposure where possible. Record geometry/section limitations explicitly.
- Preserve source MI/default profile evidence. Isolated reference copies belong in ignored Artifacts; shipping plugin defaults must not depend on that directory.

## Acceptance

- All nine slots accounted for: body, cloth01, cloth02, brow, eye shadow, face, hair, hair shadow, iris.
- Live graph paths, inherited parameters, masks/encoding, blend modes and auxiliary drivers verified.
- Per-slot before/reference/reconstruction captures, including moving-light and view-dependent responses.
- Necessary DA/component contracts validated, lifecycle regressions and independent read-only review completed.
- English technical contract and concise Chinese learning order delivered. Final visual evidence and remaining limitations reported honestly.

## Current delivery (2026-09-27)

- All nine reference masters and five large material functions are saved. Live source outputs were lowered to HLSL with source-node comments; small supporting shaders retain native scene sampling and vertex dependencies. Old generic masters remain intact.
- `DA_Chen_NPR_Reference`, copied textures/weighted SDF curve, generated MICs and placed Chen bindings in `L_ChenACT` are saved. Fresh editor processes reload the nine bindings. Dependency traversal found no `/Game/ZMD` or `/Game/Comparison` references in the saved profile chain.
- Reference scalar/vector maps are authoritative; source MI owns textures. CPD 7/11 carry the signed SDF curve value/validity. New reference-only surface types reject generic mode. Independent C++ review passed after the legacy Cloth mismatch fix.
- Actual-mesh Matcap isolation identified the sharp nose patch. Only the Padma Face DA overrides Matcap_Color to zero; reference formulas remain unchanged. The underlying vertex attribute difference is not established.
- No source project, engine code, mesh sections, gameplay or environment lighting was changed. ZMD's 13-slot geometry is not equivalent to Padma's nine slots.

## Verification

- Editor build succeeded: `Artifacts/PadmaNPRVisual/build-final.log`; final test-only build: `build-tests.log`.
- All **10 PadmaNPR automation tests passed**, including reference contracts, scalar removal/inheritance, curve endpoints/validity and runtime scalar MID propagation: `Artifacts/PadmaNPRVisual/tests.log`, `Tests/index.json`.
- Independent-process source/replacement captures: `comparison_front.png`, `comparison_body.png`, `comparison_side.png` in the same artifact folder. Whole-image 8-bit RGB mean absolute errors are approximately front `[.205,.192,.190]`, body `[.015,.012,.011]`, side `[.008,.006,.007]`. These are not foreground-only metrics and do not establish every pixel or animation is identical. Cloth retains differences around detailed highlights/edges.
- Fresh actual-project captures: `padma_front.png`, `padma_body.png`, `padma_side.png`; all final capture processes exited 0. Face calibration persists. Side capture changes camera and key-light direction; it is a separate lighting case, not continuous-motion validation.
- No material compilation failures/default-material fallback in final captures. Startup logs contain engine smoke-test `Condition failed` messages adjacent to UnifiedError tests; these are separate from the explicit ten successful PadmaNPR tests.
- Learning/file/usage contract: `Plugins/PadmaNPR/ReferenceCharacter.md` and Chinese guide. Advanced planned features are explicitly not represented as implemented.
- Documentation audit and strict project validation passed.

## Remaining acceptance

### Calibration follow-up

- Added opt-in head-plane SDF angles, stable vertical-light fallback, independent threshold feather/bias, top-light stabilization and field/shadow debug views. Chen enables planar mode and FaceControls `(.035,0,1,0)`, with a warmer/lighter shadow multiplier `(.67,.62,.61,1)`.
- Corrected the earlier weather claim: source Skin/Cloth `RainStrengh=1` was active. The saved Chen DA now sets it to zero for the requested dry baseline. Source shader arithmetic is retained for traceability, not presented as an implemented weather service.
- The first Python view sweep used positional Rotator arguments incorrectly and is superseded. Corrected captures explicitly set pitch/yaw/roll; final per-view JSON records must be checked, not inferred from image filenames. Failed attempts to inspect private CPD through Python were removed; native tests cover CPD.
- The eleven-test suite passed after adding actual saved nine-slot configuration checks and GPU feather/hard-step/top-light boundary assertions. Independent review identified tiny positive feather rounding; values below 1e-5 now take the hard-step path. Evidence: `Artifacts/PadmaNPRSDF/tests.log`, `build-final.log`.
- Hair anisotropy, cloth02/HairLine, eye, brow and eye-shadow source bindings remain independent. No unsupported POM, physical skin scattering or new projected hair-shadow pass is claimed. Matcap face calibration remains explicit.
- Near 90-degree side light still follows the central contour authored into the source SDF. Feather does not manufacture a different nasal shadow shape. This is a documented artistic asset constraint.
- Final explicit-rotation sweeps cover front, both oblique sides, both 90-degree sides, overhead and back light; see `Artifacts/PadmaNPRSDF/padma-*.json` and matching PNGs. `verified-*.log` supersedes the first mislabeled sweep. Short turn/light samples are in `motion_sheet.png`; they exercise transient .88 bang opacity. Python animation time advancement alone is not proof of evaluated animated geometry.
- The native `PadmaNPR.Reference.Binding` regression now explicitly evaluates actual idle poses, verifies a changed head transform and checks the resulting CPD basis. It passed in `pose-test.log` after the complete eleven-test run. This distinguishes pose evaluation from the Python light/actor sweep.
- The saved Chen DA enables localized bang opacity .88; master default 1 remains available. Current map lighting is unchanged by the transient capture sweeps. Final save/reload evidence: `saved-final.log`, `saved-reload.log`.

The nine-slot dry character workflow is delivered and calibrated. Task remains **In Progress** for final artistic acceptance of the actual Padma result, especially scene reflections, the source SDF contour and missing independent source sections. Long/high-speed gameplay temporal stability, packaged behavior and performance have not been validated. No whole-character pixel identity or finished custom renderer/atlas is claimed.
