# Manual character authoring

Implemented 2026-09-27. [Chinese usage](ManualAuthoring.zh-CN.md). This is a basic, opaque, explicit-key-light backend, distinct from the calibrated Chen reference path. It does not claim the reference path's full visual fidelity or shadow-map visibility.

## Usage

Open Window > Padma NPR Studio. Select a skeletal mesh in the Content Browser and press Read Selected Mesh / Recipe, or set Mesh in Details. Existing slot names are read without changing the source mesh. Choose a surface for each intended slot; KeepOriginal is the safe default. Transparent helper sections should remain KeepOriginal. Types available are Cloth, Skin, Face, Hair, Eye and Brow. Hair adds a tangent-oriented strand lobe; optional Face SDF adds mirrored-field planar light evaluation; other types share the baseline Cel response rather than pretending to implement special eye optics or physical skin scattering.

Assign BaseColor and standard UE tangent-space Normal textures. Scalar masks accept independent textures and R/G/B/A selections; assign the same texture multiple times for packed channels. Glossiness explicitly inverts the selected roughness channel. Constant roughness/metallic and unoccluded AO support missing scalar maps. BaseColor may be omitted for an intentional constant-color surface. UV channel/scale/offset is common to the slot. Standard Normal compression is required; custom reference RG normals and virtual textures are rejected, not silently reinterpreted. Shared source textures are never edited. Scalar maps must be linear and not Normal-compressed. Missing Matcap and FaceSDF disable those optional layers.

Choose a valid head bone and axes for SDF. ReadSlots defaults to the root only as a usable baseline for surfaces without SDF; it does not infer the correct head orientation. SDF uses the selected channel, U mirroring and 1-(dot(projectedLight,headForward)*.5+.5) threshold convention. Other authored SDF conventions need conversion/calibration. This is separate from the Chen reference curve path.

Press Build NPR Preview after input/style edits. This compiles a transient material/profile set and renders the actual mesh in an SEditorViewport, with fixed exposure. Light yaw/pitch and play/pause update while the viewport ticks. Select an animation from the same skeleton, then use the normal editor viewport mouse controls for camera movement. Light changes affect the explicit shader light; they are not an environment-lighting preset system. No weather, dedicated studio floor or reference-DA preview is implied.

Press **生成 NPR 角色** with a new /Game output folder. It validates, compiles and saves:

- M_NPR_Manual_<index>: generated resource adapter/master for each converted slot; shared math lives in PadmaManualCore.ush. Per-slot adapter specialization allows explicit channels/UV/import encodings without silently assuming ORM.
- MI_NPR_Source_<index>: source material instances inheriting those adapters.
- DA_NPR_Character: runtime Profile with artistic parameter maps.
- DA_NPR_Recipe: editor-only regeneration inputs, including resource choices and preview settings.
- T_NPR_LinearWhite: an owned neutral numeric texture.

No original material, mesh section, topology or texture is overwritten. Existing output collisions fail before mutation: change OutputFolder for the next revision. Owned in-place regeneration is not implemented. A compile failure releases new unsaved objects; a disk save failure reports the failed object and can leave already-saved partial outputs. Generation is synchronous and can pause the panel during shader compilation.

After generation, select a level actor and press Apply to Selected Character. Exactly one skeletal mesh component must match the recipe's mesh. Existing ApplyToActor creates persistent applied MICs and binds the NPR component. Save those MICs and the level using normal editor Save All. Set Key Light on the component if needed; otherwise the generated fallback light direction is used. PIE creates isolated MIDs with the same DA artistic overrides. Changes to generation inputs invalidate Apply until regeneration. To apply a saved Profile after reopening, use **已有 DA / 参考版** to open the existing Profile inspector/Apply/Restore workflow. Loading DA_NPR_Recipe restores inputs for editing and preview; it does not silently select a generated Profile for a possibly changed recipe.

## Authority and file roles

Recipe owns explicit resource/encoding choices for regeneration. The saved source MI/master chain contains their generated runtime representation. DA ReferenceVectors owns ManualTint, ManualShade, ManualStyle=(threshold,feather,rim,highlight) and ManualSurface=(surface kind,Matcap strength,0,0). ManualResponse requires ReferenceResponse and the M_NPR_Manual_ prefix; it is not the Chen reference formula. Runtime Profile has no reference to the editor-only recipe.

| File | Responsibility |
|---|---|
| Source/PadmaNPREditor/Public/PadmaNPRCharacterRecipe.h | Manual semantic inputs and editor-only recipe |
| Source/PadmaNPREditor/Private/PadmaNPRCharacterRecipe.cpp | Preflight, adapter graph creation, compilation checks and asset saving |
| Source/PadmaNPREditor/Private/SPadmaNPRCharacterStudio.cpp | Slate form, preview scene/mesh/camera, preview animation, Generate and Apply actions |
| Shaders/Private/PadmaManualCore.ush | Named normalization, band and strand-highlight helpers plus composition |
| Source/PadmaNPRRuntime/Private/PadmaNPRProfile.cpp | Backend validation; existing parameter maps remain the common editor/runtime contract |
| Source/PadmaNPRRuntime/Private/PadmaNPRComponent.cpp | Existing runtime MID and head/light CPD path; no editor dependency |

ManualCore is an Unlit artistic evaluator with explicit light direction, roughness-dependent specular response, metallic tint, AO, Rim and optional Matcap. It does not receive actual UE light occlusion, multiple lights or environment BRDF lighting. Meshes can still participate in scene shadow casting according to their component/material settings. Keep the existing reference workflow when those authored responses are needed. Outline, automatic matching, weather, custom passes and second-character validation remain later work.

## Evidence

Editor build: Artifacts/PadmaNPRSDF/manual-build.log. Real generation/compilation and invalid-mask/collision checks: manual-result.json. Actual Slate render plus close/reopen: manual-ui-test.log and manual-studio.png. Reloaded generated Profile applied transiently to the existing scene, followed by actual PIE seven-MID checks: manual-pie-result.json; no source level save is part of that test. These checks do not establish packaging, every animation, arbitrary asset compatibility or reference-image parity.
